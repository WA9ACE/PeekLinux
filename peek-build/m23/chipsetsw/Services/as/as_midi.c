/**
 * @file
 *
 * @brief Definition of the BAE midi player management functions.
 *
 */

/*
 * History:
 *
 *	Date       	Author					Modification
 *	-------------------------------------------------------------------
 *	1/25/2005	f-maria@ti.com		Create.
 *
 * (C) Copyright 2005 by Texas Instruments Incorporated, All Rights Reserved
 */

#include "as/as_api.h"
#include "as/as_i.h"
#include "as/as_midi.h"
#include "as/as_processing.h"
#include "as/as_audio.h"
#include "as/as_utils.h"

#ifdef AS_OPTION_MIDI

#include "mBAE_PlayerCtrl.h"
#include "bae/bae_api_new.h"

#endif

#include "rvf/rvf_api.h"

#if ( AS_OPTION_LINEAR_FS == 1)
#include "ffs/ffs.h"
#endif

#if(AS_RFS_API == 1)
#include "rfs/rfs_api.h"
#endif

#ifdef AS_OPTION_MIDI

#if (AS_OPTION_CHANGE_PATH == 1)
/// DRC parameters for the stereo heaset output device.
static const mbParam as_midi_drc_headset[] =
{
    { mbDRC_Threshold,  -18000 }, //    0 to -60000
    { mbDRC_Ratio,        2000 }, // 1000 to  10000
    { mbDRC_AttackTime,    200 }, //    0 to   1000
    { mbDRC_ReleaseTime,  1000 }, //    0 to  10000
    { 0, 0 }
};


/// DRC parameters for the mono speaker output device.
static const mbParam as_midi_drc_speaker[] =
{
    { mbDRC_Threshold,   -9000 }, //    0 to -60000
    { mbDRC_Ratio,        1000 }, // 1000 to  10000
    { mbDRC_AttackTime,    100 }, //    0 to   1000
    { mbDRC_ReleaseTime,   200 }, //    0 to  10000
    { 0, 0 }
};
#elif (AS_OPTION_CHANGE_PATH == 0)
/// DRC parameters for the mono speaker output device.
static const mbParam as_midi_drc[] =
{
    { mbDRC_Threshold,   	BAE_DRC_THRESHOLD }, //    0 to -60000
    { mbDRC_Ratio,        	BAE_DRC_RATIO }, // 1000 to  10000
    { mbDRC_AttackTime,    	BAE_DRC_ATTACK_TIME }, //    0 to   1000
    { mbDRC_ReleaseTime,   	BAE_DRC_RELEASE_TIME }, //    0 to  10000
    { 0, 0 }
};
#else
#error "AS_OPTION_CHANGE_PATH is not defined"
#endif

// AS options for debug and/or work-around.

/// Compilation option.
/// Set AS_OPT_SKIP_AUDIO_CONFIG to 1 to get the audio configuration skipped.
#define AS_OPT_SKIP_AUDIO_CONFIG 0

/// Compilation option.
/// Set AS_OPT_MIDI_ALWAYS_USE_STEREO to 1 to always get BAE generating
/// stereo output. Ohterwise, mono is used for ringer and user
/// preference used for player.
#define AS_OPT_MIDI_ALWAYS_USE_STEREO 0

/// Fade time when a stop is required.
#define AS_MIDI_FADE_TIME 0

mbTime total_dur = 0, callback_position = 0, new_client_data = 0, old_client_data = 0;

/* LOCAL FUNCTION PROTOTYPES*/
void as_midi_kill_bae();
/**
 * Change the limit number of voices used in midi playback.
 *
 * First get the previous value in order to avoid to modify the
 * audio limit, then, set the new value according to the current mode.
 *
 * If the GPRS mode is active, only 16 voices are available.
 *
 * Otherwise, 24 voices are used in 'ringer' mode, while the user
 * parameter value is used in 'player' mode.
 *
 * @noparam
 *
 * @retval AS_OK         The processing is successful.
 * @retval AS_PLAYER_ERR An error occurred in the BAE player.
 */
T_AS_RET
as_midi_set_voice_limit(void)
{
    mbResult res;
    mbInt16 midi_limit;
    mbInt16 audio_limit;

    // Get previous values (audio_limit).
    res = baeGetRendererVoiceLimit(&midi_limit, &audio_limit);

    if (res != mbNo_Err)
    {
		as_midi_kill_bae();
        AS_TRACE_L(ERROR, "midi_set_voice_limit() baeGetRendererVoiceLimit(): error=", res);
        return (T_AS_RET)AS_PLAYER_ERR;
    }

    // Value if GPRS mode active.
    midi_limit = 16;

    // If GPRS mode not active, relax the constraint.
    if (!as_ctrl_blk->gprs_mode)
    {
        if (as_ctrl_blk->ringer_mode > AS_RINGER_MODE_NONE)
        {

#if( LOCOSTO_LITE == 1 )
     		// Locosto Lite configuration - Maximum value for a ringer.
            midi_limit = 16;
#else
      		// Maximum value for a ringer.
// OMAPS00100145, OMAPS00100166
            midi_limit = 32; //24;
#endif
        }
        else
        {
            // Value from Midi parameters setting.
            midi_limit = as_ctrl_blk->midi.params.voice_limit;
        }
    }

    res = baeSetRendererVoiceLimit(midi_limit, audio_limit);

    if (res != mbNo_Err)
    {
		as_midi_kill_bae();
        AS_TRACE_2(ERROR, "midi_set_voice_limit() baeSetRendererVoiceLimit(): error=%d, limit=%d", res, midi_limit);
        return (T_AS_RET)AS_PLAYER_ERR;
    }
    AS_DEBUG_L(HIGH, "midi_set_voice_limit() limit =", midi_limit);

    return AS_OK;
}


/**
 * Callback function called upon midi playback termination.
 *
 * This function is called back by the BAE core when a playback
 * terminates (due to an end of media reached).
 *
 * The main processing is to act as if a stop request was required,
 * but setting the end_of_media context variable.
 *
 * @param client_data @in Client data. Not used.
 *
 * @noreturn
 */
static void
as_midi_terminated_cb(void* client_data)
{
    AS_DEBUG_0(HIGH, "midi_terminated_cb() called");

    // End of media reached.
    as_ctrl_blk->end_of_media = TRUE;

    // Request internal stop.
    as_send_internal_stop_req();
}

static void
as_midi_position_cb(void* dummy_client_data,
                    mbTime requestedPos,
                    mbTime actualPos )
{
    mbTime* client_data;
     T_AS_PROBAR_IND_MP3 *  msg;
    T_RVF_MB_STATUS mb_status;
    mbResult res=mbNo_Err; 
    const T_RV_RETURN_PATH* rp = &as_ctrl_blk->client_rp;
    //rp=&as_ctrl_blk->addr_id;
   client_data= (mbTime *) dummy_client_data;
		AS_TRACE_3(ERROR, "as_midi_position_cb(): position(ms):requested=%d,actual=%d,total=%d.", (UINT32)requestedPos, (UINT32)actualPos,(UINT32) total_dur);
 
           mbRemovePlayerPositionCallback(as_ctrl_blk->midi.player,*client_data);
            if (res != mbNo_Err)
            {
				AS_TRACE_L(ERROR,
						   "as_midi_position_cb() baeRemovePlayerPositionCallback(): error=",
						   res);
	      }
		else
		{
			AS_DEBUG_1(HIGH, "as_midi_position_cb() position %d callback removed",*client_data);
		}
			/*if(as_ctrl_blk->loop && *client_data >= song_duration)
			{
				client_data = 0;
			}
*/			*client_data = *client_data +1000;            
			new_client_data = *client_data;
            if(/*as_ctrl_blk->loop ||*/ *client_data < total_dur) 
			{
                res = mbAddPlayerPositionCallback(as_ctrl_blk->midi.player, as_midi_position_cb, client_data,  *client_data, mbTrue);
				if (res != mbNo_Err)
				{
					AS_TRACE_L(ERROR,
							"as_midi_position_cb() baeAddPlayerPositionCallback(): error=",
							res);
				}
				else
				{
					AS_DEBUG_1(HIGH, "as_midi_position_cb() position %d callback set",*client_data);
				}
            		}


	if(as_ctrl_blk->loop && *client_data >= (total_dur - 1001))
	{
		*client_data = 0;

		callback_position=1000;		
		res = mbAddPlayerPositionCallback(as_ctrl_blk->midi.player,
                                               as_midi_position_cb,
                                               &callback_position,
                                               callback_position,
                                               mbTrue);

                if (res != mbNo_Err)
                {
                    AS_TRACE_L(ERROR,
                               "midi_create_player() baeAddPlayerPositionCallback(): error=",
                               res);
                }
                else
                {
                    AS_DEBUG_0(HIGH, "midi_create_player() position callback set");
                }
		
	}



    // Allocate the memory for the message to send.
    mb_status = rvf_get_buf(as_ctrl_blk->mb_external,
                            sizeof(T_AS_PROBAR_IND_MP3),
                            (T_RVF_BUFFER **) &msg);

    // If insufficient resources, report a memory error and return.
    if (mb_status != RVF_RED)
    {
        if (mb_status == RVF_YELLOW)
        {
            AS_TRACE_L(WARNING,
                       "send_progress_bar_ind() memory usage reached the threshold, mb_id=",
                       as_ctrl_blk->mb_external);
        }

        // Fill the message.

        // System information.
        msg->header.msg_id       = AS_PROBAR_IND;
        msg->header.src_addr_id  = as_ctrl_blk->addr_id;
        msg->header.dest_addr_id = rp->addr_id;
	 msg->u32totalTimeEst = total_dur/1000;
	 msg->u32TotalTimePlayed = actualPos/1000;
	 msg->media_type = AS_PLAYER_TYPE_MIDI;

        msg->status = AS_OK;

//	 AS_DEBUG_2(LOW, "u32totalTimeEst->%d u32TotalTimePlayed->%d", msg ->u32totalTimeEst,msg->u32TotalTimePlayed);
        
        // Use return path to send back the indication.
        if (rp->callback_func != NULL)
        {
            // Send the message using callback function.
            rp->callback_func((void *) msg);

            AS_DEBUG_0(LOW, "send_progress_bar_ind() successfully called callback function.");

            // Free the allocated message.
            if (rvf_free_buf(msg) != RVF_OK)
            {
                AS_TRACE_0(WARNING,
                           "send_progress_bar_ind() unable to free message.");
            }
        }
        else
        {
            // Send the message using the addess id.
            if (rvf_send_msg(rp->addr_id, msg) != RVF_OK)
            {
                AS_TRACE_0(ERROR, "send_progress_bar_ind() unable to send message.");
                rvf_free_buf(msg);
            }
            else
            {
                AS_DEBUG_0(LOW, "send_progress_bar_ind() successfully sent message.");
            }
        }
    }
    else
    {
        AS_TRACE_L(ERROR,
                   "send_progress_bar_ind() not enough memory to allocate the message, mb_id=",
                   as_ctrl_blk->mb_external);
    }
        
}


void 
as_terminated_l1_conf_cb()
{
    T_AS_INTERNAL_BAE_IND*  msg;
    T_RVF_MB_STATUS mb_status;

	mbResult res = mbNo_Err; //0;omaps00090550
	
    AS_DEBUG_0(HIGH, " function : as_terminated_l1_conf_cb ");
   if (as_ctrl_blk->midi.player != 0)
   {
        // Stop the player and destroy it (5ms fade time).
        res = baeStop(as_ctrl_blk->midi.player, AS_MIDI_FADE_TIME);
        if (res != mbNo_Err)
        {
            AS_TRACE_L(ERROR, "midi_destroy_player() baeStop(): error=", res);
        }
        else
        {
            AS_DEBUG_0(HIGH, "midi_destroy_player() player stopped");
        }

        // Wait fading.
        if (AS_MIDI_FADE_TIME > 0)
        {
            rvf_delay(RVF_MS_TO_TICKS(AS_MIDI_FADE_TIME));
        }

        res = baeDestroy(as_ctrl_blk->midi.player);
        if (res != mbNo_Err)
        {
            AS_TRACE_L(ERROR,
                       "midi_destroy_player() baeDestroy(): error=", res);
        }
        else
        {
            AS_DEBUG_0(HIGH, "midi_destroy_player() player destroyed");
        }

		as_midi_kill_bae();
        // Player is not usable any more.
        as_ctrl_blk->midi.player = 0;
    }
   
   
    // Allocate the memory for the message to send.
    mb_status = rvf_get_buf(as_ctrl_blk->mb_external,
                            sizeof(T_AS_INTERNAL_BAE_IND),
                            (T_RVF_BUFFER **) &msg);

    // If insufficient resources, report a memory error and return.
    if (mb_status != RVF_RED)
    {
        if (mb_status == RVF_YELLOW)
        {
            AS_TRACE_L(WARNING,
                       "send_internal_stop_req() memory usage reached the threshold, mb_id=",
                       as_ctrl_blk->mb_external);
        }

        // Fill the message.

        // System information.
        msg->header.msg_id       = AS_INTERNAL_BAE_IND;
        msg->header.src_addr_id  = as_ctrl_blk->addr_id;
        msg->header.dest_addr_id = as_ctrl_blk->addr_id;

        // Send the message using the addess id.
        if (rvf_send_msg(as_ctrl_blk->addr_id, msg) != RVF_OK)
        {
            AS_TRACE_0(ERROR, "send_internal_stop_req() unable to send message.");
            rvf_free_buf(msg);
        }
        else
        {
            AS_DEBUG_0(LOW,
                       "send_internal_stop_req() successfully sent message.");
        }
    }
    else
    {
        AS_TRACE_L(ERROR,
                   "send_internal_stop_req() not enough memory to allocate the message, mb_id=",
                   as_ctrl_blk->mb_external);
    } 
}


/**
 * Create the BAE midi player instance.
 *
 * This function performs the following processing to initialize
 * the midi player:
 * - Create the player and load the memory or file containing the data.
 * - Set the looping feature (or not).
 * - Set and engage the midi DRC parameters depending on the output device.
 * - Define the termination callback in case the end of media is reached.
 * - Set the mono/stereo mode (number of output channels).
 * - Set the number of voices to play simultaneously (voice limit).
 *
 * @param msg @in The start request from the AS client.
 *
 * @retval AS_OK         The processing is successful.
 * @retval AS_PLAYER_ERR An error occurred in the BAE player.
 */
static T_AS_RET
as_midi_create_player(const T_AS_START_REQ* msg)
{
    mbResult res = mbNo_Err; //0; //ompas00090550
    UINT32 numsecs;
#if ( AS_OPTION_LINEAR_FS == 1)
    UINT32 * 	linear_file_start_address = 0;
    UINT32 	linear_file_size = 0;
    req_id_t	return_val = 0;
#endif


if(as_ctrl_blk->play_bar_on == PLAYBAR_SUPPORTED)
    as_ctrl_blk->loop = msg->loop;

	bae_init();
    if (res != mbNo_Err)
    {
		as_midi_kill_bae();
        AS_TRACE_L(ERROR,
                   "midi_create_player() bae_init(): error=", res);
        return (T_AS_RET)AS_PLAYER_ERR;
    }
	
    bae_start();
    if (res != mbNo_Err)
    {
		as_midi_kill_bae();
        AS_TRACE_L(ERROR,
                   "midi_create_player() bae_start(): error=", res);
        return (T_AS_RET)AS_PLAYER_ERR;
    }

    // Create the midi player and load it with data.
    if (msg->data_location == AS_LOCATION_FILE)
    {

	/* x0083025 - OMAPS00156759 - Jan 29, 2008 - Start*/
		as_ctrl_blk->midi.player = baeCreateAndLoadPlayerFromFile(
									(mbFilePath)msg->data.filename, 
									mbTrue, &res);
	/* x0083025 - OMAPS00156759 - Jan 29, 2008 - End*/

    }
#if ( AS_OPTION_LINEAR_FS == 1)
    else if (msg->data_location == AS_LOCATION_LINEAR_FILE)
    {
       /// Open the file and get the start address of the LFS file. If 
       /// not able to open, send a START_IND with status as 
       /// AS_LFS_ERR.
       as_ctrl_blk->linear_file_descriptor =  lfs_open(msg->data.filename, 
       									FFS_O_RDONLY, 
       									(unsigned int *) &linear_file_size);

	if( as_ctrl_blk->linear_file_descriptor < EFFS_OK )
	{
		AS_TRACE_L(ERROR,
                        "LFS file open failed with error:",
                         as_ctrl_blk->linear_file_descriptor);
		as_ctrl_blk->linear_file_descriptor = NULL;
		return AS_LFS_ERR;
	}

	return_val = lfs_read( as_ctrl_blk->linear_file_descriptor, (char**) &linear_file_start_address);

	if( return_val < EFFS_OK )
	{
		AS_TRACE_L(ERROR,
                        "LFS file read failed with error:",
                        return_val);
		return AS_LFS_ERR;
	}

	as_ctrl_blk->midi.player
            = baeCreateAndLoadPlayerFromMemory(linear_file_start_address,
                                               linear_file_size,
                                               mbTrue,
                                               &res);
       
       /// Create MIDI player
    }
#endif
    else // AS_LOCATION_MEMORY
    {
        as_ctrl_blk->midi.player
            = baeCreateAndLoadPlayerFromMemory(msg->data.memory.address,
                                               msg->data.memory.size,
                                               mbTrue,
                                               &res);
    }

    if (res != mbNo_Err)
    {
        as_ctrl_blk->midi.player = NULL;
		as_midi_kill_bae();
        AS_TRACE_2(ERROR,
                   "midi_create_player() baeCreateAndLoadPlayerFrom%s(): error %d.",
                   (msg->data_location == AS_LOCATION_FILE)
                   ? "File"
                   : "Memory",
                   res);
        return (T_AS_RET)AS_PLAYER_ERR;
    }
    AS_DEBUG_L(HIGH, "midi_create_player() created player =",
               (UINT32) as_ctrl_blk->midi.player);

    // Set the looping feature according to the request.
    res = baeEnableLooping(as_ctrl_blk->midi.player, msg->loop ? mbTrue : mbFalse);

    if (res != mbNo_Err)
    {
		as_midi_kill_bae();
        AS_TRACE_L(ERROR,
                   "midi_create_player() baeEnableLooping(): error=", res);
        return (T_AS_RET)AS_PLAYER_ERR;
    }

#if (AS_OPTION_CHANGE_PATH == 1)
    // Set the DRC parameters according to the output device.
    res = baeSetDRCParameters(as_ctrl_blk->midi.player,
                              ((as_ctrl_blk->ringer_mode > AS_RINGER_MODE_NONE) ||
                               (as_ctrl_blk->params.output_device == AS_DEVICE_SPEAKER))
                              ? as_midi_drc_speaker
                              : as_midi_drc_headset);
#elif (AS_OPTION_CHANGE_PATH == 0) 
    // Set the DRC parameters according to the output device.
    res = baeSetDRCParameters(as_ctrl_blk->midi.player,
                              as_midi_drc);
#endif
    if (res != mbNo_Err)
    {
		as_midi_kill_bae();
        AS_TRACE_L(ERROR,
                   "midi_create_player() baeSetDRCParameters(): error=", res);
        return (T_AS_RET)AS_PLAYER_ERR;
    }

    // Use the DRC parameters for the midi player.
    res = baeEngageDRC(as_ctrl_blk->midi.player, mbTrue);

    if (res != mbNo_Err)
    {
		as_midi_kill_bae();
        AS_TRACE_L(ERROR, "midi_create_player() baeEngageDRC(): error=", res);
        return (T_AS_RET)AS_PLAYER_ERR;
    }

#if (AS_OPTION_CHANGE_PATH == 1)
    AS_DEBUG_L(HIGH, "midi_create_player() DRC engaged, device =", as_ctrl_blk->params.output_device);
#endif

    // Define the callbak function to call upon player termination.
    res = baeSetPlayerTerminationCallback(as_ctrl_blk->midi.player,
                                          as_midi_terminated_cb,
                                          NULL,
                                          mbTrue);

    if (res != mbNo_Err)
    {
		as_midi_kill_bae();
        AS_TRACE_L(ERROR, "midi_create_player() baeEngageDRC(): error=", res);
        return (T_AS_RET)AS_PLAYER_ERR;
    }

if(as_ctrl_blk->play_bar_on == PLAYBAR_SUPPORTED)
{
     AS_DEBUG_0(HIGH, "should not enter this in ringer mode ");
        // Set position callback.
        total_dur = baeGetDuration(as_ctrl_blk->midi.player, &res );
        if (res != mbNo_Err)
        {
            AS_TRACE_L(ERROR,
                       "midi_create_player() baeGetDuration(): error=",
                       res);
        }
        else
        {
            AS_DEBUG_0(HIGH, "midi_create_player() got duration");
        }


        // Set position callback.
             new_client_data = 0, old_client_data = 0;
		callback_position=1000;		
		res = mbAddPlayerPositionCallback(as_ctrl_blk->midi.player,
                                               as_midi_position_cb,
                                               &callback_position,
                                               callback_position,
                                               mbTrue);

                if (res != mbNo_Err)
                {
                    AS_TRACE_L(ERROR,
                               "midi_create_player() baeAddPlayerPositionCallback(): error=",
                               res);
                }
                else
                {
                    AS_DEBUG_0(HIGH, "midi_create_player() position callback set");
                }
}
    
    
    // Set the number of output channels (mono/stereo).
#if (AS_OPT_MIDI_ALWAYS_USE_STEREO == 0)
    res = baeSetOutputHardwareChannelCount((as_ctrl_blk->ringer_mode > AS_RINGER_MODE_NONE)
                                           ? 1
                                           : as_ctrl_blk->midi.params.output_channels);
    AS_DEBUG_L(HIGH, "midi_create_player() channel count = ",
               (as_ctrl_blk->ringer_mode > AS_RINGER_MODE_NONE)
               ? 1
               : as_ctrl_blk->midi.params.output_channels);
#else
    res = baeSetOutputHardwareChannelCount(2);
    AS_DEBUG_0(HIGH, "midi_create_player() channel count = 2");
#endif

    if (res != mbNo_Err)
    {
#if 0
		as_midi_kill_bae();
        AS_TRACE_L(ERROR, "midi_create_player() baeSetOutputHardwareChannelCount(): error=", res);
        return AS_PLAYER_ERR;
#else
        /// @todo Temporary work-around to use mono/stereo.
        AS_TRACE_L(WARNING, "midi_create_player() baeSetOutputHardwareChannelCount(): error=", res);
#endif
    }

    // Set the limit of simultaneous voices.

    // Trace already logged in as_midi_set_voice_limit().
    return as_midi_set_voice_limit();
}


/**
 * Destroy the BAE midi player instance.
 *
 * This function performs the following processing to terminate
 * the midi player:
 * - Remove the termination callback to prevent BAE core to call it.
 * - Actually stop the player (using fade time if any).
 * - Wait the fade time, and disengage the output hardware (audio/L1).
 * - Destroy the player object.
 *
 * @noparam
 *
 * @noreturn
 */
static void
as_midi_destroy_player(void)
{
	mbBool engaged;	
#if (AS_OPTION_LINEAR_FS == 1)
    effs_t return_val = 0;
#endif
 
    if (as_ctrl_blk->midi.player != 0)
    {
        mbResult res = mbNo_Err; //0;omaps00090550
        // Do not need to call the termination callback any more.
		res = baeSetPlayerTerminationCallback(as_ctrl_blk->midi.player,
                                              NULL,
                                              NULL,
                                              mbTrue);
		if (res != mbNo_Err)
		{	
			AS_TRACE_L(ERROR,
				"midi_destroy_player() baeSetTerminationCallback(): error=",
				res);
		}
		else
		{	
			AS_DEBUG_0(HIGH, "midi_destroy_player() termination callback reset");
		}

		
		/*** x0086292,OMAPS00166448 ****/	
		/*** start ***/
		/*engaged = baeIsDRCEngaged(bae_env_ctrl_blk_p->system, &res);
		if (!engaged)
		{
			AS_DEBUG_0(HIGH, "midi_destroy_player() not engaged ");
			as_terminated_l1_conf_cb();
		}
			else
		{
			AS_DEBUG_0(HIGH, "midi_destroy_player() engaged ");
		}
		/**** End ****/
		
        res = baeDisengageOutputHardware();
        if (res != mbNo_Err)
        {
            AS_TRACE_L(ERROR,
                       "midi_destroy_player() mbDisengageOutputHardware(): error=", res);
        }
        else
        {
            AS_DEBUG_0(HIGH, "midi_destroy_player() output hardware disengaged");
        }
		/*Apr 10,2008: x0082844(Shubhro) , OMAPS00167883 -->Start*/		//Destroying midi player after Output Hardware has been Disengaged
		engaged = baeIsDRCEngaged(bae_env_ctrl_blk_p->system, &res);
				if (!engaged)
				{
					AS_DEBUG_0(HIGH, "midi_destroy_player() not engaged ");
					as_terminated_l1_conf_cb();
				}
					else
				{
					AS_DEBUG_0(HIGH, "midi_destroy_player() engaged ");
				}
		/*Apr 10,2008: x0082844(Shubhro) , OMAPS00167883 -->End*/		
	}
	else
    {
	as_ctrl_blk->loop = 0;
        AS_TRACE_0(ERROR, "midi_destroy_player() player not created.");
    }

#if (AS_OPTION_LINEAR_FS == 1)
    /// Close the LFS file
    if( NULL != as_ctrl_blk->linear_file_descriptor )
    {
    	return_val = lfs_close(as_ctrl_blk->linear_file_descriptor);

	as_ctrl_blk->linear_file_descriptor = NULL;

	if(return_val != EFFS_OK)
	{
	   AS_TRACE_0(ERROR, "midi_destroy_player() player not created.");
	}
    }
#endif

}


/**
 * Prepare audio configuration and midi player to a future play.
 *
 * This function performs the following processing to prepare
 * the midi playing:
 * - Create the midi player and initialize it.
 * - Call the audio configuration chain (see as_audio_fa_write()).
 *
 * If any error occurred, destroy the player and return an error.
 *
 * @param msg @in The start request from the AS client.
 *
 * @retval AS_OK         The processing is successful.
 * @retval AS_PLAYER_ERR An error occurred in the BAE player.
 * @retval AS_AUDIO_ERR  An error occurred in the Audio SWE.
 */
T_AS_RET
as_midi_prepare_setting(const T_AS_START_REQ* msg)
{
    // Create the BAE player and initialize it.
    T_AS_RET rc = as_midi_create_player(msg);

    if (rc == AS_OK)
    {
#if (AS_OPT_SKIP_AUDIO_CONFIG == 1)
        // call player start whithout setting audio.
        return as_midi_start_player();
#else
        // Define audio configuration before to change it.
        rc = as_audio_fa_read();

        if (rc == AS_OK)
        {
            // Now start the audio parameters setting callback chain.
            // Transition: change to SETTING state.
            if (as_audio_fa_write(AS_STATE_SETTING) != AS_OK)
            {
                // Error while configuring audio parameters.
                as_midi_destroy_player();
                rc = (T_AS_RET)AS_AUDIO_ERR;
            }
        }
        else
        {
            // Error while reading audio parameters.
            (void) as_midi_destroy_player();
        }
#endif
    }
    else
    {
        // Error while creating and configuring BAE player.
        (void) as_midi_destroy_player();
    }

    return rc;
}


/**
 * Actually start the midi player.
 *
 * This function starts the midi player by:
 * - Engaging the output hardware (audio/L1).
 * - Starting the BAE player instance.
 *
 * @noparam
 *
 * @retval AS_OK         The processing is successful.
 * @retval AS_PLAYER_ERR An error occurred in the BAE player.
 */
T_AS_RET
as_midi_start_player(void)
{
    T_AS_RET rc  = AS_OK;
    mbResult res = mbNo_Err; //omaps00090550 0;

    // Engage the audio hadware.
    res = baeEngageOutputHardware();
    if (res != mbNo_Err)
    {
        AS_TRACE_L(ERROR, "midi_start_player() baeEngageOutputHardware(): error=", res);
        rc = (T_AS_RET)AS_PLAYER_ERR;
    }
    else
    {
        AS_DEBUG_0(HIGH, "midi_start_player() output hardware engaged");
    }

    // Start the BAE player.
    res = (mbResult) baeStart(as_ctrl_blk->midi.player);
    if (res != mbNo_Err)
    {
        AS_TRACE_L(ERROR, "midi_start_player() baeStart(): error=", res);
        rc = (T_AS_RET)AS_PLAYER_ERR;
    }
    else
    {
        AS_DEBUG_0(HIGH, "midi_start_player() player started");
        // Change to PLAYING state.
        as_process_to_playing_state(AS_OK);
    }
	if(AS_PLAYER_ERR == rc)
		as_midi_kill_bae();
    return rc;
}

/**
 *Pause the midi player.
 *
 * This function pauses player by:
 *
 * @noparam
 *
 * @retval AS_OK         The processing is successful.
 * @retval AS_PLAYER_ERR An error occurred in the BAE player.
 */
T_AS_RET
as_midi_pause_player(void)
{
    T_AS_RET rc  = (T_AS_RET)AS_OK;
    mbResult res = mbNo_Err; //omaps00090550 0;

     /* Send a message and change the state to PAUSE*/
    res = baePause(as_ctrl_blk->midi.player); //Pause the player

    if (res != mbNo_Err)
    {
   
        AS_TRACE_L(ERROR, "as_midi_pause_player() baePause(): error=", res);
        rc = (T_AS_RET)AS_PLAYER_ERR;
    }
    else
    {
        AS_DEBUG_0(HIGH, "as_mide_pause_player() completed");
        
        // Everything is OK. Transition to PAUSE state.
	 AS_CHANGE_STATE(AS_STATE_PAUSED);

	}
	if(AS_PLAYER_ERR == rc)
		as_midi_kill_bae();
	return rc;
}


/**
 *Resume the midi player.
 *
 * This function resumes player by:
 *
 * @noparam
 *
 * @retval AS_OK         The processing is successful.
 * @retval AS_PLAYER_ERR An error occurred in the BAE player.
 */
T_AS_RET
as_midi_resume_player(void)
{
    T_AS_RET rc  = (T_AS_RET)AS_OK;
    mbResult res = mbNo_Err; //omaps00090550 0;

    /* Send a message and change the state to PAUSE*/
   res = baeResume(as_ctrl_blk->midi.player); //Resume the player
   
    if (res != mbNo_Err)
    {
        AS_TRACE_L(ERROR, "as_midi_resume_player() baeResume(): error=", res);
        rc = (T_AS_RET)AS_PLAYER_ERR;
    }
    else
    {

        AS_DEBUG_0(HIGH, "as_mide_resume_player() completed");
        
        // Everything is OK. Transition to RESUME state.
	 AS_CHANGE_STATE(AS_STATE_PLAYING);
    }
	if(AS_PLAYER_ERR == rc)
		as_midi_kill_bae();
   return rc;
}

/**
 *forward or rewind the play position of midi player.
 *
 * This function forwards or rewind the play position by media_skip_time units:
 *
 * @noparam
 *
 * @retval AS_OK         The processing is successful.
 * @retval AS_PLAYER_ERR An error occurred in the BAE player.
 */
T_AS_RET
as_midi_forrew_player(void)
{
    T_AS_RET rc  = (T_AS_RET)AS_OK;
    mbResult res = mbNo_Err; //omaps00090550 0;
     mbTime cur_pos=0;

     /* Send a message and change the state to PAUSE*/


   if(as_ctrl_blk->forrew == FORWARD)
   	{
   	new_client_data = new_client_data + ((as_ctrl_blk->midi.params.media_skip_time )* 1000) ;
		
      cur_pos = baeGetPosition (as_ctrl_blk->midi.player, &res );
      if (total_dur > (cur_pos +((as_ctrl_blk->midi.params.media_skip_time )*1000)))
      res = baeSetPosition(as_ctrl_blk->midi.player,cur_pos +((as_ctrl_blk->midi.params.media_skip_time )* 1000)); //forward/rewind the player
      else
    	{
    	as_midi_stop_player();
      	}

		
   	}
   else if (as_ctrl_blk->forrew == REWIND)
   	{
	new_client_data = new_client_data - ((as_ctrl_blk->midi.params.media_skip_time )* 1000) ;   	

	
 
   cur_pos = baeGetPosition (as_ctrl_blk->midi.player, &res );
    if (((as_ctrl_blk->midi.params.media_skip_time )*1000) < cur_pos)
    	{
    res = baeSetPosition(as_ctrl_blk->midi.player,cur_pos-((as_ctrl_blk->midi.params.media_skip_time) * 1000));
    	}
  
    else
    	{
    		as_midi_start_player();
		return rc;
    	}   

	
   	}

    if (res != mbNo_Err)
    {
   
        AS_TRACE_L(ERROR, "as_midi_forrew_player() baeSetPosition(): error=", res);
        rc = (T_AS_RET)AS_PLAYER_ERR;
    }
    else
    {
        AS_DEBUG_0(HIGH, "as_midi_forrew_player() completed");

		old_client_data = callback_position;

		 mbRemovePlayerPositionCallback(as_ctrl_blk->midi.player,old_client_data);
            if (res != mbNo_Err)
            {
				AS_TRACE_L(ERROR,
						   "as_midi_position_cb() baeRemovePlayerPositionCallback(): error=",
						   res);
	      }
			else
			{
				AS_DEBUG_1(HIGH, "as_midi_position_cb() position %d callback removed",old_client_data);
			}

         // Set position callback.
		callback_position=new_client_data;		
		res = mbAddPlayerPositionCallback(as_ctrl_blk->midi.player,
                                               as_midi_position_cb,
                                               &callback_position,
                                               new_client_data,
                                               mbTrue);

                if (res != mbNo_Err)
                {
                    AS_TRACE_L(ERROR,
                               "midi_create_player() baeAddPlayerPositionCallback(): error=",
                               res);
                }
                else
                {
                    AS_DEBUG_0(HIGH, "midi_create_player() position callback set");
                }
	
           
    }
	if(AS_PLAYER_ERR == rc)
		as_midi_kill_bae();
	return rc;
}


/**
 * Stop the midi player.
 *
 * This function stop the midi player by performing the following steps:
 * - Destroy the player.
 * - Retore audio configuration.
 *
 * @noparam
 *
 * @retval AS_OK         The processing is successful.
 * @retval AS_AUDIO_ERR  An error occurred in the Audio SWE.
 */
T_AS_RET
as_midi_stop_player(void)
{
    T_AS_RET rc = (T_AS_RET)AS_OK;

    // Destroy the BAE player.
    as_midi_destroy_player();

    // Transition: change to STOPPING state.
    AS_CHANGE_STATE(AS_STATE_STOPPING);

    /*
#if (AS_OPT_SKIP_AUDIO_CONFIG == 1)
    as_restoring_do_fa_write_done(0);
#else
    rc = as_audio_fa_write(AS_STATE_RESTORING);
#endif
    */
    return rc;
}

T_AS_RET 
as_stopping_do_midi_bae_ind()
{
 T_AS_RET rc = AS_OK;
 AS_DEBUG_0(HIGH, " function : as_stoping_do_midi_bae_con");

    // Now start the audio parameters setting callback chain.
    // Transition: change to RESTORING state.

#if (AS_OPT_SKIP_AUDIO_CONFIG == 1)
    as_restoring_do_fa_write_done(0);
#else
    rc = as_audio_fa_write(AS_STATE_RESTORING);
#endif

    return rc;
}

void as_midi_kill_bae()
{
		mbResult res = mbNo_Err;
		res = bae_stop(NULL);
        if (res != mbNo_Err)
        {
            AS_TRACE_L(ERROR,
                       "as_midi_kill_bae() bae_stop(): error=", res);
        }
        else
        {
            AS_DEBUG_0(HIGH, "as_midi_kill_bae() engine stopped");
        }
		
		res = bae_kill();
        if (res != mbNo_Err)
        {
            AS_TRACE_L(ERROR,
                       "as_midi_kill_bae() bae_kill(): error=", res);
        }
        else
        {
            AS_DEBUG_0(HIGH, "as_midi_kill_bae() engine destroyed");
        }
}

#endif


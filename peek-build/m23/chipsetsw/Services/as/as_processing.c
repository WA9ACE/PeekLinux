/**
 * @file
 *
 * @brief Definition of the message processing functions.
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


#include "as/as_processing.h"
#include "as/as_env.h"
#include "as/as_i.h"
#include "as/as_message.h"
#include "as/as_audio.h"
#include "as/as_buzzer.h"
#include "as/as_midi.h"
#include "as/as_utils.h"



#include "audio/audio_api.h"
#include "tty/tty_api.h"
#include "audio/audio_compatibilities.h"



// ----------------------------------------------------------------------------
//   COMMON COMMON COMMON COMMON COMMON COMMON COMMON COMMON COMMON COMMON
// ----------------------------------------------------------------------------


/**
 * This private function saves the START request so that it can be
 * processed the next time the AS state changes to IDLE.
 *
 * In the case there is already a saved request, the previous one is freed
 * and an error is logged.
 *
 * @param msg @in The start request to save.
 *
 * @noreturn
 */
void
as_request_save(const T_AS_START_REQ* msg)
{
    if (as_ctrl_blk->saved != NULL)
    {
        // Error: a saved request exists !
        AS_TRACE_0(ERROR,
                   "request_save() saved request already exist ! Freeing it.");

        rvf_free_buf((T_RVF_BUFFER*) as_ctrl_blk->saved);
    }

    // Anyway, save the start request now.
    as_ctrl_blk->saved    = msg;
    as_ctrl_blk->free_msg = FALSE;

    AS_DEBUG_L(HIGH, "request_save() saved request =", (UINT32) msg);
}


/**
 * This private function restore a previously stored START request.
 *
 * @noparam
 *
 * @return A pointer to the saved START request, or NULL if no request was
 *         previously saved.
 */
const T_AS_START_REQ*
as_request_restore(void)
{
    // Get current possible saved request.
    const T_AS_START_REQ* msg = as_ctrl_blk->saved;

    // Allways reset the saved request.
    as_ctrl_blk->saved = NULL;

    if (msg != NULL)
    {
        AS_DEBUG_L(HIGH, "request_restore() restored request =", (UINT32) msg);
    }

    // Return the saved request (may be NULL).
    return msg;
}




/**
 * This private function manage audio incompatibilities.
 *
 * It get existing running audio jobs and according to the
 * audio compatibility matrix, asks audio to terminate the current
 * incompatible job.
 *
 * @param msg @in The start request from the AS client.
 *
 * @retval AS_OK  Processing was successful.
 *
 * @todo Finish implementation/Check.
 * @todo Check rc of rvf_*_mutex().
 */
T_AS_RET
as_manage_incompatibilities(const T_AS_START_REQ* msg)
{
    T_AS_RET rc = AS_OK;
    return rc;
}




// ----------------------------------------------------------------------------
//   IDLE IDLE IDLE IDLE IDLE IDLE IDLE IDLE IDLE IDLE IDLE IDLE IDLE IDLE IDLE
// ----------------------------------------------------------------------------


/**
 * Perform common processing before to change to IDLE state.
 *
 * If the current player was not interrupted, a normal
 * STOP_IND message is sent back to the AS client.
 * Else (player interrupted) nothing is done. The saved START request will be
 * processed when returning to the as_env_handle_message() function.
 *
 * Finally, the AS state is changed to IDLE, and the player type is
 * reset to NONE.
 *
 * @note The value of player type is important as it determines the way
 *       the as_adapt_gprs_mode() function behaves.
 *
 * @param status       @in The status to reply in the STOP_IND message.
 * @param end_of_media @in The end of media flag to reply in STOP_IND.
 *
 * @noreturn
 */
void
as_process_to_idle_state(T_AS_RET status, BOOLEAN end_of_media)
{
    AS_DEBUG_2(HIGH, "process_to_idle_state() status: %d end_of_media %d",
               status, end_of_media);

    if (!as_ctrl_blk->interrupted)
    {
        // Send back the STOP indication.
        as_send_stop_ind(status, end_of_media, &as_ctrl_blk->client_rp);
    }
    /* else interrupted: nothing to do, the saved message will be
     * processed when returning back to the
     * as_env_handle_message() function.
     */

    // Transition: change to IDLE state.
    AS_CHANGE_STATE(AS_STATE_IDLE);

    // Reset player type (used in as_adapt_gprs_mode()).
    as_ctrl_blk->player_type = AS_PLAYER_TYPE_NONE;
}


/**
 * Process the START_REQ message from IDLE state.
 *
 * Silent mode processing:
 * - Downgrade priority (ringer mode set to NONE) as a silent
 *   player/ringer shall not stop or prevent an audible one.
 * - Change the state to PLAYING.
 * - If the player/ringer is not intended to loop, simply
 *   change back to IDLE state.
 *
 * Audible mode processing:
 * - Check the player incompatibilities and if required, ask
 *   the AUDIO SWE to stop the current play back.
 *   Note this is only valid for melodies other than
 *   MIDI (or other new supported players).
 * - MIDI: prepare the settings.
 * - BUZZER: start the buzzer.
 *
 * Error processing:
 * - change back to the IDLE state.
 *
 * @param msg @in The start request from the AS client.
 *
 * @noreturn
 */
void
as_idle_do_start_req(const T_AS_START_REQ* msg)
{
    // Set the context.
    as_ctrl_blk->ringer_mode   = msg->ringer_mode;
    as_ctrl_blk->player_type   = msg->player_type;
    as_ctrl_blk->volume        = msg->volume;
    as_ctrl_blk->client_rp     = msg->rp;
    as_ctrl_blk->play_bar_on = msg->play_bar_on;
    as_ctrl_blk->recover       = 0;
    as_ctrl_blk->end_of_media  = FALSE;
    as_ctrl_blk->internal_stop = FALSE;
    as_ctrl_blk->error         = AS_OK;
  

    // Check for volume.
    if (msg->volume == AS_VOLUME_SILENT)
    {
        AS_DEBUG_0(HIGH, "idle_do_start_req() silent mode");

        // Silent mode implies downgraded priority.
        as_ctrl_blk->ringer_mode = AS_RINGER_MODE_NONE;

        // Change to PLAYING state.
        as_process_to_playing_state(AS_OK);

        // If the player is not intended to loop, terminate it.
        if (!msg->loop)
        {
            // Change to IDLE state.
            as_process_to_idle_state(AS_OK, TRUE);
        }
    }
    else
    {
        // Audible mode.

        T_AS_RET status = AS_OK;

        // --- FIND OUT PLAYER INCOMPATIBILITIES ---
        if (as_manage_incompatibilities(msg) == AS_OK)
        {
            switch (msg->player_type)
            {
#ifdef AS_OPTION_MIDI

            case AS_PLAYER_TYPE_MIDI:
                status = as_midi_prepare_setting(msg);
                break;
#endif

#if(AS_OPTION_MP3 == 1)
            case AS_PLAYER_TYPE_MP3:
		  status = (T_AS_RET)as_mp3_prepare_setting(msg);
                break;
#endif
#if(AS_OPTION_AAC == 1)
            case AS_PLAYER_TYPE_AAC:
		  status = (T_AS_RET)as_aac_prepare_setting(msg);
                break;
#endif

#if(AS_OPTION_E1_MELODY== 1)
            case AS_PLAYER_TYPE_E1_MELODY:
		  status = (T_AS_RET)as_e1_melody_prepare_setting(msg);
                break;
#endif
		
#if (AS_OPTION_BUZZER == 1)
            case AS_PLAYER_TYPE_BUZZER:
                status = as_buzzer_start_player(msg);
                break;
#endif // AS_OPTION_BUZZER

            default:
                AS_TRACE_L(ERROR,
                           "idle_do_start_req() IDLE state inconsistency: player_type=",
                           as_ctrl_blk->player_type);
                status = AS_INTERNAL_ERR;
                break;
            }
        }
        else
        {
            // Unable to manage incompatibilities.
            status = AS_INTERNAL_ERR;
        }

        if (status != AS_OK)
        {
            // Change back to IDLE state.
            as_process_to_idle_state(status, FALSE);
        }
    }
}




// ----------------------------------------------------------------------------
//   SETTING SETTING SETTING SETTING SETTING SETTING SETTING SETTING SETTING
// ----------------------------------------------------------------------------


/**
 * Process the AUDIO_FULL_ACCESS_WRITE_DONE message from SETTING state.
 *
 * Regular mode processing:
 * - MIDI: start the player.
 *
 * Recover & error mode processing:
 * - MIDI: destroy the created player and restore audio configuration.
 *
 * @param msg @in The response message from the audio SWE.
 *
 * @noreturn
 */
void
as_setting_do_fa_write_done(const T_AUDIO_FULL_ACCESS_WRITE_DONE* msg)
{
    T_AS_RET status = AS_OK;

    if (msg->status == AUDIO_OK)
    {
        if (as_ctrl_blk->recover == 0)
        {
            // Regular processing.
            switch (as_ctrl_blk->player_type)
            {
#ifdef AS_OPTION_MIDI

            case AS_PLAYER_TYPE_MIDI:
                status = as_midi_start_player();
                // Remember the error.
                as_ctrl_blk->error = status;
                break;
#endif

#if(AS_OPTION_MP3 == 1)
            case AS_PLAYER_TYPE_MP3:
                status =(T_AS_RET) as_mp3_start_player();
                // Remember the error.
                as_ctrl_blk->error = status;
                break;
#endif
#if(AS_OPTION_AAC == 1)
            case AS_PLAYER_TYPE_AAC:
                status = (T_AS_RET)as_aac_start_player();
                // Remember the error.
                as_ctrl_blk->error = status;
                break;
#endif

#if(AS_OPTION_E1_MELODY == 1)
            case AS_PLAYER_TYPE_E1_MELODY:
                status = (T_AS_RET)as_e1_melody_start_player();
                // Remember the error.
                as_ctrl_blk->error = status;
                break;
#endif

            default:
                AS_TRACE_L(ERROR,
                           "setting_do_fa_write_done() SETTING state inconsistency: player_type=",
                           as_ctrl_blk->player_type);
                status = AS_INTERNAL_ERR;
                // Remember the error.
                as_ctrl_blk->error = status;
                break;
            }
        }
        else
        {
            // Recover mode:
            // Only use the same processing as error, but do not remember an error.
            status = AS_INTERNAL_ERR;
        }
    }
    else
    {
        AS_TRACE_L(ERROR,
                   "setting_do_fa_write_done() audio fa write error, status =",
                   msg->status);
        // Error, switch to error processing.
        status = (T_AS_RET)AS_AUDIO_ERR;
        // Remember the error.
        as_ctrl_blk->error = status;
    }

    if (status != AS_OK)
    {
        // Error or recover processing.
        switch (as_ctrl_blk->player_type)
        {

#ifdef AS_OPTION_MIDI
        case AS_PLAYER_TYPE_MIDI:
            // Act as if there was a request to stop the player.
            // This also starts the restoring audio configuration chain.
            as_midi_stop_player();

            if (as_ctrl_blk->recover != 0)
            {

                // Error on error/recover, best effort: switch to IDLE state.
                as_process_to_idle_state(status, FALSE);
            }
            break;
#endif

#if(AS_OPTION_MP3 == 1)
        case AS_PLAYER_TYPE_MP3:
            // Error on error/recover, best effort: switch to IDLE state.
            as_process_to_idle_state(status, FALSE);
            break;
#endif
#if(AS_OPTION_AAC == 1)
        case AS_PLAYER_TYPE_AAC:
            // Error on error/recover, best effort: switch to IDLE state.
            as_process_to_idle_state(status, FALSE);
            break;
#endif

#if(AS_OPTION_E1_MELODY == 1)
        case AS_PLAYER_TYPE_E1_MELODY:
            // Error on error/recover, best effort: switch to IDLE state.
            as_process_to_idle_state(status, FALSE);
            break;
#endif

        default:
            // Switch back to IDLE state.
            as_process_to_idle_state(status, FALSE);
            break;
        }
    }
}




// ----------------------------------------------------------------------------
//   STARTING STARTING STARTING STARTING STARTING STARTING STARTING STARTING
// ----------------------------------------------------------------------------


// No STARTING state until MP3 implementation.




// ----------------------------------------------------------------------------
//   PLAYING PLAYING PLAYING PLAYING PLAYING PLAYING PLAYING PLAYING PLAYING
// ----------------------------------------------------------------------------


/**
 * Perform common processing before to change to PLAYING state.
 *
 * Send the START_IND message back to the AS client.
 *
 * Because the AS successfully changes to PLAYING state, the interrupted
 * flag is reset so that an interrupting (higher priority) ringer request
 * can be handled.
 *
 * @param status @in The status to reply in the START_IND message.
 *
 * @noreturn
 */
void
as_process_to_playing_state(T_AS_RET status)
{
    AS_DEBUG_1(HIGH, "process_to_playing_state() status: %d", status);

	//OMAPS00175540
	 if(as_ctrl_blk->bt_connect_status == TRUE)
		 {
		    if((as_ctrl_blk->player_type==AS_PLAYER_TYPE_MP3) ||(as_ctrl_blk->player_type==AS_PLAYER_TYPE_AAC))
        		{
					AS_CHANGE_STATE(AS_STATE_STARTING);  // Everything is OK. Transition to PLAYING state.
        		}
	 	}
   else
   {
    // Send the successful START indication message to the client.
    as_send_start_ind(status,
                      as_ctrl_blk->interrupted,
                      &as_ctrl_blk->client_rp);

    // Reset the interrupted flag.
    as_ctrl_blk->interrupted = FALSE;

    // Everything is OK. Transition to PLAYING state.
    AS_CHANGE_STATE(AS_STATE_PLAYING);
}
}


/**
 * Performs the real stop processing.
 *
 * Silent mode processing:
 * - Simply change back to the IDLE state.
 *
 * Audible mode processing:
 * - MIDI: stop the player (will need audio restoring).
 * - BUZZER: stop the player (no audio restoring required).
 *
 * Error processing:
 * - An error in stopping the midi player will result in changing to IDLE state.
 * - A bad player type will result in changing to IDLE state too.
 *
 * @noparam
 *
 * @noreturn
 */
void
as_playing_process_stop(void)
{
    T_AS_RET status = AS_OK;

    AS_TRACE_1(DEBUG_LOW, "playing_process_stop() player_type = %d", as_ctrl_blk->player_type);
 

    // Check for silent mode.
    if (as_ctrl_blk->volume == AS_VOLUME_SILENT)
    {
        // Silent player, nothing to stop.

        AS_DEBUG_0(HIGH, "playing_process_stop() silent mode");

        as_process_to_idle_state(AS_OK, FALSE);
    }
    else
    {
        // Audible mode.

        switch (as_ctrl_blk->player_type)
        {
#ifdef AS_OPTION_MIDI

        case AS_PLAYER_TYPE_MIDI:
            // Stops the midi player.
            // This also starts the restoring audio configuration chain.
            status = as_midi_stop_player();

            if (status != AS_OK)
            {
                // Error, best effort: switch to IDLE state.
                as_process_to_idle_state(status, FALSE);
            }
            break;
#endif 

#if(AS_OPTION_MP3 == 1)
            case AS_PLAYER_TYPE_MP3:
		  // Stops the mp3 player.
            	  // This changes the state to STOPPING.
                status = (T_AS_RET)as_mp3_stop_player();

            	  if (status != AS_OK)
            	  {
               	 // Error, best effort: switch to IDLE state.
                	as_process_to_idle_state(status, FALSE);
           	  }
             break;
#endif
#if(AS_OPTION_AAC == 1)
            case AS_PLAYER_TYPE_AAC:
		  // Stops the aac player.
            	  // This changes the state to STOPPING.
                status = (T_AS_RET)as_aac_stop_player();

            	  if (status != AS_OK)
            	  {
               	 // Error, best effort: switch to IDLE state.
                	as_process_to_idle_state(status, FALSE);
           	  }
             break;
#endif

#if(AS_OPTION_E1_MELODY== 1)
            case AS_PLAYER_TYPE_E1_MELODY:
		  // Stops the aac player.
            	  // This changes the state to STOPPING.
                status = (T_AS_RET)as_e1_melody_stop_player();

            	  if (status != AS_OK)
            	  {
               	 // Error, best effort: switch to IDLE state.
                	as_process_to_idle_state(status, FALSE);
           	  }
             break;
#endif

#if (AS_OPTION_BUZZER == 1)
        case AS_PLAYER_TYPE_BUZZER:
            (void) as_buzzer_stop_player(AS_OK, FALSE);
            break;
#endif // AS_OPTION_BUZZER

        default:
            AS_TRACE_L(ERROR,
                       "playing_process_stop() PLAYING state inconsistency: player_type=",
                       as_ctrl_blk->player_type);
            // Error, best effort: switch to IDLE state.
            as_process_to_idle_state(AS_INTERNAL_ERR, FALSE);
            break;
        }
    }
}

/**
 * Performs the pause processing.
 *
 * Silent mode processing:
 * - Simply change back to the IDLE state.
 *
 * Audible mode processing:
 * - MIDI: stop the player (will need audio restoring).
 * - BUZZER: stop the player (no audio restoring required).
 *
 * Error processing:
 * - An error in stopping the midi player will result in changing to IDLE state.
 * - A bad player type will result in changing to IDLE state too.
 *
 * @noparam
 *
 * @noreturn
 */
void
as_playing_to_pause_state(const T_AS_PAUSE_REQ* msg)
{
T_AS_RET status = AS_OK;

    AS_TRACE_1(DEBUG_LOW, "playing_process_paused() player_type = %d", as_ctrl_blk->player_type);
 

     switch (as_ctrl_blk->player_type)
        {
#ifdef AS_OPTION_MIDI

         case AS_PLAYER_TYPE_MIDI:
		  // Pause the midi player.
            	  // This changes the state to PAUSE.
                status = as_midi_pause_player();

            	  if (status != AS_OK)
            	  {
     AS_TRACE_1(DEBUG_LOW, "playing_process_paused()status = %d", status);
 
               	 // Error, best effort: switch to IDLE state.
                	as_process_to_idle_state(status, FALSE);
           	  }
             break;
#endif
        
#if(AS_OPTION_MP3 == 1)
            case AS_PLAYER_TYPE_MP3:
		  // Pause the mp3 player.
            	  // This changes the state to STOPPING.
                status = (T_AS_RET)as_mp3_pause_player();

            	  if (status != AS_OK)
            	  {
     AS_TRACE_1(DEBUG_LOW, "playing_process_paused()status = %d", status);
 
               	 // Error, best effort: switch to IDLE state.
                	as_process_to_idle_state(status, FALSE);
           	  }
             break;
#endif
#if(AS_OPTION_AAC == 1)
            case AS_PLAYER_TYPE_AAC:
		  // Pause the aac player.
            	  // This changes the state to STOPPING.
                status =(T_AS_RET) as_aac_pause_player();

            	  if (status != AS_OK)
            	  {
               	 // Error, best effort: switch to IDLE state.
                	as_process_to_idle_state(status, FALSE);
           	  }
             break;
#endif

        default:
            AS_TRACE_L(ERROR,
                       "playing_process_stop() PLAYING state inconsistency: player_type=",
                       as_ctrl_blk->player_type);
            // Error, best effort: switch to IDLE state.
            as_process_to_idle_state(AS_INTERNAL_ERR, FALSE);
            break;
        }
}



/**
 * Performs the resume processing.
 *
 * Silent mode processing:
 * - Simply change back to the IDLE state.
 *
 * Audible mode processing:
 * - MIDI: stop the player (will need audio restoring).
 * - BUZZER: stop the player (no audio restoring required).
 *
 * Error processing:
 * - An error in stopping the midi player will result in changing to IDLE state.
 * - A bad player type will result in changing to IDLE state too.
 *
 * @noparam
 *
 * @noreturn
 */
void
as_pause_do_resume_req(const T_AS_RESUME_REQ * msg)
{
T_AS_RET status = AS_OK;

    AS_TRACE_1(DEBUG_LOW, "paused_process_resume() player_type = %d", as_ctrl_blk->player_type);
 

     switch (as_ctrl_blk->player_type)
        {
#ifdef AS_OPTION_MIDI

        case AS_PLAYER_TYPE_MIDI:
		  // Resume the midi player.
            	  // This changes the state to RESUME.
                status = as_midi_resume_player();

            	  if (status != AS_OK)
            	  {
     AS_TRACE_1(DEBUG_LOW, "playing_process_paused()status = %d", status);
 
               	 // Error, best effort: switch to IDLE state.
                	as_process_to_idle_state(status, FALSE);
           	  }
             break;
#endif
#if(AS_OPTION_MP3 == 1)
            case AS_PLAYER_TYPE_MP3:
		  // Stops the mp3 player.
            	  // This changes the state to STOPPING.
                status = (T_AS_RET)as_mp3_resume_player();

            	  if (status != AS_OK)
            	  {
               	 // Error, best effort: switch to IDLE state.
                	as_process_to_idle_state(status, FALSE);
           	  }
             break;
#endif
#if(AS_OPTION_AAC == 1)
            case AS_PLAYER_TYPE_AAC:
		  // Stops the aac player.
            	  // This changes the state to STOPPING.
                status = (T_AS_RET)as_aac_resume_player();

            	  if (status != AS_OK)
            	  {
               	 // Error, best effort: switch to IDLE state.
                	as_process_to_idle_state(status, FALSE);
           	  }
             break;
#endif

        default:
            AS_TRACE_L(ERROR,
                       "playing_process_stop() PLAYING state inconsistency: player_type=",
                       as_ctrl_blk->player_type);
            // Error, best effort: switch to IDLE state.
            as_process_to_idle_state(AS_INTERNAL_ERR, FALSE);
            break;
        }
    
}


/**
 * Performs the pause to stop processing.
 *
 * Silent mode processing:
 * - Simply change back to the IDLE state.
 *
 * Audible mode processing:
 * - MIDI: stop the player (will need audio restoring).
 * - BUZZER: stop the player (no audio restoring required).
 *
 * Error processing:
 * - An error in stopping the midi player will result in changing to IDLE state.
 * - A bad player type will result in changing to IDLE state too.
 *
 * @noparam
 *
 * @noreturn
 */
void
as_pause_do_stop_req(const T_AS_STOP_REQ* msg)
{
	as_ctrl_blk->client_rp = msg->rp;
   
    // Call stop processing.
    as_playing_process_stop();
}

void
as_playing_do_forrew_req(const T_AS_FORREW_REQ* msg)
{
 T_AS_RET status = AS_OK;

  

    AS_TRACE_1(DEBUG_LOW, "playing_do_forrew() player_type = %d", as_ctrl_blk->player_type);
 

     switch (as_ctrl_blk->player_type)
        {

#ifdef AS_OPTION_MIDI
         case AS_PLAYER_TYPE_MIDI:
		  as_ctrl_blk->midi.params.media_skip_time = msg->media_skip_time;
		  as_ctrl_blk->forrew = msg->forrew;

		  // Forward or Rewind the midi player.
            	  // This changes the state to PAUSE.
                status = as_midi_forrew_player();


            	  if (status != AS_OK)
            	  {
     AS_TRACE_1(DEBUG_LOW, "playing_process_forrew()status = %d", status);
 
               	 // Error, best effort: switch to IDLE state.
                	as_process_to_idle_state(status, FALSE);
           	  }
             break;
#endif

#if(AS_OPTION_MP3 == 1)
            case AS_PLAYER_TYPE_MP3:
		 as_ctrl_blk->media_skip_time = msg->media_skip_time;
		  as_ctrl_blk->forrew = msg->forrew;
		  // Forward or Rewind the mp3 player.
            	  // This changes the state to STOPPING.
                status = (T_AS_RET)as_mp3_forrew_player();

            	  if (status != AS_OK)
            	  {
     AS_TRACE_1(DEBUG_LOW, "playing_process_paused()status = %d", status);
 
               	 // Error, best effort: switch to IDLE state.
                	as_process_to_idle_state(status, FALSE);
           	  }
             break;
#endif

        default:
            AS_TRACE_L(ERROR,
                       "playing_process_stop() PLAYING state inconsistency: player_type=",
                       as_ctrl_blk->player_type);
            // Error, best effort: switch to IDLE state.
            as_process_to_idle_state(AS_INTERNAL_ERR, FALSE);
            break;
        }
}


/**
 * Process the STOP_REQ message from PLAYING state.
 *
 * Simply save the return path of the caller and call the
 * real stop processing function.
 *
 * @param msg @in The stop request from the AS client.
 *
 * @noreturn
 */
void
as_playing_do_stop_req(const T_AS_STOP_REQ* msg)
{
  
    as_ctrl_blk->client_rp = msg->rp;
   
    // Call stop processing.
    as_playing_process_stop();

  }


/**
 * Process the AS_START_REQ message from PLAYING state.
 *
 * Check if the start request has higher priority.
 * If not, send back a denied START_IND.
 *
 * If the start request has higher priority, set the interrupted flag,
 * save the received start request and stop the current player.
 *
 * @param msg @in The start request from the AS client.
 *
 * @noreturn
 */
void
as_playing_do_start_req(const T_AS_START_REQ* msg)
{
    // Check priority.
    if (msg->ringer_mode < as_ctrl_blk->ringer_mode)
    {
        // Not higher priority: START request is denied.
        as_send_start_ind((T_AS_RET)AS_DENIED, FALSE, &msg->rp);

        // Do not change current state.
    }
    else
    {
        AS_DEBUG_0(HIGH, "playing_do_start_req() current player interrupted.");

        // Priority request, current player is interrupted.
        as_ctrl_blk->interrupted = TRUE;

        // Call the regular stop after having saved the request.
        as_request_save(msg);
        as_playing_process_stop();
    }
}

/**
 * Process the AS_START_REQ message from PAUSE state.
 *
 * Check if the start request has higher priority.
 * If not, send back a denied START_IND.
 *
 * If the start request has higher priority, set the interrupted flag,
 * save the received start request and stop the current player.
 *
 * @param msg @in The start request from the AS client.
 *
 * @noreturn
 */
void
as_pause_do_start_req(const T_AS_START_REQ* msg)
{
    // Check priority.
    AS_DEBUG_0(HIGH, "pausing_do_start_req() request came.");
    if (msg->ringer_mode < as_ctrl_blk->ringer_mode)
    {
        // Not higher priority: START request is denied.
        as_send_start_ind((T_AS_RET)AS_DENIED, FALSE, &msg->rp);

        // Do not change current state.
    }
    else
    {
        AS_DEBUG_0(HIGH, "pausing_do_start_req() current player interrupted.");

        // Priority request, current player is interrupted.
        as_ctrl_blk->interrupted = TRUE;

        // Call the regular stop after having saved the request.
        as_request_save(msg);
        as_playing_process_stop();
    }
}




// ----------------------------------------------------------------------------
//   STOPPING STOPPING STOPPING STOPPING STOPPING STOPPING STOPPING STOPPING
// ----------------------------------------------------------------------------


// No STOPPING state until MP3 implementation.


// ----------------------------------------------------------------------------
//   RESTORING RESTORING RESTORING RESTORING RESTORING RESTORING RESTORING
// ----------------------------------------------------------------------------


/**
 * Process the AS_START_REQ message from RESTORING/STOPPING state.
 *
 * Because the player is terminated there is no priority check.
 *
 * The function set the interrupted flag and save the received start request.
 *
 * There is no need to stop the current player, as it is already in stop phase.
 *
 * @param msg @in The start request from the AS client.
 *
 * @noreturn
 */
void
as_internal_stop_do_start_req(const T_AS_START_REQ* msg)
{
    // No priority to check, because the current player terminated.

    AS_DEBUG_0(HIGH, "internal_stop_do_start_req() current player interrupted.");

    // Priority request, current player is interrupted.
    as_ctrl_blk->interrupted = TRUE;

    // Call the regular stop after having saved the request.
    as_request_save(msg);

    // Player is already stopping.
}

/**
 * Process the AS_PAUSE_REQ message from RESTORING/STOPPING state.
 *
 * Because the player is terminated there is no priority check.
 *
 * The function set the interrupted flag and save the received start request.
 *
 * There is no need to stop the current player, as it is already in stop phase.
 *
 * @param msg @in The start request from the AS client.
 *
 * @noreturn
 */
void
as_internal_stop_do_pause_req(const T_AS_START_REQ* msg)
{
    // No priority to check, because the current player terminated.

    AS_DEBUG_0(HIGH, "internal_stop_do_start_req() current player interrupted.");

    // Priority request, current player is interrupted.
    as_ctrl_blk->interrupted = TRUE;

    // Call the regular stop after having saved the request.
    as_request_save(msg);

    // Player is already stopping.
}


/**
 * Process the AUDIO_FULL_ACCESS_WRITE_DONE message from RESTORING state.
 *
 * Regular mode processing:
 * - Change regularly to the IDLE state (end of media not reached).
 *
 * Recover mode processing:
 * - Send either START/STOP_IND message to requester with NOT_ALLOWED status.
 * - Set the state to IDLE
 *   (the recover flag is reset when receiving a START_REQ in IDLE state).
 *
 * Error processing:
 * - An error status from the audio is reported to the AS client.
 *
 * @param msg @in The response message from the audio SWE.
 *                May be null (0) if audio config is skipped.
 *
 * @noreturn
 */
void
as_restoring_do_fa_write_done(const T_AUDIO_FULL_ACCESS_WRITE_DONE* msg)
{
    if (as_ctrl_blk->recover == 0)
    {
        // Regular restoring processing.

        // Check for audio error !
        if ((msg != 0) && (msg->status != AUDIO_OK))
        {
            AS_TRACE_L(ERROR, "restoring_do_fa_write_done() bad status =", msg->status);
            as_ctrl_blk->error = (T_AS_RET)AS_AUDIO_ERR;
        }

        // Switch to IDLE state.
        as_process_to_idle_state(as_ctrl_blk->error, as_ctrl_blk->end_of_media);
    }
    else
    {
        // Recover mode: send START/STOP(NOT_ALLOWED) according to request.
        if (as_ctrl_blk->recover == AS_START_REQ)
        {
            as_send_start_ind((T_AS_RET)AS_NOT_ALLOWED, FALSE, &as_ctrl_blk->client_rp);
        }
        else // AS_STOP_REQ
        {
            as_send_stop_ind((T_AS_RET)AS_NOT_ALLOWED, FALSE, &as_ctrl_blk->client_rp);
        }

        // Transition: change to IDLE state.
        AS_CHANGE_STATE(AS_STATE_IDLE);
    }
}
/*Process BT Connect or disconnect request*/
void as_do_bt_cfg_req(const T_AS_BT_CFG_REQ * msg)
{
     T_AS_RET status = AS_OK;
     as_ctrl_blk->bt_connect_status = msg->connected_status;
     as_ctrl_blk->client_rp     = msg->rp;
  
                status = (T_AS_RET) as_bt_status ();


            	  if (status != AS_OK)
            	  {
     AS_TRACE_1(DEBUG_LOW, "bt_Cfg()status = %d", status);
          	         	  }
}


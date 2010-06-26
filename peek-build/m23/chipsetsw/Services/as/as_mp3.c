/**
 * @file
 *
 * @brief Definition of the MP3 player management functions.
 *
 */

/*
 * History:
 *
 *	Date       	Author					Modification
 *	-------------------------------------------------------------------
 *	10/24/2005	ramanujam@ti.com		Create.
 *
 * (C) Copyright 2005 by Texas Instruments Incorporated, All Rights Reserved
 */

#include "as/as_i.h"

#if (AS_OPTION_MP3 == 1)

#include "as/as_api.h"
#include "as/as_mp3.h"
#include "as/as_processing.h"
#include "as/as_audio.h"
#include "as/as_utils.h"

#include "rvf/rvf_api.h"

#if (AS_RFS_API == 1)
  #include "rfs/rfs_api.h"
#endif

/**
 * Prepare audio configuration.
 *
 * This function performs the following processing to prepare
 * the midi playing:
 * - Call the audio configuration chain (see as_audio_fa_write()).
 *
 *
 * @param msg @in The start request from the AS client.
 *
 * @retval AS_OK         The processing is successful.
 * @retval AS_PLAYER_ERR An error occurred in the BAE player.
 * @retval AS_AUDIO_ERR  An error occurred in the Audio SWE.
 */
T_AS_RET
as_mp3_prepare_setting(const T_AS_START_REQ* msg)
{
    T_AS_RET rc  = (T_AS_RET)AS_OK;

    // This is specifically needed for MP3 as the filenames are passed to lower layers
    // only after Audio settings are done.
#if(AS_RFS_API == 1)
	wstrcpy( as_ctrl_blk->player_data.filename, msg->data.filename);
#else
	strcpy( as_ctrl_blk->player_data.filename, msg->data.filename);
#endif
    as_ctrl_blk->loop = msg->loop;
    as_ctrl_blk->first_play = TRUE;
  //  as_ctrl_blk->play_bar_on = msg->play_bar_on;
#if (AS_OPT_SKIP_AUDIO_CONFIG == 1)
        // call player start whithout setting audio.
        return as_mp3_start_player();
#else
    // Define audio configuration before to change it.
    rc = as_audio_fa_read();

    if (rc == AS_OK)
    {
           // Now start the audio parameters setting callback chain.
           // Transition: change to SETTING state.
           if (as_audio_fa_write(AS_STATE_SETTING) != (T_AS_RET)AS_OK)
            {
                // Error while configuring audio parameters.
                rc = (T_AS_RET)AS_AUDIO_ERR;
            }
    }
    else
    {
          // Error while reading audio parameters.
          rc = (T_AS_RET)AS_AUDIO_ERR;
    }

    return (T_AS_RET)rc;
#endif
}


/**
 * Actually start the Mp3 .
 *
 * This function starts the MP3 playing by:
 * - Sending a message AUDIO_MP3_START_REQ to L1.
 *
 * @noparam
 *
 * @retval AS_OK         The processing is successful.
 * @retval AS_PLAYER_ERR An error occurred in the BAE player.
 */
T_AS_RET
as_mp3_start_player(void)
{
    T_AS_RET rc  = (T_AS_RET)AS_OK;
    T_AUDIO_RET audio_return = AUDIO_OK;
    T_AUDIO_MP3_PARAMETER mp3_param;
    	 

    mp3_param.mono_stereo = as_ctrl_blk->mp3.params.mono_stereo;
#if(AS_RFS_API == 1)
	wstrcpy(mp3_param.mp3_name,  as_ctrl_blk->player_data.filename);
#else
	strcpy(mp3_param.mp3_name,  as_ctrl_blk->player_data.filename);
#endif
    mp3_param.size_file_start = as_ctrl_blk->mp3.params.size_file_start;
    mp3_param.play_bar_on = as_ctrl_blk->play_bar_on;
   
    /* Send a message and change the state to STARTING*/
    audio_return = audio_mp3_start ( &mp3_param, as_ctrl_blk->rp );
	
    if (audio_return != AUDIO_OK)
    {
        AS_TRACE_L(ERROR, "mp3_start_player() audio_mp3_start(): error=", audio_return);
        rc = (T_AS_RET)AS_PLAYER_ERR;
    }
    else
    {
        AS_DEBUG_0(HIGH, "mp3_start_player() started");
        
        if( (TRUE == as_ctrl_blk->loop) && (FALSE == as_ctrl_blk->first_play))
       {
		// MP3 playing in loop is not possible in the lower layer. Hence 
		// it is simulated in AS. 
		// To avoid sending AS_START_IND everytime in the loop. This 
		// check is done.
		AS_CHANGE_STATE(AS_STATE_PLAYING);
        }
       else
       {
        // Everything is OK. Transition to STARTING state.
	       // Change it to PLAYING - As audio_mp3_start doesnt give 
       	// AUDIO_MP3_STATUS_MSG after starting and so no need 
        	// to go to STARTING state
	 	//AS_CHANGE_STATE(AS_STATE_PLAYING);
		as_process_to_playing_state( AS_OK );

		as_ctrl_blk->first_play = FALSE;
        }
    }

    return (T_AS_RET)rc;
}

/**
 * pause the mp3 player.
 *
 * This function pauses the mp3 player by performing the following steps:
 * - restoring the player.
 * 
 * @noparam
 *
 * @retval AS_OK         The processing is successful.
 * @retval AS_AUDIO_ERR  An error occurred in the Audio SWE.
 */
T_AS_RET
as_mp3_pause_player(void)
{
    T_AS_RET rc = (T_AS_RET)AS_OK;
    T_AUDIO_RET audio_return = AUDIO_OK;
   
    /* Send a message and change the state to STARTING*/
    audio_return = audio_mp3_pause( );

    if (audio_return != AUDIO_OK)
    {
       AS_TRACE_L(ERROR, "mp3_pause_player() audio_mp3_pause(): error=", audio_return);
        rc =(T_AS_RET) AS_PLAYER_ERR;
    }
    else
    {
        AS_DEBUG_0(HIGH, "mp3_pause_player() completed");
        
        // Everything is OK. Transition to PAUSE state.
	 AS_CHANGE_STATE(AS_STATE_PAUSED);
	
    }
  return (T_AS_RET)rc;
}



/**
 * resume the mp3 player.
 *
 * This function resumes the mp3 player by performing the following steps:
 * - restoring the player.
 * 
 * @noparam
 *
 * @retval AS_OK         The processing is successful.
 * @retval AS_AUDIO_ERR  An error occurred in the Audio SWE.
 */
T_AS_RET
as_mp3_resume_player(void)
{
    T_AS_RET rc = (T_AS_RET)AS_OK;
    T_AUDIO_RET audio_return = AUDIO_OK;
   
    /* Send a message and change the state to STARTING*/
      audio_return = audio_mp3_resume( );

    if (audio_return != AUDIO_OK)
    {
        AS_TRACE_L(ERROR, "mp3_resume_player() audio_mp3_resume(): error=", audio_return);
        rc = (T_AS_RET)AS_PLAYER_ERR;
    }
    else
    {
        AS_DEBUG_0(HIGH, "mp3_resume_player() completed");
        
        // Everything is OK. Transition to PLAYING state.
	 AS_CHANGE_STATE(AS_STATE_PLAYING);
	
    }
	  return (T_AS_RET)rc;
}


/**
 * forward or rewind the mp3 player.
 *
 * This function forwar/rewind the mp3 player by performing the following steps:
 * - restoring the player.
 * 
 * @noparam
 *
 * @retval AS_OK         The processing is successful.
 * @retval AS_AUDIO_ERR  An error occurred in the Audio SWE.
 */
T_AS_RET
as_mp3_forrew_player(void)
{
    T_AS_RET rc = (T_AS_RET)AS_OK;
    T_AUDIO_RET audio_return = AUDIO_OK;
   
    /* Send a message and change the state to STARTING*/
   if(as_ctrl_blk->forrew == FORWARD)
   	{
   		AS_DEBUG_0(HIGH, "call mp3 forward");
    audio_return = audio_mp3_forward(as_ctrl_blk->media_skip_time);
   	}
   
   else if (as_ctrl_blk->forrew == REWIND)
   	{

	AS_DEBUG_0(HIGH, "call mp3 rewind");
        audio_return = audio_mp3_rewind(as_ctrl_blk->media_skip_time);
   	}

    if (audio_return != AUDIO_OK)
    {
       AS_TRACE_L(ERROR, "mp3_forrew_player() audio_mp3_forward() audio_mp3_rewind: error=", audio_return);
        rc =(T_AS_RET) AS_PLAYER_ERR;
    }
    else
    {
            AS_DEBUG_1(HIGH, "mp3_forrew_player() completed %d",as_ctrl_blk->media_skip_time);
             as_ctrl_blk->media_skip_time = 0; 
    }
  return (T_AS_RET)rc;
}

/**
 * Stop the mp3 player.
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
as_mp3_stop_player(void)
{
    T_AS_RET rc = (T_AS_RET)AS_OK;
    T_AUDIO_RET audio_return = AUDIO_OK;
    UINT32 size_played;

    /* Send a message and change the state to STARTING*/
    audio_return = audio_mp3_stop ( &size_played );

   as_ctrl_blk->player_data.memory.size = size_played;//Daisy tang added for Real Resume feature 20071107
    if (audio_return != AUDIO_OK)
    {
        AS_TRACE_L(ERROR, "mp3_stop_player() audio_mp3_stop(): error=", audio_return);
        rc = (T_AS_RET)AS_PLAYER_ERR;
    }
    else
    {
        AS_DEBUG_0(HIGH, "mp3_stop_player() completed");
        
        // Everything is OK. Transition to STARTING state.
	 AS_CHANGE_STATE(AS_STATE_STOPPING);
    }

    return (T_AS_RET)rc;
}

/**
 * Handle the STATUS message for MP3 start.
 *
 * This function handles the STATUS message for MP3 start:
 * - On Success change AS state to PLAYING.
 *
 * @noparam
 *
 * @retval AS_OK         The processing is successful.
 * @retval AS_PLAYER_ERR  An error occurred in the Audio SWE.
 */
T_AS_RET
as_starting_do_mp3_status(T_AUDIO_MP3_STATUS* msg)
{
    T_AS_RET rc = AS_OK;
	
    if( AUDIO_OK == msg->status )
    {
 
 //OMAPS00175540
	 if(as_ctrl_blk->bt_connect_status == TRUE)
		 {
    as_send_start_ind(AS_OK,
                      as_ctrl_blk->interrupted,
                      &as_ctrl_blk->client_rp);

    // Reset the interrupted flag.
    as_ctrl_blk->interrupted = FALSE;
	 	}
        /// change state to PLAYING
	AS_CHANGE_STATE(AS_STATE_PLAYING);
    }
    else
    {
	AS_TRACE_L(ERROR, "mp3 start error", msg->status);
      	rc = (T_AS_RET)AS_PLAYER_ERR;
    }

    return rc;
}


/**
 * Handle the STATUS message for MP3 stop request.
 *
 * This function handles the STATUS message for MP3 start:
 * - On Success starts making the audio configurations.
 *
 * @noparam
 *
 * @retval AS_OK         The processing is successful.
 * @retval AS_PLAYER_ERR  An error occurred in the Audio SWE.
 */
T_AS_RET
as_stopping_do_mp3_status( T_AUDIO_MP3_STATUS* msg)
{
   T_AS_RET rc = AS_OK;
	
   if( AUDIO_OK == msg->status )
   {
      // Now start the audio parameters setting callback chain.
      // Transition: change to RESTORING state.
     #if (AS_OPT_SKIP_AUDIO_CONFIG == 1)
     as_restoring_do_fa_write_done(0);
     #else
      rc = as_audio_fa_write(AS_STATE_RESTORING);
      #endif
   }
   else
   {
	AS_TRACE_L(ERROR, "mp3 start error", msg->status);
      	rc = (T_AS_RET)AS_PLAYER_ERR;
   }

return rc;
}

/**
 * Handle the STATUS message media complete message.
 *
 * This function handles the STATUS message because of MP3 playback completion:
 * - On Success and if loop is requested, starts MP3 playback again
 * - On Success if loop is not requested starts making the audio configurations.
 *
 * @noparam
 *
 * @retval AS_OK         The processing is successful.
 * @retval AS_PLAYER_ERR  An error occurred in the Audio SWE.
 */
T_AS_RET
as_playing_do_mp3_complete(T_AUDIO_MP3_STATUS* msg)
{
  T_AS_RET rc = AS_OK;

   if( AUDIO_OK == msg->status )
   {
      if( TRUE == as_ctrl_blk->loop )
      {
         as_mp3_start_player();
		 // Check for error and return appropriately
      }
      else
      {
          // Now start the audio parameters setting callback chain.
          // Transition: change to RESTORING state.
         #if (AS_OPT_SKIP_AUDIO_CONFIG == 1)
          as_restoring_do_fa_write_done(0);
         #else
          rc = as_audio_fa_write(AS_STATE_RESTORING);
         #endif
      }
   }
   else if( AUDIO_MP3_NOT_SUPP == msg->status )
   	{
	    //if L1 & L2 MP3 files pop up error Message
		AS_TRACE_L(ERROR, "Layer1 Layer2 mp3 stop error", msg->status );
   	 	as_send_stop_ind((T_AS_RET)AS_NOT_SUPPORTED, FALSE, &as_ctrl_blk->client_rp);
		// Error on error/recover, best effort: switch to IDLE state.
        as_process_to_idle_state((T_AS_RET)msg->status, FALSE);
      	rc = (T_AS_RET)AS_PLAYER_ERR;
    }
   else
   {
       AS_TRACE_L(ERROR, "mp3 stop error", msg->status );

	// Error on error/recover, best effort: switch to IDLE state.
       as_process_to_idle_state((T_AS_RET)msg->status, FALSE);
      	rc = (T_AS_RET)AS_PLAYER_ERR;
   }

   return (T_AS_RET)rc;
}

/**
 * Handle the STATUS BAR message.
 *
 * This function handles the STATUS bar message
 *
 * @noparam
 *
 * @retval AS_OK         The processing is successful.
 * @retval AS_PLAYER_ERR  An error occurred in the Audio SWE.
 */


void
as_send_probar_mp3_ind(T_AUDIO_MP3_STATUS_BAR* pro_bar, BOOLEAN media_type,const T_RV_RETURN_PATH* rp)
{
   T_AS_PROBAR_IND_MP3*  msg;
    T_RVF_MB_STATUS mb_status;

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
	 msg->u32totalTimeEst = (pro_bar->u32totalTimeEst)/1000;
	 msg->u32TotalTimePlayed =( pro_bar->u32TotalTimePlayed)/1000;
	 msg->media_type = media_type;

        msg->status = (T_AS_RET)pro_bar->status;

	 AS_DEBUG_2(LOW, "u32totalTimeEst->%d u32TotalTimePlayed->%d", msg ->u32totalTimeEst,msg->u32TotalTimePlayed);
        
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

#endif

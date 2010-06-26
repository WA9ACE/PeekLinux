/**
 * @file
 *
 * @brief Definition of the E1-Melody player management functions.
 *
 */

/*
 * History:
 *
 *	Date       	Author					Modification
 *	-------------------------------------------------------------------
 *	12/12/2006							Create.
 *
 * (C) Copyright 2005 by Texas Instruments Incorporated, All Rights Reserved
 */

#include "as/as_i.h"

#if (AS_OPTION_E1_MELODY == 1)

#include "as/as_api.h"
#include "as/as_E1_Melody.h"
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
as_e1_melody_prepare_setting(const T_AS_START_REQ* msg)
{

T_AS_RET rc  = AS_OK;

    // This is specifically needed for E1-Melody as the filenames are passed to lower layers
    // only after Audio settings are done.
    #if(AS_RFS_API == 1)
	wstrcpy( as_ctrl_blk->player_data.filename, msg->data.filename);
#else
	strcpy( as_ctrl_blk->player_data.filename, msg->data.filename);
#endif

//  loop not required as lower support is not for E1-Melody
 if (AS_OPTION_E1_MELODY == 1)
{
   
    as_ctrl_blk->e1_melody.params.loopback = msg->loop;
   

} 
	
#if (AS_OPT_SKIP_AUDIO_CONFIG == 1)
        // call player start whithout setting audio.
        return as_e1_melody_start_player();
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
                rc = (T_AS_RET)AS_AUDIO_ERR;
            }
    }
    else
    {
          // Error while reading audio parameters.
          rc = (T_AS_RET)AS_AUDIO_ERR;
    }

    return rc;
#endif
    
}


/**
 * Actually start the E1-Melody .
 *
 * This function starts the E1-Melody  playing by:
 * - Sending a message AUDIO_E1_MELODY_START_REQ to L1.
 *
 * @noparam
 *
 * @retval AS_OK         The processing is successful.
 * @retval AS_PLAYER_ERR An error occurred in the BAE player.
 */
T_AS_RET
as_e1_melody_start_player(void)
{
    T_AS_RET rc  = AS_OK;
    T_AUDIO_RET audio_return = AUDIO_OK;
    T_AUDIO_MELODY_E1_PARAMETER e1_melody_param;

       
    e1_melody_param.loopback= as_ctrl_blk->e1_melody.params.loopback;
    e1_melody_param.melody_mode = as_ctrl_blk->e1_melody.params.melody_mode;
   #if(AS_RFS_API == 1)
	wstrcpy(e1_melody_param.melody_name,  as_ctrl_blk->player_data.filename);
#else
	strcpy(e1_melody_param.melody_name,  as_ctrl_blk->player_data.filename);
#endif
    

     /* Send a message and change the state to STARTING*/
    audio_return = audio_melody_E1_start ( &e1_melody_param, as_ctrl_blk->rp );
	
    if (audio_return != AUDIO_OK)
    {
        AS_TRACE_L(ERROR, "e1_melody_start_player() audio_e1_melody_start(): error=", audio_return);
        rc = (T_AS_RET)AS_PLAYER_ERR;
    }
    else
    {
        AS_DEBUG_0(HIGH, "e1_melody_start_player() started");

	as_process_to_playing_state( AS_OK );
        
		//as_ctrl_blk->first_play = FALSE;
        
    }

    return rc;
}


/**
 * Stop the e1-melody player.
 *
 * This function stop the e1-melody player by performing the following steps:
 * - Destroy the player.
 * - Restore audio configuration.
 *
 * @noparam
 *
 * @retval AS_OK         The processing is successful.
 * @retval AS_AUDIO_ERR  An error occurred in the Audio SWE.
 */
T_AS_RET
as_e1_melody_stop_player(void)
{
    T_AS_RET rc = AS_OK;
    T_AUDIO_RET audio_return = AUDIO_OK;
    UINT32 size_played;
    T_AUDIO_MELODY_E1_STOP_PARAMETER p_parameter;
    T_RV_RETURN return_path ;
    return_path.callback_func = NULL;
    return_path.addr_id = rvf_get_taskid();

   //strcpy(melody_E1_stop_parameter.melody_name,"/melo/LASER");
   #if(AS_RFS_API == 1)
	wstrcpy(p_parameter.melody_name,  as_ctrl_blk->player_data.filename);
   #else
    strcpy(p_parameter.melody_name, as_ctrl_blk->player_data.filename);
    #endif
     
    /* Send a message and change the state to STARTING*/
    audio_return = audio_melody_E1_stop ( &p_parameter,return_path );

    if (audio_return != AUDIO_OK)
    {
        AS_TRACE_L(ERROR, "e1_melody_stop_player() audio_e1_melody_stop(): error=", audio_return);
        rc = (T_AS_RET)AS_PLAYER_ERR;
    }
    else
    {
        AS_TRACE_0(ERROR, "e1_melody_stop_player() completed");
        
        // Everything is OK. Transition to STARTING state.
	 AS_CHANGE_STATE(AS_STATE_STOPPING);
    }

    return rc;
}

/**
 * Handle the STATUS message for E1-MELODY start.
 *
 * This function handles the STATUS message for E1-MELODY start:
 * - On Success change AS state to PLAYING.
 *
 * @noparam
 *
 * @retval AS_OK         The processing is successful.
 * @retval AS_PLAYER_ERR  An error occurred in the Audio SWE.
 */
T_AS_RET
as_starting_do_e1_melody_status(T_AUDIO_MELODY_E1_STATUS* msg)
{
    T_AS_RET rc = AS_OK;
	
    if( AUDIO_OK == msg->status )
    {
        /// change state to PLAYING
	AS_CHANGE_STATE(AS_STATE_PLAYING);
    }
    else
    {
	AS_TRACE_L(ERROR, "E1-Melody start error", msg->status);
      	rc = (T_AS_RET)AS_PLAYER_ERR;
    }

    return rc;
}

/**
 * Handle the STATUS message for E1-MELODY stop request.
 *
 * This function handles the STATUS message for E1-MELODY start:
 * - On Success starts making the audio configurations.
 *
 * @noparam
 *
 * @retval AS_OK         The processing is successful.
 * @retval AS_PLAYER_ERR  An error occurred in the Audio SWE.
 */
T_AS_RET
as_stopping_do_e1_melody_status( T_AUDIO_MELODY_E1_STATUS* msg)
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
	AS_TRACE_L(ERROR, "E1-MELODY start error", msg->status);
      	rc = (T_AS_RET)AS_PLAYER_ERR;
   }

return rc;
}

/**
 * Handle the STATUS message media complete message.
 *
 * This function handles the STATUS message because of E1-Melody playback completion:
 * - On Success and if loop is requested, starts E1-Melody playback again
 * - On Success if loop is not requested starts making the audio configurations.
 *
 * @noparam
 *
 * @retval AS_OK         The processing is successful.
 * @retval AS_PLAYER_ERR  An error occurred in the Audio SWE.
 */
T_AS_RET
as_playing_do_e1_melody_complete(T_AUDIO_MELODY_E1_STATUS* msg)
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
       AS_TRACE_L(ERROR, "E1-MELODY stop error", msg->status );

	// Error on error/recover, best effort: switch to IDLE state.
       as_process_to_idle_state((T_AS_RET) msg->status, FALSE);
      	rc = (T_AS_RET)AS_PLAYER_ERR;
   }

   return rc;
}

#endif



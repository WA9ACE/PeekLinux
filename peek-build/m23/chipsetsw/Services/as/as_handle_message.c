/**
 * @file
 *
 * @brief Implementation of the message handling function.
 *
 * This function allows the AS SWE to handle received messages sent
 * by client SWEs.
 * It is called when the AS SWE receives a new message in its mailbox.
 */

/*
 * History:
 *
 *	Date       	Author			Modification
 *	-------------------------------------------------------------------
 *	1/25/2005	f-maria@ti.com		Create.
 *
 * (C) Copyright 2005 by Texas Instruments Incorporated, All Rights Reserved
 */


#include "as/as_env.h"
#include "as/as_i.h"
#include "as/as_message.h"
#include "as/as_processing.h"
#include "as/as_utils.h"

#include "Audio/audio_api.h"
//#include "Audio/audio_mp3_i.h"
//#include "Audio/audio_aac_i.h"

#include "rv/rv_general.h"


/**
 * @brief Process messages received while in IDLE state.
 *
 * This private function is intended to dispatch the received message
 * when the AS SWE is in the AS_STATE_IDLE state.
 *
 * The following messages are supported in this state:
 * - AS_START_REQ: a client SWE requests to start either the player or ringer.
 * - AS_STOP_REQ: a client SWE requests to stop either the player or ringer.
 *
 * @param msg @in A pointer to the header of the message.
 *
 * @retval RVM_OK  The operation was successful.
 * @retval RVM_INTERNAL_ERR  The received message was unexpected.
 */
T_RVM_RETURN
as_state_idle(const T_RV_HDR* msg)
{
    T_RVM_RETURN rc = RVM_OK;

    // Process the incoming message.
    switch (msg->msg_id)
    {
    case AS_START_REQ:
        // Start player/ringer request.
        as_idle_do_start_req((const T_AS_START_REQ*) msg);
        break;

   
	 
    case AS_STOP_REQ:
        AS_TRACE_0(WARNING,
                   "state_idle() received STOP_REQ message in IDLE state");
        // Warning: send back a message to the client.
        as_send_stop_ind((T_AS_RET)AS_ALREADY, FALSE,
                         &((const T_AS_STOP_REQ*) msg)->rp);
        break;
    
    case AS_PAUSE_REQ:
    case AS_RESUME_REQ:
    case AS_FORREW_REQ:
	 rc = RVM_INTERNAL_ERR;	
	 break;
    case AS_BT_CFG_REQ:
        // Start BT request.
        as_do_bt_cfg_req ((const T_AS_BT_CFG_REQ *) msg);
	 break;

    default:
        AS_TRACE_2(ERROR,
                   "state_idle() unexpected message %d received in state %d: discarded",
                   msg->msg_id, as_ctrl_blk->state);
        rc = RVM_INTERNAL_ERR;
        break;
    }

    return rc;
}


/**
 * @brief Process messages received while in SETTING state.
 *
 * This private function is intended to dispatch the received message
 * when the AS SWE is in the AS_STATE_SETTING state.
 *
 * The following messages are supported in this state:
 * - AUDIO_FULL_ACCESS_WRITE_DONE: the AUDIO replied to an audio_full_access_write() call.
 * - AS_STOP_REQ: a client SWE requests to stop either the player or ringer.
 * - AS_START_REQ: a client SWE requests to start either the player or ringer.
 *
 * @param msg @in A pointer to the header of the message.
 *
 * @retval RVM_OK  The operation was successful.
 * @retval RVM_INTERNAL_ERR  The received message was unexpected.
 */
T_RVM_RETURN
as_state_setting(const T_RV_HDR* msg)
{
    T_RVM_RETURN rc = RVM_OK;

    // Process the incoming message.
    switch (msg->msg_id)
    {
    case AUDIO_FULL_ACCESS_WRITE_DONE:
        // Audio replied to an audio_full_access_write() call.
        as_setting_do_fa_write_done((const T_AUDIO_FULL_ACCESS_WRITE_DONE*) msg);
        break;

    case AS_STOP_REQ:
        // Stop player/ringer request while operating => recover mode.
        AS_TRACE_2(ERROR, "state_setting()AS_STOP_REQ ", msg->msg_id, as_ctrl_blk->state);
        as_enter_recover_mode(msg->msg_id, &((const T_AS_STOP_REQ*)msg)->rp);

        // Do not change AS state.
        break;

    case AS_START_REQ:
        // Start player/ringer request while operating => recover mode.
        AS_TRACE_2(ERROR, "state_setting()AS_START_REQ ", msg->msg_id, as_ctrl_blk->state);
        as_enter_recover_mode(msg->msg_id, &((const T_AS_START_REQ*)msg)->rp);

        // Do not change AS state.
        break;

    case AS_PAUSE_REQ:
	// Pause player/ringer request while operating => recover mode.
 //       as_enter_recover_mode(msg->msg_id, &((const T_AS_PAUSE_REQ*)msg)->rp);
	AS_TRACE_2(ERROR, "state_setting() pause received discarded", msg->msg_id, as_ctrl_blk->state);
        // Do not change AS state.
        break;

    case AS_RESUME_REQ:
	// Pause player/ringer request while operating => recover mode.
	AS_TRACE_2(ERROR, "state_setting() resume received discarded", msg->msg_id, as_ctrl_blk->state);

        // Do not change AS state.
        break;
     case AS_BT_CFG_REQ:
        // BT CFG request.
        as_do_bt_cfg_req ((const T_AS_BT_CFG_REQ *) msg);
        break;

   case AS_FORREW_REQ:
        //	 as_enter_recover_mode(msg->msg_id, &((const T_AS_START_REQ*)msg)->rp);
        AS_TRACE_2(ERROR, "state_setting() forward rewind received discarded", msg->msg_id, as_ctrl_blk->state);
        break;
    default:
        AS_TRACE_2(ERROR,
                   "state_setting() unexpected message %d received in state %d: discarded",
                   msg->msg_id, as_ctrl_blk->state);
        rc = RVM_INTERNAL_ERR;
        break;
    }

    return rc;
}


/**
 * @brief Process messages received while in STARTING state.
 *
 * This private function is intended to dispatch the received message
 * when the AS SWE is in the AS_STATE_STARTING state.
 *
 * The following messages are supported in this state:
 * - AUDIO_MP3_STATUS: (NOT YET IMPLEMENTED)the AUDIO replied to an audio_mp3_start() call.
 * - AS_STOP_REQ: a client SWE requests to stop either the player or ringer.
 * - AS_START_REQ: a client SWE requests to start either the player or ringer.
 *
 * @param msg @in A pointer to the header of the message.
 *
 * @retval RVM_OK  The operation was successful.
 * @retval RVM_INTERNAL_ERR  The received message was unexpected.
 */
T_RVM_RETURN
as_state_starting(const T_RV_HDR* msg)
{
    T_RVM_RETURN rc = RVM_OK;

    // Process the incoming message.
    switch (msg->msg_id)
    {
#if(AS_OPTION_MP3 == 1)
    case AUDIO_MP3_STATUS_MSG:
        // Audio replied to an audio_mp3_start() call.
       as_starting_do_mp3_status((const T_AUDIO_MP3_STATUS*) msg);
       break;
#endif
#if(AS_OPTION_AAC == 1)
    case AUDIO_AAC_STATUS_MSG:
        // Audio replied to an audio_aac_start() call.
       as_starting_do_aac_status((const T_AUDIO_AAC_STATUS*) msg);
       break;
#endif

#if(AS_OPTION_E1_MELODY == 1)
    case AUDIO_MELODY_E1_STATUS_MSG:
        // Audio replied to an audio_e1_melody_start() call.
       as_starting_do_e1_melody_status((const T_AUDIO_MELODY_E1_STATUS*) msg);
       break;
#endif

    case AS_STOP_REQ:
        // Stop player/ringer request while operating => recover mode.
        AS_TRACE_2(ERROR, "as_state_starting()AS_STOP_REQ ", msg->msg_id, as_ctrl_blk->state);
        as_enter_recover_mode(msg->msg_id, &((const T_AS_STOP_REQ*)msg)->rp);

        // Do not change AS state.
        break;

    case AS_START_REQ:
        // Start player/ringer request while operating => recover mode.
        AS_TRACE_2(ERROR, "as_state_starting()AS_START_REQ ", msg->msg_id, as_ctrl_blk->state);
        as_enter_recover_mode(msg->msg_id, &((const T_AS_START_REQ*)msg)->rp);

        // Do not change AS state.
        break;


   case AS_PAUSE_REQ:
	AS_TRACE_2(ERROR, "state_starting() pause received discarded", msg->msg_id, as_ctrl_blk->state);
	 break;


  case AS_RESUME_REQ:
	AS_TRACE_2(ERROR, "state_starting() resume received discarded", msg->msg_id, as_ctrl_blk->state);
	 break;
	 
  case AS_FORREW_REQ:
  	   AS_TRACE_2(ERROR, "state_starting() forward rewind received discarded", msg->msg_id, as_ctrl_blk->state);
	 break;
  case AS_BT_CFG_REQ:
        // BT CFG request.
        as_do_bt_cfg_req ((const T_AS_BT_CFG_REQ *) msg);
        break;
   
    default:
        AS_TRACE_2(ERROR,
                   "state_starting() unexpected message %d received in state %d: discarded",
                   msg->msg_id, as_ctrl_blk->state);
        rc = RVM_INTERNAL_ERR;
        break;
    }

    return rc;
}


/**
 * @brief Process messages received while in PLAYING state.
 *
 * This private function is intended to dispatch the received message
 * when the AS SWE is in the AS_STATE_PLAYING state.
 *
 * The following messages are supported in this state:
 * - AS_STOP_REQ: a client SWE requests to stop either the player or ringer.
 * - AS_START_REQ: a client SWE requests to start either the player or ringer.
 *
 * @param msg @in A pointer to the header of the message.
 *
 * @retval RVM_OK  The operation was successful.
 * @retval RVM_INTERNAL_ERR  The received message was unexpected.
 */
T_RVM_RETURN
as_state_playing(const T_RV_HDR* msg)
{
    T_RVM_RETURN rc = RVM_OK;

    AS_TRACE_1(DEBUG_LOW, "msg->msg_id = %d",msg->msg_id);
    // Process the incoming message.
    switch (msg->msg_id)
    {
    case AS_STOP_REQ:
        // Stop player/ringer request.
      //  AS_TRACE_1(DEBUG_LOW, "playing_process_stop() as_state_playing = %d", as_ctrl_blk->player_type);
        as_playing_do_stop_req((const T_AS_STOP_REQ*) msg);
        break;

   #if(AS_OPTION_MP3 == 1)
    case AUDIO_MP3_STATUS_MSG:
	// MP3 play automatic media completion message.
	as_playing_do_mp3_complete((T_AUDIO_MP3_STATUS*) msg);
	break;
#endif
#if(AS_OPTION_AAC == 1)
   case AUDIO_AAC_STATUS_MSG:
	// AAC play automatic media completion message.
	as_playing_do_aac_complete((T_AUDIO_AAC_STATUS*) msg);
	break;
#endif

#if(AS_OPTION_MP3 == 1)
    case AUDIO_MP3_STATUS_BAR:
	// MP3 play status bar update message.
	//AS_TRACE_2(ERROR, "AUDIO_MP3_STATUS_BAR  message %x received in state %d:",msg->msg_id, as_ctrl_blk->state);
	// as_send_start_ind(status, as_ctrl_blk->interrupted, &as_ctrl_blk->client_rp);
       as_send_probar_mp3_ind((T_AUDIO_MP3_STATUS_BAR*) msg, AS_PLAYER_TYPE_MP3, &as_ctrl_blk->client_rp);
	break;
#endif
#if(AS_OPTION_AAC == 1)
     case AUDIO_AAC_STATUS_BAR:
     // AAC play status bar update message.
  //   AS_TRACE_2(ERROR, "AUDIO_MP3_STATUS_BAR  message %x received in state %x: discarded",  msg->msg_id, as_ctrl_blk->state);
	as_send_probar_aac_ind((T_AUDIO_AAC_STATUS_BAR*) msg, AS_PLAYER_TYPE_AAC, &as_ctrl_blk->client_rp);
	break;
#endif

#if(AS_OPTION_E1_MELODY == 1)   
	case AUDIO_MELODY_E1_STATUS_MSG:	// E1-MELODY play automatic media completion message.
	as_playing_do_e1_melody_complete((T_AUDIO_MELODY_E1_STATUS*) msg);	
	break;
#endif  

	case AS_INTERNAL_STOP_REQ:
        // Internal stop player/ringer request.

        // Set the tracking flag.
        as_ctrl_blk->internal_stop = TRUE;
        // Act as a regular stop, but do not set the return path.
         AS_TRACE_1(DEBUG_LOW, "internal_process_stop() as_state_playing = %d", as_ctrl_blk->player_type);
        as_playing_process_stop();
        break;

    case AS_START_REQ:
        // Start player/ringer request.
        as_playing_do_start_req((const T_AS_START_REQ*) msg);
        break;
		
   case AS_PAUSE_REQ:
        // Pause player request.
        as_playing_to_pause_state((const T_AS_PAUSE_REQ*) msg);
        break;

   case AS_RESUME_REQ:
	 rc = RVM_INTERNAL_ERR;	
	 break;
 
  case AS_FORREW_REQ:
        // forward or rewind the play position by given time
        as_playing_do_forrew_req((const T_AS_FORREW_REQ*) msg);
        break;
  case AS_BT_CFG_REQ:
        // BT CFG request.
        as_do_bt_cfg_req ((const T_AS_BT_CFG_REQ *) msg);
        break;
    default:
        AS_TRACE_2(ERROR,
                   "state_playing() unexpected message %d received in state %d: discarded",
                   msg->msg_id, as_ctrl_blk->state);
        rc = RVM_INTERNAL_ERR;
        break;
    }

    return rc;
}
/**
 * @brief Process messages received while in PAUSED state.
 *
 * This private function is intended to dispatch the received message
 * when the AS SWE is in the AS_STATE_PAUSED state.
 *
 * The following messages are supported in this state:
 * - AUDIO_MP3_RESUME_REQ: a client SWE requests to pause  the player.
 * - AS_STOP_REQ: a client SWE requests to stop the player.
 *
 * @param msg @in A pointer to the header of the message.
 *
 * @retval RVM_OK  The operation was successful.
 * @retval RVM_INTERNAL_ERR  The received message was unexpected.
 */
T_RVM_RETURN
as_state_paused(const T_RV_HDR* msg)
{
    T_RVM_RETURN rc = RVM_OK;

   AS_TRACE_1(DEBUG_LOW, "paused state msg->msg_id = %d",msg->msg_id);
    // Process the incoming message.
    switch (msg->msg_id)
    {
    
    case AS_START_REQ:
	// Start Player request.
//	AS_TRACE_1(DEBUG_LOW, "start_req() as_state_paused = %d", as_ctrl_blk->player_type);
	as_pause_do_start_req((const T_AS_START_REQ*) msg);
       break;

     case AS_STOP_REQ:
        // Stop player/ringer request.
         //AS_TRACE_1(DEBUG_LOW, "paused_process_stop() as_state_paused = %d", as_ctrl_blk->player_type);
        as_pause_do_stop_req((const T_AS_STOP_REQ*) msg);
        break;

   case AS_RESUME_REQ:
        // Pause player request.
     // AS_TRACE_1(DEBUG_LOW, "as_resume() as_state_paused = %d", as_ctrl_blk->player_type);
        as_pause_do_resume_req((const T_AS_RESUME_REQ*) msg);
        break;

   case AS_PAUSE_REQ:
	 rc = RVM_INTERNAL_ERR;	
	 break;
   case AS_FORREW_REQ:
        // forward or rewind the play position by given time
        as_playing_do_forrew_req((const T_AS_FORREW_REQ*) msg);
        break;
#if(AS_OPTION_AAC == 1)
   case AUDIO_AAC_STATUS_MSG:
	// AAC play automatic media completion message.
	as_playing_do_aac_complete((T_AUDIO_AAC_STATUS*) msg);
	break;
#endif
    case AS_BT_CFG_REQ:
        // BT CFG request.
        as_do_bt_cfg_req ((const T_AS_BT_CFG_REQ *) msg);
        break;	 
    default:
        AS_TRACE_2(ERROR,
                   "state_idle() unexpected message %d received in state %d: discarded",
                   msg->msg_id, as_ctrl_blk->state);
        rc = RVM_INTERNAL_ERR;
        break;
    }

    return rc;
}


/**
 * @brief Process messages received while in STARTING state.
 *
 * This private function is intended to dispatch the received message
 * when the AS SWE is in the AS_STATE_STARTING state.
 *
 * The following messages are supported in this state:
 * - AUDIO_MP3_STATUS: (NOT YET IMPLEMENTED)the AUDIO replied to an audio_mp3_start() call.
 * - AS_STOP_REQ: a client SWE requests to stop either the player or ringer.
 * - AS_START_REQ: a client SWE requests to start either the player or ringer.
 *
 * @param msg @in A pointer to the header of the message.
 *
 * @retval RVM_OK  The operation was successful.
 * @retval RVM_INTERNAL_ERR  The received message was unexpected.
 */
T_RVM_RETURN
as_state_stopping(const T_RV_HDR* msg)
{
    T_RVM_RETURN rc = RVM_OK;

    // Process the incoming message.
    switch (msg->msg_id)
    {
#if(AS_OPTION_MP3 == 1)
    case AUDIO_MP3_STATUS_MSG:
        // Audio replied to an audio_mp3_stop() call.
        as_stopping_do_mp3_status((const T_AUDIO_MP3_STATUS*) msg);
        break;
#endif
#if(AS_OPTION_AAC == 1)
    case AUDIO_AAC_STATUS_MSG:
        // Audio replied to an audio_mp3_stop() call.
        as_stopping_do_aac_status((const T_AUDIO_AAC_STATUS*) msg);
        break;
#endif

#if(AS_OPTION_E1_MELODY == 1)
    case AUDIO_MELODY_E1_STATUS_MSG:
        // Audio replied to an audio_e1_melody_stop() call.
        as_stopping_do_e1_melody_status((const T_AUDIO_MELODY_E1_STATUS*) msg);
        break;
#endif

#ifdef AS_OPTION_MIDI
    case AS_INTERNAL_BAE_IND :
	AS_DEBUG_0(HIGH," Received the AS_INTERNAL_BAE_IND ");
	as_stopping_do_midi_bae_ind();
        break;
#endif

    case AS_STOP_REQ:
        if (as_ctrl_blk->internal_stop)
        {
            // Clear the internal stop flag, as a requested stop was just received.
            as_ctrl_blk->internal_stop = FALSE;
            // Save the return path where the reply the indication.
            as_ctrl_blk->client_rp = ((const T_AS_STOP_REQ*)msg)->rp;
        }
        else
        {
            // Stop player/ringer request while operating => recover mode.
            AS_TRACE_2(ERROR, "Thukaram AS AS state_stopping()AS_STOP_REQ ", msg->msg_id, as_ctrl_blk->state); 
 		    AS_CHANGE_STATE(AS_STATE_PLAYING);

		}

        // Do not change AS state.
        break;

    case AS_INTERNAL_STOP_REQ:
        // Internal stop player/ringer request.

        // This is a regular behavior, just discard the message.
        AS_DEBUG_0(LOW,
                   "state_stopping() discarded internal stop as AS is already stopping.");
        break;

    case AS_START_REQ:
        if (as_ctrl_blk->internal_stop)
        {
            // Clear the internal stop flag, as a requested stop was just received.
            as_ctrl_blk->internal_stop = FALSE;

            // Start player/ringer request.
            as_internal_stop_do_start_req((const T_AS_START_REQ*) msg);
        }
        else
        {
            // Start player/ringer request while operating => recover mode.
            AS_TRACE_2(ERROR, "as_state_stopping()AS_START_REQ ", msg->msg_id, as_ctrl_blk->state); 
            as_enter_recover_mode(msg->msg_id, &((const T_AS_START_REQ*)msg)->rp);
        }

        // Do not change AS state.
        break;

    
    case AS_PAUSE_REQ:
 	// as_enter_recover_mode(msg->msg_id, &((const T_AS_PAUSE_REQ*)msg)->rp);
	 break;

    case AS_RESUME_REQ:
	// as_enter_recover_mode(msg->msg_id, &((const T_AS_RESUME_REQ*)msg)->rp);
	 break;

   case AS_FORREW_REQ:
	// as_enter_recover_mode(msg->msg_id, &((const T_AS_RESUME_REQ*)msg)->rp);
	 break;
   case AS_BT_CFG_REQ:
        // BT CFG request.
        as_do_bt_cfg_req ((const T_AS_BT_CFG_REQ *) msg);
        break;

    default:
        AS_TRACE_2(ERROR,
                   "state_stopping() unexpected message %d received in state %d: discarded",
                   msg->msg_id, as_ctrl_blk->state);
        rc = RVM_INTERNAL_ERR;
        break;
    }

    return rc;
}


/**
 * @brief Process messages received while in RESTORING state.
 *
 * This private function is intended to dispatch the received message
 * when the AS SWE is in the AS_STATE_RESTORING state.
 *
 * The following messages are supported in this state:
 * - AUDIO_FULL_ACCESS_WRITE_DONE: the AUDIO replied to an audio_full_access_write() call.
 * - AS_STOP_REQ: a client SWE requests to stop either the player or ringer.
 * - AS_START_REQ: a client SWE requests to start either the player or ringer.
 *
 * @param msg @in A pointer to the header of the message.
 *
 * @retval RVM_OK  The operation was successful.
 * @retval RVM_INTERNAL_ERR  The received message was unexpected.
 */
T_RVM_RETURN
as_state_restoring(const T_RV_HDR* msg)
{
    T_RVM_RETURN rc = RVM_OK;

    // Process the incoming message.
    switch (msg->msg_id)
    {
    case AUDIO_FULL_ACCESS_WRITE_DONE:
        // Audio replied to an audio_full_access_write() call.
        as_restoring_do_fa_write_done((const T_AUDIO_FULL_ACCESS_WRITE_DONE*) msg);
        break;

    case AS_STOP_REQ:
        if (as_ctrl_blk->internal_stop)
        {
            // Clear the internal stop flag, as a requested stop was just received.
            as_ctrl_blk->internal_stop = FALSE;
            // Save the return path where the reply the indication.
            as_ctrl_blk->client_rp = ((const T_AS_STOP_REQ*)msg)->rp;
        }
        else
        {
            // Stop player/ringer request while operating => recover mode.
            AS_TRACE_2(ERROR, "as_state_restoring()AS_STOP_REQ ", msg->msg_id, as_ctrl_blk->state); 
            as_enter_recover_mode(msg->msg_id, &((const T_AS_STOP_REQ*)msg)->rp);
        }

        // Do not change AS state.
        break;

    case AS_INTERNAL_STOP_REQ:
        // Internal stop player/ringer request.

        // This is a regular behavior, just discard the message.
        AS_DEBUG_0(LOW,
                   "state_restoring() discarded internal stop as AS is already stopping.");
        break;

    case AS_START_REQ:
        if (as_ctrl_blk->internal_stop)
        {
            // Clear the internal stop flag, as a requested stop was just received.
            as_ctrl_blk->internal_stop = FALSE;

            // Start player/ringer request.
            as_internal_stop_do_start_req((const T_AS_START_REQ*) msg);
        }
        else
        {
            // Start player/ringer request while operating => recover mode.
            AS_TRACE_2(ERROR, "as_state_restoring()AS_START_REQ ", msg->msg_id, as_ctrl_blk->state); 
            as_enter_recover_mode(msg->msg_id, &((const T_AS_START_REQ*)msg)->rp);

            // Do not change AS state.
        }
        break;

   
    case AS_PAUSE_REQ:
	// as_enter_recover_mode(msg->msg_id, &((const T_AS_PAUSE_REQ*)msg)->rp);
	 break;
 
   case AS_RESUME_REQ:
	// as_enter_recover_mode(msg->msg_id, &((const T_AS_RESUME_REQ*)msg)->rp);
	 break;

   case AS_FORREW_REQ:
	// as_enter_recover_mode(msg->msg_id, &((const T_AS_RESUME_REQ*)msg)->rp);
	 break;

   case AS_BT_CFG_REQ:
        // BT CFG request.
        as_do_bt_cfg_req ((const T_AS_BT_CFG_REQ *) msg);
        break;
    default:
        AS_TRACE_2(ERROR,
                   "state_restoring() unexpected message %d received in state %d: discarded",
                   msg->msg_id, as_ctrl_blk->state);
        rc = RVM_INTERNAL_ERR;
        break;
    }

    return rc;
}


/**
 * Generic message handling function. Required by RVM.
 *
 * This function is called every time the AS SWE is in WAITING state 
 * (from the RVM point of view) and a new message has arrived in its mailbox.
 *
 * The AS SWE also receives the ownership of the message. Thus it is
 * responsible to free the received message.
 *
 * @param a_msg @io A pointer to the header of the message.
 *
 * @return An error code telling if the processing is successful or not.
 *
 * @retval RVM_OK  The processing is successful.
 * @retval RVM_INTERNAL_ERR  The state machine is in an unknown state.
 * @retval RVM_MEMORY_ERR  Unable to free the received message.
 */
T_RVM_RETURN
as_env_handle_message(T_RV_HDR* a_msg)
{
    T_RVM_RETURN rc  = RVM_OK;
    T_RV_HDR*    msg = a_msg;

    AS_TRACE_0(DEBUG_LOW, "handle_message() called");

    while (msg != NULL)
    {
        // By default, free processed message.
        as_ctrl_blk->free_msg = TRUE;

        switch (as_ctrl_blk->state)
        {
        case AS_STATE_IDLE:
            rc = as_state_idle(msg);
            break;

        case AS_STATE_SETTING:
            rc = as_state_setting(msg);
            break;

        case AS_STATE_STARTING:
            rc = as_state_starting(msg);
            break;

        case AS_STATE_PLAYING:
//	AS_TRACE_1(DEBUG_LOW, "playing_process() as_handle_state = %x", as_ctrl_blk->state);
            rc = as_state_playing(msg);
            break;

        case AS_STATE_STOPPING:
//	     AS_TRACE_1(DEBUG_LOW, "playing_process_stop() as_handle_msg = %d", as_ctrl_blk->state);
            rc = as_state_stopping(msg);
            break;

        case AS_STATE_RESTORING:
            rc = as_state_restoring(msg);
            break;
	case AS_STATE_PAUSED:
		rc = as_state_paused(msg);
		break;

        default: 
            // Current state is unknown : inconsistency.
            AS_TRACE_L(ERROR,
                       "handle_message() inconsistency, unknown state=",
                       as_ctrl_blk->state);
            rc = RVM_INTERNAL_ERR;
            break; 
        }

        // Check if message need to be freed (not a saved START request).
        if (as_ctrl_blk->free_msg)
        {
            // Free received message.
            if (rvf_free_buf(msg) != RVF_OK)
            {
                AS_TRACE_0(ERROR, "handle_message() unable to free message");
                rc = RVM_MEMORY_ERR;
            }
        }

        // Check for saved START request when in IDLE state.
        if (as_ctrl_blk->state == AS_STATE_IDLE)
        {
            // Restore a potential saved request.
            msg = (T_RV_HDR*) as_request_restore();
        }
        else
        {
            // No more request to process. Terminate loop.
            msg = NULL;
        }
    }

    return rc;
}

/****************************************************************************/
/*                                                                          */
/*  File Name:  audio_midi.c	                                              */
/*                                                                          */
/*  Purpose:  This file contains all the functions used to manage MIDI      */
/*                                                                          */
/*  Version   0.1                                                           */
/*                                                                          */
/*  Date        Modification                                                */
/*  ------------------------------------                                    */
/*  11 June 2003 Create                                                     */
/*                                                                          */
/*  Author                                                                  */
/*     Fabrice Goucem                                                       */
/*                                                                          */
/* (C) Copyright 2003 by Texas Instruments Incorporated, All Rights Reserved*/
/****************************************************************************/


#ifdef AUDIO_MIDI

#include "rv/rv_defined_swe.h"
#ifdef RVM_AUDIO_MAIN_SWE
  #ifndef _WINDOWS
    #include "swconfig.cfg"
    #include "sys.cfg"
    #include "chipset.cfg"
  #endif

  /* include the usefull L1 header */
  #include "l1_confg.h"
  #include "l1sw.cfg"

  #include "rv/rv_general.h"
  #include "rvm/rvm_gen.h"
  #include "audio/audio_features_i.h"
  #include "audio/audio_ffs_i.h"
  #include "audio/audio_api.h"
  #include "audio/audio_structs_i.h"
  #include "audio/audio_var_i.h"
  #include "audio/audio_messages_i.h"
  #include "rvf/rvf_target.h"
  #include "audio/audio_const_i.h"
  #include "audio/audio_error_hdlr_i.h"
  #include "audio/audio_macro_i.h"
  #include "bae/bae_api.h"
#if WCP_PROF == 1
  extern prf_LogPointOfInterest(const char*);
#endif

  /* include the useful L1 header */
  #define BOOL_FLAG
  #define CHAR_FLAG
  #include "l1_types.h"
  #include "l1audio_cust.h"
  #include "l1audio_msgty.h"
  #include "l1audio_signa.h"
  #include "l1audio_const.h"
  #include "l1audio_defty.h"

 extern  T_MIDI_DMA_PARAM midi_buf;

#if(L1_BT_AUDIO ==1)||(L1_WCM ==1)
extern  void l1_audio_manager(UWORD8 *src, UWORD16 size);
#endif
#if(L1_BT_AUDIO ==1)
extern T_L1_BT_AUDIO bt_audio;
#endif



#if (L1_EXT_AUDIO_MGT == 1)

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_midi_message_switch	                              */
  /*                                                                              */
  /*    Purpose:  Manage the message supply                                       */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        start or stop message from midi features                              */
  /*                                                                              */
  /*    Output Parameters:                                                        */
  /*        index of the manager                                                  */
  /*                                                                              */
  /*    Note:                                                                     */
  /*        None.                                                                 */
  /*                                                                              */
  /*    Revision History:                                                         */
  /*        None.                                                                 */
  /*                                                                              */
  /********************************************************************************/
  UINT8 audio_midi_message_switch(T_RV_HDR *p_message)
  {
    switch(p_message->msg_id)
    {
      case AUDIO_MIDI_START_REQ:
      case AUDIO_MIDI_STOP_REQ:
        return(AUDIO_MIDI);
//omaps00090550        break;

      // driver init => check session_id is MIDI
      case AUDIO_DRIVER_INIT_STATUS_MSG:
      {
        UINT8 session_id=((T_AUDIO_DRIVER_INIT_STATUS *)p_message)->session_id;

        // session_id is MIDI
        if((session_id==AUDIO_EXT_MIDI_SESSION_ID) && (p_audio_gbl_var->midi.state!=AUDIO_IDLE))
          return(AUDIO_MIDI);
        else
          return(AUDIO_MIDI_NONE);
      }
//omaps00090550        break;

      case AUDIO_DRIVER_NOTIFICATION_MSG:
      {
        UWORD8 channel_id;
        channel_id=((T_AUDIO_DRIVER_NOTIFICATION *)p_message)->channel_id;
        if((channel_id==p_audio_gbl_var->midi.channel_id) && (p_audio_gbl_var->midi.state>AUDIO_WAIT_CHANNEL_ID))
          return(AUDIO_MIDI);
        else
          return(AUDIO_MIDI_NONE);
      }
//omaps00090550        break;

      case AUDIO_DRIVER_STATUS_MSG:
      {
        UWORD8 channel_id;
        channel_id=((T_AUDIO_DRIVER_STATUS *)p_message)->channel_id;
        if((channel_id==p_audio_gbl_var->midi.channel_id) && (p_audio_gbl_var->midi.state>AUDIO_WAIT_CHANNEL_ID))
          return(AUDIO_MIDI);
        else
          return(AUDIO_MIDI_NONE);
      }
//omaps00090550        break;

      default:
        return(AUDIO_MIDI_NONE);
//omaps00090550        break;
    } // switch
  }


  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_midi_send_status                                   */
  /*                                                                              */
  /*    Purpose:  This function sends the MIDI play status to the entity          */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        status,                                                               */
  /*        return path                                                           */
  /*                                                                              */
  /*    Output Parameters:                                                        */
  /*        None.                                                                 */
  /*                                                                              */
  /*    Note:                                                                     */
  /*        None.                                                                 */
  /*                                                                              */
  /*    Revision History:                                                         */
  /*        None.                                                                 */
  /*                                                                              */
  /********************************************************************************/
  void audio_midi_send_status(T_AUDIO_RET status, T_RV_RETURN *return_path)
  {
    T_AUDIO_MIDI_STATUS *p_send_message;
    T_RVF_MB_STATUS mb_status=RVF_RED;

    // allocate the message buffer
    while(mb_status==RVF_RED)
    {
      mb_status=rvf_get_buf(p_audio_gbl_var->mb_external,
                            sizeof(T_AUDIO_MIDI_STATUS),
                            (T_RVF_BUFFER **)(&p_send_message));

      // If insufficient resources, then report a memory error and abort
      // and wait until more ressource is given
      if(mb_status==RVF_RED)
      {
        audio_midi_error_trace(AUDIO_ENTITY_NO_MEMORY);
        rvf_delay(RVF_MS_TO_TICKS(1000));
      }
    }

    // fill the header of the message
    p_send_message->os_hdr.msg_id=AUDIO_MIDI_STATUS_MSG;

    // fill the status parameters
    p_send_message->status=status;

    // send message or call callback
    if(return_path->callback_func==NULL)
    {
      rvf_send_msg(return_path->addr_id,p_send_message);
    }
    else
    {
      (*(return_path->callback_func))((void *)p_send_message);
       rvf_free_buf((T_RVF_BUFFER *)p_send_message);
    }
  }



  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_midi_manager                                       */
  /*                                                                              */
  /*    Purpose:  This function is called to manage a MIDI play manager           */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        Message to the audio entity                                           */
  /*                                                                              */
  /*    Output Parameters:                                                        */
  /*        None.                                                                 */
  /*                                                                              */
  /*    Note:                                                                     */
  /*        None.                                                                 */
  /*                                                                              */
  /*    Revision History:                                                         */
  /*        None.                                                                 */
  /*                                                                              */
  /********************************************************************************/
  void audio_midi_manager(T_RV_HDR *p_message)
  {
    // Declare local variables
    T_RV_HDR *p_send_message;
    T_RV_RETURN return_path;
    T_RV_RET	audio_bae_return;

    // initialize the return path
    return_path.callback_func=NULL;
    return_path.addr_id=p_audio_gbl_var->addrId;

    /**************** audio_midi_manager function begins ***********************/
    switch(p_audio_gbl_var->midi.state)
    {
      case AUDIO_IDLE:
      {
        switch(p_message->msg_id)
        {
          case AUDIO_MIDI_START_REQ:
          {
            T_AUDIO_DRIVER_PARAMETER driver_parameter;

            /* save the return path + midi parameters */
            p_audio_gbl_var->midi.return_path.callback_func=((T_AUDIO_MIDI_START *)p_message)->return_path.callback_func;
            p_audio_gbl_var->midi.return_path.addr_id=((T_AUDIO_MIDI_START *)p_message)->return_path.addr_id;

            p_audio_gbl_var->midi.size=AUDIO_EXT_MIDI_BUFFER_SIZE;
            p_audio_gbl_var->midi.nb_buffer=AUDIO_EXT_MIDI_NB_BUF ;

            /* driver parameters */
            driver_parameter.nb_buffer   = AUDIO_EXT_MIDI_NB_BUF;
            driver_parameter.buffer_size = AUDIO_EXT_MIDI_BUFFER_SIZE;/*16 bit words*/

	          /* return_path for driver */
            return_path.callback_func    = NULL;
            return_path.addr_id          = p_audio_gbl_var->addrId;

            /* init driver */
            audio_driver_init_midi_session(&driver_parameter,&return_path);

            p_audio_gbl_var->midi.state=AUDIO_WAIT_CHANNEL_ID;
          }
          break;
          case AUDIO_MIDI_STOP_REQ:
          {
            audio_midi_error_trace(AUDIO_ERROR_STOP_EVENT);
            /* do not send a status message because of pre-emption issues
             An automatic stop can pre-empt a stop request. A status is sent + back in state idle
             then the stop request is received and another status is sent, which can be misinterpreted */
          }
          break;
        }
      }
      break;  /* AUDIO_IDLE */

      case AUDIO_WAIT_CHANNEL_ID:
      {
        switch(p_message->msg_id)
        {
          case AUDIO_DRIVER_INIT_STATUS_MSG:
          {
            UINT8 *play_buffer;

            /* check init is successfull otherwise, send status AUDIO_ERROR */
            if(((T_AUDIO_DRIVER_INIT_STATUS *)p_message)->status==AUDIO_OK)
            {
              /* get channel id */
              p_audio_gbl_var->midi.channel_id=((T_AUDIO_DRIVER_INIT_STATUS *)p_message)->channel_id;

              /* initializations */
              p_audio_gbl_var->midi.stop_req_allowed=TRUE;

              /* fill all buffers in advance */
              while(audio_driver_get_play_buffer(p_audio_gbl_var->midi.channel_id,&play_buffer)==AUDIO_OK)
              {
                  /* incrementation */
                  p_audio_gbl_var->midi.get_play_counter++;  

                  /* Call the BAE function to get audio data */
                  audio_bae_return = (T_RV_RET) bae_audio_data_request(play_buffer,p_audio_gbl_var->midi.size);//omaps00090550

		    #if(L1_BT_AUDIO ==1)
			if(bt_audio.connected_status ==TRUE)
			  {
				bt_audio.pcmblock.pcmBuffer=play_buffer;
				l1_audio_manager(bt_audio.pcmblock.pcmBuffer,p_audio_gbl_var->midi.size);
			   }
		    #endif
                  if (audio_bae_return!=RV_OK)
                  {
		                AUDIO_SEND_TRACE("AUDIO MIDI: bae_audio_data_request failed",RV_TRACE_LEVEL_DEBUG_LOW);
                    audio_midi_send_status(AUDIO_ERROR,&p_audio_gbl_var->midi.return_path);
                  }

                  audio_driver_play_midi_buffer(p_audio_gbl_var->midi.channel_id,play_buffer);
              }

              /* send message */
              audio_driver_start_session(p_audio_gbl_var->midi.channel_id,return_path);

              /* change state */
              p_audio_gbl_var->midi.state=AUDIO_WAIT_STOP_OR_NOTIFICATION;
            }
            else
            {
              audio_midi_error_trace(AUDIO_ERROR_START_EVENT);
              audio_midi_send_status(AUDIO_ERROR, &p_audio_gbl_var->midi.return_path);
              /* change state */
              p_audio_gbl_var->midi.state=AUDIO_IDLE;
            }
          }
          break;
          case AUDIO_MIDI_STOP_REQ:
            /* change state */
            p_audio_gbl_var->midi.state=AUDIO_WAIT_CHANNEL_ID_TO_STOP;
          break;
        }
      } /* case AUDIO_WAIT_CHANNEL_ID:*/
      break;

      case AUDIO_WAIT_STOP_OR_NOTIFICATION:
      {
        switch(p_message->msg_id)
        {
          case AUDIO_DRIVER_NOTIFICATION_MSG:
          {
            UINT8 *play_buffer;

            p_audio_gbl_var->midi.layer1_counter++;
            if(p_audio_gbl_var->midi.stop_req_allowed==TRUE) 
            {  /* try to get a buffer */
            if(audio_driver_get_play_buffer(p_audio_gbl_var->midi.channel_id,&play_buffer)==AUDIO_OK)
            {
              p_audio_gbl_var->midi.get_play_counter++;

              #if WCP_PROF == 1
			  prf_LogPointOfInterest("Start Play");
              #endif
			  /* Call the BAE function to get audio data */
              audio_bae_return = (T_RV_RET)bae_audio_data_request(play_buffer,p_audio_gbl_var->midi.size);//omaps00090550
		    #if(L1_BT_AUDIO ==1)
			if(bt_audio.connected_status ==TRUE)
			  {
				bt_audio.pcmblock.pcmBuffer=play_buffer;
				l1_audio_manager(bt_audio.pcmblock.pcmBuffer,p_audio_gbl_var->midi.size);
			   }
		    #endif
              #if WCP_PROF == 1
			  prf_LogPointOfInterest("Stop Play");
              #endif
			  
	            if (audio_bae_return!=RV_OK)
              {
                  AUDIO_SEND_TRACE("AUDIO MIDI: bae_audio_data_request failed",RV_TRACE_LEVEL_DEBUG_LOW);
                  audio_midi_send_status(AUDIO_ERROR,&p_audio_gbl_var->midi.return_path);
              }

//              audio_driver_play_midi_buffer(p_audio_gbl_var->midi.channel_id,play_buffer);
            audio_driver_play_midi_buffer(p_audio_gbl_var->midi.channel_id,play_buffer);

            } /* if(audio_driver_get_play_buffer(channel_id,&p_buffer)==AUDIO_OK) */
            else
              {
			  bt_audio.pcm_data_failed=1;
              AUDIO_SEND_TRACE("AUDIO MIDI: no buffer available",RV_TRACE_LEVEL_DEBUG_LOW);
            }
          }
		  else
            AUDIO_SEND_TRACE("AUDIO MIDI: already processed stop request. Not processing Notification msg",RV_TRACE_LEVEL_DEBUG_LOW);
          }
          break;  /* case AUDIO_DRIVER_NOTIFICATION_MSG */

          case AUDIO_MIDI_STOP_REQ:
            if(p_audio_gbl_var->midi.stop_req_allowed==TRUE)
            {
              p_audio_gbl_var->midi.stop_req_allowed=FALSE;
              audio_driver_stop_session(p_audio_gbl_var->midi.channel_id);
            }
            else
              AUDIO_SEND_TRACE("AUDIO MIDI: second stop request received",RV_TRACE_LEVEL_WARNING);
          break;

          case AUDIO_DRIVER_STATUS_MSG:
            audio_midi_send_status(AUDIO_OK,&p_audio_gbl_var->midi.return_path);
            p_audio_gbl_var->midi.state=AUDIO_IDLE;
          break;
        }
      }
      break; /* WAIT_STOP_OR_NOTIFICATION */

      case AUDIO_WAIT_CHANNEL_ID_TO_STOP:
      {
        switch(p_message->msg_id)
        {
          case AUDIO_DRIVER_INIT_STATUS_MSG:
          {
            if(((T_AUDIO_DRIVER_INIT_STATUS *)p_message)->status==AUDIO_OK)
            {
              /* get channel_id */
              p_audio_gbl_var->midi.channel_id=((T_AUDIO_DRIVER_INIT_STATUS *)p_message)->channel_id;

              audio_driver_stop_session(p_audio_gbl_var->midi.channel_id);

              /* change state */
              p_audio_gbl_var->midi.state=AUDIO_WAIT_DRIVER_STOP_CON;
            }
            else
            {
              audio_midi_send_status(AUDIO_OK,&p_audio_gbl_var->midi.return_path);

              /* change state */
              p_audio_gbl_var->midi.state=AUDIO_IDLE;
            }
          }
          break;
          case AUDIO_MIDI_STOP_REQ:
            audio_midi_error_trace(AUDIO_ERROR_STOP_EVENT);
          break;
        }
      } /* case AUDIO_WAIT_CHANNEL_ID_TO_STOP:*/
      break;
      case AUDIO_WAIT_DRIVER_STOP_CON:
      {
        switch(p_message->msg_id)
        {
          case AUDIO_DRIVER_STATUS_MSG:
          {
            if(((T_AUDIO_DRIVER_STATUS *)p_message)->status_type==AUDIO_STOP_STATUS)
            {
              
              audio_midi_send_status(((T_AUDIO_DRIVER_STATUS *)p_message)->status,
                                     &p_audio_gbl_var->midi.return_path);
              p_audio_gbl_var->midi.state=AUDIO_IDLE;
            }
          }
          break;
          case AUDIO_MIDI_STOP_REQ:
            audio_midi_error_trace(AUDIO_ERROR_STOP_EVENT);
          break;
        }
      } /*case AUDIO_WAIT_DRIVER_STOP_CON:*/
      break;
    }
  } /*********************** End of audio_midi_manager function **********************/

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_driver_midi_manager                                */
  /*                                                                              */
  /*    Purpose:  This function is called to manage a MIDI manager                */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        Message to the audio entity                                           */
  /*                                                                              */
  /*    Output Parameters:                                                        */
  /*        None.                                                                 */
  /*                                                                              */
  /*    Note:                                                                     */
  /*        None.                                                                 */
  /*                                                                              */
  /*    Revision History:                                                         */
  /*        None.                                                                 */
  /*                                                                              */
  /********************************************************************************/
  T_AUDIO_RET audio_driver_midi_manager(T_RV_HDR *p_message, T_AUDIO_DRIVER_SESSION *p_session)
  {
    /**************** audio_driver_midi_manager function begins ***********************/
    switch(p_session->session_info.state)
    {
      case AUDIO_DRIVER_CHANNEL_WAIT_INIT:
      {
        /* init buffer index, layer1 not valid until 1st buffer is filled */
        /* index_l1 will be set to 0 when get_play_buffer() is called in WAIT_START state */
        p_session->session_info.index_l1    = 0xFF;
        p_session->session_info.index_appli = 0;
        p_session->session_info.play_api_state = AUDIO_PLAY_API_STATE_GET_BUF;



        /* allocate the buffer for the message to the L1 */
        p_session->session_req.p_l1_send_message=audio_allocate_l1_message(sizeof(T_MMI_EXT_AUDIO_MGT_START_REQ));
        ((T_MMI_EXT_AUDIO_MGT_START_REQ *)(p_session->session_req.p_l1_send_message))->session_id=AUDIO_EXT_MIDI_SESSION_ID;
        ((T_MMI_EXT_AUDIO_MGT_START_REQ *)(p_session->session_req.p_l1_send_message))->source_buffer_address=(INT8 *)midi_buf.audio_play_buffer;
        ((T_MMI_EXT_AUDIO_MGT_START_REQ *)(p_session->session_req.p_l1_send_message))->sampling_frequency= p_audio_gbl_var->midi.sampling_rate;
        ((T_MMI_EXT_AUDIO_MGT_START_REQ *)(p_session->session_req.p_l1_send_message))->data_type=p_audio_gbl_var->midi.data_type;
        ((T_MMI_EXT_AUDIO_MGT_START_REQ *)(p_session->session_req.p_l1_send_message))->element_number=2;
        ((T_MMI_EXT_AUDIO_MGT_START_REQ *)(p_session->session_req.p_l1_send_message))->frame_number=p_audio_gbl_var->midi.frame_number;
        ((T_MMI_EXT_AUDIO_MGT_START_REQ *)(p_session->session_req.p_l1_send_message))->mono_stereo=p_audio_gbl_var->midi.channel;
        ((T_MMI_EXT_AUDIO_MGT_START_REQ *)(p_session->session_req.p_l1_send_message))->DMA_channel_number=AUDIO_DMA_CHANNEL;


        if(p_session->session_req.p_l1_send_message!=NULL)
          return(AUDIO_OK);
        else
          return(AUDIO_ERROR);
      }
//omaps00090550        break;

      case AUDIO_DRIVER_CHANNEL_WAIT_START:
      {
        /* send the start voice memo play message to the L1 */
        audio_send_l1_message(MMI_EXT_AUDIO_MGT_START_REQ,
                              p_session->session_req.p_l1_send_message);
        return(AUDIO_OK);
      }
//omaps00090550        break;

      case AUDIO_DRIVER_CHANNEL_WAIT_STOP:
      {
        /* send the stop command to the audio L1 */
        DummyStruct *p_send_message = audio_allocate_l1_message(0);
        if(p_send_message!=NULL)
        {
          /* send the stop command to the audio L1 */
          audio_send_l1_message(MMI_EXT_AUDIO_MGT_STOP_REQ,p_send_message);
          return(AUDIO_OK);
        }
        return(AUDIO_ERROR);
      }
//omaps00090550        break;
      case AUDIO_DRIVER_CHANNEL_WAIT_START_CON_TO_STOP:
      {
        /* send the stop command to the audio L1 */
        DummyStruct *p_send_message = audio_allocate_l1_message(0);
        if(p_send_message!=NULL)
        {
          /* send the stop command to the audio L1 */
          audio_send_l1_message(MMI_EXT_AUDIO_MGT_STOP_REQ,p_send_message);
          return(AUDIO_OK);
        }
        return(AUDIO_ERROR);
      }
//omaps00090550        break;
    }
    /* no corresponding message found? */
    return (AUDIO_ERROR);
  } /***************** End of audio_driver_midi_manager function ******************/


  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_midi_l1_simulator                                  */
  /*                                                                              */
  /*    Purpose:  This function simulates the L1 for MIDI                         */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        event: Event that triggered the function                              */
  /*        p_msg: Message (if any) associated with the event                     */
  /*                                                                              */
  /*    Note:                                                                     */
  /*        None.                                                                 */
  /*                                                                              */
  /*    Revision History:                                                         */
  /*        None.                                                                 */
  /*                                                                              */
  /********************************************************************************/
  void audio_midi_l1_simulator(UINT16 event, T_RV_HDR *p_message)
  {
#ifdef _WINDOWS
    enum { WAIT_START_REQ, WAIT_STOP };

    T_RVF_MB_STATUS mb_status;
    T_RV_RETURN *return_path=&(p_audio_gbl_var->audio_driver_session[p_audio_gbl_var->midi.channel_id].session_req.return_path);

    switch(p_audio_gbl_var->midi.l1_state)
    {
      case WAIT_START_REQ:
        if(p_message->msg_id==MMI_EXT_AUDIO_MGT_START_REQ)
        {
          rvf_start_timer(AUDIO_MIDI_L1_SIMUL_TIMER,
                          RVF_MS_TO_TICKS(1000),
                          AUDIO_MIDI_L1_SIMUL_ONE_SHOT_TIMER);
          p_audio_gbl_var->midi.counter=10;

          /* send MMI_EXT_AUDIO_MGT_START_CON message to the Riviera audio entity */
          mb_status=rvf_get_buf(p_audio_gbl_var->mb_internal,
                                sizeof(T_AUDIO_DRIVER_NOTIFICATION),
                                (T_RVF_BUFFER **)(&p_message));
          if(mb_status==RVF_RED)
          {
            AUDIO_SEND_TRACE("AUDIO entity has no memory for driver notification",RV_TRACE_LEVEL_ERROR);
            return;
          }
          ((T_AUDIO_DRIVER_NOTIFICATION *)p_message)->header.msg_id=MMI_EXT_AUDIO_MGT_START_CON;
          ((T_AUDIO_DRIVER_NOTIFICATION *)p_message)->channel_id=p_audio_gbl_var->midi.channel_id;
          ((T_AUDIO_DRIVER_NOTIFICATION *)p_message)->p_buffer=NULL;
          if(return_path->callback_func==NULL)
            rvf_send_msg(return_path->addr_id, p_message);
          else
          {
            (*return_path->callback_func)((void *)(p_message));
            rvf_free_buf((T_RVF_BUFFER *)p_message);
          }

          p_audio_gbl_var->midi.l1_state=WAIT_STOP;
          return;
        }
      break;

      case WAIT_STOP:
        if(event & AUDIO_MIDI_L1_SIMUL_TIMER_EVT_MASK)
        {
          p_audio_gbl_var->midi.counter--;

          /* switch buffer */
          {
            T_AUDIO_DRIVER_SESSION *p=&p_audio_gbl_var->audio_driver_session[p_audio_gbl_var->midi.channel_id];
            p->session_info.index_l1++;
            if(p->session_info.index_l1==p->session_req.nb_buffer) p->session_info.index_l1=0;
          }

          /* send notification message to the Riviera audio entity */
          mb_status=rvf_get_buf(p_audio_gbl_var->mb_internal,
                                sizeof(T_AUDIO_DRIVER_NOTIFICATION),
                                (T_RVF_BUFFER **)(&p_message));
          if(mb_status==RVF_RED)
          {
            AUDIO_SEND_TRACE("AUDIO entity has no memory for driver notification",RV_TRACE_LEVEL_ERROR);
            return;
          }
          ((T_AUDIO_DRIVER_NOTIFICATION *)p_message)->header.msg_id=AUDIO_DRIVER_NOTIFICATION_MSG;
          ((T_AUDIO_DRIVER_NOTIFICATION *)p_message)->channel_id=p_audio_gbl_var->midi.channel_id;
          ((T_AUDIO_DRIVER_NOTIFICATION *)p_message)->p_buffer=NULL;
          if(return_path->callback_func==NULL)
            rvf_send_msg(return_path->addr_id, p_message);
          else
          {
            (*return_path->callback_func)((void *)(p_message));
            rvf_free_buf((T_RVF_BUFFER *)p_message);
          }

          /* check if we're done with the simulation */
          if(p_audio_gbl_var->midi.counter==0)
          {
            rvf_stop_timer(AUDIO_MIDI_L1_SIMUL_TIMER);

            /* send MMI_EXT_AUDIO_MGT_STOP_CON message to the Riviera audio entity */
            mb_status=rvf_get_buf(p_audio_gbl_var->mb_internal,
                                  sizeof(T_AUDIO_DRIVER_NOTIFICATION),
                                  (T_RVF_BUFFER **)(&p_message));
            if(mb_status==RVF_RED)
            {
              AUDIO_SEND_TRACE("AUDIO entity has no memory for driver notification",RV_TRACE_LEVEL_ERROR);
              return;
            }
            ((T_AUDIO_DRIVER_NOTIFICATION *)p_message)->header.msg_id=MMI_EXT_AUDIO_MGT_STOP_CON;
            ((T_AUDIO_DRIVER_NOTIFICATION *)p_message)->channel_id=p_audio_gbl_var->midi.channel_id;
            ((T_AUDIO_DRIVER_NOTIFICATION *)p_message)->p_buffer=NULL;
            if(return_path->callback_func==NULL)
              rvf_send_msg(return_path->addr_id, p_message);
            else
            {
              (*return_path->callback_func)((void *)(p_message));
              rvf_free_buf((T_RVF_BUFFER *)p_message);
            }

            p_audio_gbl_var->midi.l1_state=WAIT_START_REQ;
            return;
          }
          rvf_start_timer(AUDIO_MIDI_L1_SIMUL_TIMER,
                          RVF_MS_TO_TICKS(1000),
                          AUDIO_MIDI_L1_SIMUL_ONE_SHOT_TIMER);
        }
        if(p_message->msg_id==MMI_EXT_AUDIO_MGT_STOP_REQ)
        {
          rvf_stop_timer(AUDIO_MIDI_L1_SIMUL_TIMER);

          /* send MMI_EXT_AUDIO_MGT_STOP_CON message to the Riviera audio entity */
          mb_status=rvf_get_buf(p_audio_gbl_var->mb_internal,
                                sizeof(T_AUDIO_DRIVER_NOTIFICATION),
                                (T_RVF_BUFFER **)(&p_message));
          if(mb_status==RVF_RED)
          {
            AUDIO_SEND_TRACE("AUDIO entity has no memory for driver notification",RV_TRACE_LEVEL_ERROR);
            return;
          }
          ((T_AUDIO_DRIVER_NOTIFICATION *)p_message)->header.msg_id=MMI_EXT_AUDIO_MGT_STOP_CON;
          ((T_AUDIO_DRIVER_NOTIFICATION *)p_message)->channel_id=p_audio_gbl_var->midi.channel_id;
          ((T_AUDIO_DRIVER_NOTIFICATION *)p_message)->p_buffer=NULL;
          if(return_path->callback_func==NULL)
            rvf_send_msg(return_path->addr_id, p_message);
          else
          {
            (*return_path->callback_func)((void *)(p_message));
            rvf_free_buf((T_RVF_BUFFER *)p_message);
          }

          p_audio_gbl_var->midi.l1_state=WAIT_START_REQ;
          return;
        }
      break;
    }
#endif /* _WINDOWS */
  }

#endif /* #if (L1_EXT_AUDIO_MGT == 1) */
#endif /* #ifdef RVM_AUDIO_MAIN_SWE */


#endif

/****************************************************************************/
/*                                                                          */
/*  Name        audio_bgd_task.c                                            */
/*                                                                          */
/*  Function    this file contains the main AUDIO BACKGROUND functionk      */
/*                                                                          */
/*  Version   1                                                             */
/*                                                                          */
/*  Date        Modification                                                */
/*  ------------------------------------                                    */
/*  19 June 2003  Create                                                    */
/*                                                                          */
/*  Author   Frederic Turgis                                                */
/*                                                                          */
/* (C) Copyright 2001 by Texas Instruments Incorporated, All Rights Reserved*/
/****************************************************************************/

#include "rv/rv_defined_swe.h"

#ifdef RVM_AUDIO_BGD_SWE

  #include "rv/rv_general.h"
  #include "audio/audio_bgd/audio_bgd_structs_i.h"
  #include "audio/audio_bgd/audio_bgd_macro_i.h"
  #include "audio/audio_bgd/audio_bgd_const_i.h"
  #include "audio/audio_bgd/audio_bgd_env_i.h"

  /********************************************************************************/
  /* Function         audio_bgd_core                                              */
  /*                                                                              */
  /* Description      Core of the audio background task, which waits for events   */
  /*                                                                              */
  /********************************************************************************/
  T_RV_RET audio_bgd_core(void)
  {
    /* Declare local variables */
    BOOLEAN       error         = FALSE;
    T_RV_HDR      *p_message    = NULL;
    UINT16        received_event= 0x0000;
    T_RVF_MB_STATUS      status;

    AUDIO_BGD_SEND_TRACE("AUDIO_BACKGROUD TASK started", RV_TRACE_LEVEL_DEBUG_HIGH);

    /* loop to process messages */
    while (error == FALSE)
    {
      /* Wait for the necessary events (all events and no time out). */
      received_event = rvf_wait ( AUDIO_BGD_ALL_EVENT_FLAGS, AUDIO_BGD_NOT_TIME_OUT);

      /* If an event is received, then ....*/
      if (received_event & AUDIO_BGD_TASK_MBOX_EVT_MASK)
      {
        /* Read the message in the audio mailbox */
        p_message = (T_RV_HDR *) rvf_read_mbox(AUDIO_BGD_MBOX);

        if (p_message != NULL)
        {
          p_audio_bgd_gbl_var->message_processed = FALSE;

          // Handle messages

          // Check message has been processed
          if (p_audio_bgd_gbl_var->message_processed == FALSE)
          {
            AUDIO_BGD_SEND_TRACE_PARAM(" AUDIO BGD ERROR (env). A wrong message is received ",
                                       p_message->msg_id, RV_TRACE_LEVEL_ERROR);
          }
          status = (T_RVF_MB_STATUS)rvf_free_buf((T_RVF_BUFFER *)p_message);
          if (status != RVF_GREEN)
          {
            AUDIO_BGD_SEND_TRACE(" AUDIO BGD ERROR (env). Can't deallocate message",
                                 RV_TRACE_LEVEL_ERROR);
          }
        } // if (p_message != NULL)
        else
        {
          AUDIO_BGD_SEND_TRACE(" AUDIO BGD ERROR (env). Can't read the message received",
                               RV_TRACE_LEVEL_ERROR);
        }
      } // if (received_event & AUDIO_BGD_TASK_MBOX_EVT_MASK)

      /* If one of the occured events is unexpected (due to an unassigned */
      /* mailbox), then report an internal error.                         */
      if ( received_event & ~(AUDIO_BGD_TASK_MBOX_EVT_MASK) )
      {
        AUDIO_BGD_SEND_TRACE(" AUDIO BGD ERROR (env). One of the events is unexpected ",
                                    RV_TRACE_LEVEL_ERROR);

        error = TRUE;
      }
    } // while (error == FALSE)
    return(RV_INTERNAL_ERR);
  } /****************************** End of audio_bgd task function **************************/

#endif /* #ifdef RVM_AUDIO_BGD_SWE */

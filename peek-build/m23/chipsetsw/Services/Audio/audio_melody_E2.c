/****************************************************************************/
/*                                                                          */
/*  File Name:  audio_melody_E2.c                                           */
/*                                                                          */
/*  Purpose:  This file contains all the functions used to manage the       */
/*            melody_E2.                                                    */
/*                                                                          */
/*  Version    .1                                                           */
/*                                                                          */
/*  Date        Modification                                                */
/*  ------------------------------------                                    */
/*  10 dicember 2001 Create                                                 */
/*                                                                          */
/*  Author                                                                  */
/*     Stephanie Gerthoux                                                   */
/*                                                                          */
/* (C) Copyright 2001 by Texas Instruments Incorporated, All Rights Reserved*/
/****************************************************************************/



#ifdef RVM_AUDIO_MAIN_SWE
#include "rv/rv_defined_swe.h"
  #ifndef _WINDOWS
    #include "swconfig.cfg"
    #include "sys.cfg"
    #include "chipset.cfg"
  #endif

 /* include the usefull L1 header */
  #include "l1_confg.h"

 #if (MELODY_E2)
  #include "rv/rv_general.h"
  #include "rvm/rvm_gen.h"
  #include "audio/audio_features_i.h"
#if (AS_RFS_API == 1)
  #include "rfs/rfs_api.h"
#else
  #include "audio/audio_ffs_i.h"
#endif
  #include "audio/audio_api.h"
  #include "audio/audio_structs_i.h"
  #include "audio/audio_var_i.h"
  #include "audio/audio_messages_i.h"
  #include "rvf/rvf_target.h"
  #include "audio/audio_const_i.h"
  #include "audio/audio_error_hdlr_i.h"

  /* include the usefull L1 header */
  #define BOOL_FLAG
  #include "l1_types.h"
  #include "l1audio_cust.h"
  #include "l1audio_defty.h"
  #include "l1audio_msgty.h"
  #include "l1audio_signa.h"

  #ifndef _WINDOWS
    #if TESTMODE
      #include "l1tm_defty.h"
    #endif
    #if (L1_GTT == 1)
      #include "l1gtt_const.h"
      #include "l1gtt_defty.h"
    #endif

    #include "l1audio_const.h"
    #include "l1audio_varex.h"
    #include "l1_const.h"
    #if (L1_MP3 == 1)
      #include "l1mp3_defty.h"
    #endif
    #if (L1_AAC == 1)
      #include "l1aac_defty.h"
    #endif
    #include "l1_defty.h"
    #include "l1_varex.h"
  #endif

  #include <string.h>

  /* external prototype */
  extern UWORD16 Cust_audio_melody_E2_load_instrument (UWORD8 customer_instrument_id,
                                                       volatile UWORD16 *API_address,
                                                       UWORD16 allowed_size);

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_melody_E2_send_status                              */
  /*                                                                              */
  /*    Purpose:  This function sends the melody_E2 status to the entity.         */
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
  void audio_melody_E2_send_status (T_AUDIO_RET status, T_RV_RETURN return_path)
  {
    T_AUDIO_MELODY_E2_STATUS *p_send_message;
    T_RVF_MB_STATUS mb_status;

    /* allocate the message buffer */
    mb_status = rvf_get_buf (p_audio_gbl_var->mb_external,
                             sizeof (T_AUDIO_MELODY_E2_STATUS),
                             (T_RVF_BUFFER **) (&p_send_message));

    /* If insufficient resources, then report a memory error and abort.            */
    if (mb_status == RVF_RED)
    {
      audio_melody_E2_error_trace(AUDIO_ENTITY_NO_MEMORY);
    }

    /*fill the header of the message */
    ((T_AUDIO_MELODY_E2_STATUS *)p_send_message)->os_hdr.msg_id = AUDIO_MELODY_E2_STATUS_MSG;

    /* fill the status parameters */
    ((T_AUDIO_MELODY_E2_STATUS *)p_send_message)->status = status;

    if (return_path.callback_func == NULL)
    {
      /* send the message to the entity */
      rvf_send_msg (return_path.addr_id,
                    p_send_message);
    }
    else
    {
      /* call the callback function */
      (*return_path.callback_func)((void *)p_send_message);
      rvf_free_buf((T_RVF_BUFFER *)p_send_message);
    }
  }

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_melody_E2_message_switch                           */
  /*                                                                              */
  /*    Purpose:  Manage the message supply between the melody manager 1 and      */
  /*              manager 0                                                       */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        start or stop message from the melody E2 features                     */
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
  UINT8 audio_melody_E2_message_switch (T_RV_HDR *p_message)
  {
    switch (p_message->msg_id)
    {
      case AUDIO_MELODY_E2_START_REQ:
      {
        /* Check if the melody E2 number 0 isn't started */
        if (p_audio_gbl_var->melody_E2_0.state == AUDIO_MELODY_E2_IDLE)
        {
          /* Save the mode of the melody */
          p_audio_gbl_var->melody_E2_mode =
            ((T_AUDIO_MELODY_E2_START *)p_message)->melody_E2_parameter.melody_E2_mode;
          return(AUDIO_MELODY_E2_0);
        }
        /* Check if the melody E2 number 1 isn't started and if the current mode is game mode */
        if ( (p_audio_gbl_var->melody_E2_1.state == AUDIO_MELODY_E2_IDLE) &&
             (p_audio_gbl_var->melody_E2_mode == AUDIO_MELODY_GAME_MODE) )
        {
          return(AUDIO_MELODY_E2_1);
        }

        /* This start command is an error  */
        /* error due to a wrong melody mode */
        if ( (p_audio_gbl_var->melody_E2_1.state == AUDIO_MELODY_E2_IDLE) &&
             (p_audio_gbl_var->melody_E2_mode == AUDIO_MELODY_NORMAL_MODE) )
        {
          /* Close the FFS file previously open by the audio start API*/
          #ifndef _WINDOWS
#if(AS_RFS_API == 1)
			if ( rfs_close(((T_AUDIO_MELODY_E2_START *)(p_message))->audio_E2_ffs_fd) != RFS_EOK )
#else
			if ( ffs_close(((T_AUDIO_MELODY_E2_START *)(p_message))->audio_E2_ffs_fd) != EFFS_OK )
#endif

            {
              audio_melody_E2_error_trace(AUDIO_ENTITY_FILE_NO_CLOSE);
            }
          #endif
          audio_melody_E2_send_status (AUDIO_MODE_ERROR, ((T_AUDIO_MELODY_E2_START *)(p_message))->return_path);
          audio_melody_E2_error_trace(AUDIO_ERROR_MODE);
          p_audio_gbl_var->message_processed = TRUE;
          return(AUDIO_MELODY_E2_NONE);
        }

        /* error due to a wrong start */
        /* Close the FFS file previously open by the audio start API*/
        #ifndef _WINDOWS
#if(AS_RFS_API == 1)
		  if ( rfs_close(((T_AUDIO_MELODY_E2_START *)(p_message))->audio_E2_ffs_fd) != RFS_EOK )
#else
		  if ( ffs_close(((T_AUDIO_MELODY_E2_START *)(p_message))->audio_E2_ffs_fd) != EFFS_OK )
#endif
          {
            audio_melody_E2_error_trace(AUDIO_ENTITY_FILE_NO_CLOSE);
          }
        #endif
        audio_melody_E2_send_status (AUDIO_ERROR, ((T_AUDIO_MELODY_E2_START *)(p_message))->return_path);
        audio_melody_E2_error_trace(AUDIO_ERROR_START_EVENT);
        p_audio_gbl_var->message_processed = TRUE;
        return(AUDIO_MELODY_E2_NONE);
      }

      case AUDIO_MELODY_E2_STOP_REQ:
      {
        /* Check if the melody E2 number 0 is running and if the stop command and */
        /* the manager 0 talk about the same melody name */
#if(AS_RFS_API == 1)
		if ( (p_audio_gbl_var->melody_E2_0.state != AUDIO_MELODY_E2_IDLE) &&
              (wstrcmp(p_audio_gbl_var->melody_E2_0.melody_name,
                  ((T_AUDIO_MELODY_E2_STOP*)p_message)->melody_E2_name) == 0) )
#else
		if ( (p_audio_gbl_var->melody_E2_0.state != AUDIO_MELODY_E2_IDLE) &&
              (strcmp(p_audio_gbl_var->melody_E2_0.melody_name,
                  ((T_AUDIO_MELODY_E2_STOP*)p_message)->melody_E2_name) == 0) )
#endif
        {
          return(AUDIO_MELODY_E2_0);
        }
        /* Check if the melody E2 number 1 is running and if the stop command and */
        /* the manager 1 talk about the same melody name */
#if(AS_RFS_API == 1)
		if ( (p_audio_gbl_var->melody_E2_1.state != AUDIO_MELODY_E2_IDLE) &&
              (wstrcmp(p_audio_gbl_var->melody_E2_1.melody_name,
                  ((T_AUDIO_MELODY_E2_STOP*)p_message)->melody_E2_name) == 0) )
#else
		if ( (p_audio_gbl_var->melody_E2_1.state != AUDIO_MELODY_E2_IDLE) &&
              (strcmp(p_audio_gbl_var->melody_E2_1.melody_name,
                  ((T_AUDIO_MELODY_E2_STOP*)p_message)->melody_E2_name) == 0) )
#endif
        {
          return(AUDIO_MELODY_E2_1);
        }

        /* the stop command isn't apply to a melody E2, it's an error */
        /* event error - send an error message*/
        audio_melody_E2_send_status (AUDIO_ERROR, ((T_AUDIO_MELODY_E2_STOP *)(p_message))->return_path);
        audio_melody_E2_error_trace(AUDIO_ERROR_STOP_EVENT);
        p_audio_gbl_var->message_processed = TRUE;
        return(AUDIO_MELODY_E2_NONE);
      }

      case AUDIO_FFS_INIT_DONE:
      {
        /* The FFS message is from the melody E2 manager 0 */
        if ( ((T_AUDIO_FFS_INIT *)p_message)->session_id == AUDIO_FFS_SESSION_MELODY_E2_0 )
        {
          return(AUDIO_MELODY_E2_0);
        }
        /* The FFS message is from the melody E2 manager 1 */
        if ( ((T_AUDIO_FFS_INIT *)p_message)->session_id == AUDIO_FFS_SESSION_MELODY_E2_1 )
        {
          return(AUDIO_MELODY_E2_1);
        }
        return(AUDIO_MELODY_E2_NONE);
      }

      case AUDIO_FFS_STOP_CON:
      {
        /* The FFS message is from the melody E2 manager 0 */
        if ( ((T_AUDIO_FFS_STOP_REQ *)p_message)->session_id == AUDIO_FFS_SESSION_MELODY_E2_0 )
        {
          return(AUDIO_MELODY_E2_0);
        }
        /* The FFS message is from the melody E2 manager 1 */
        if ( ((T_AUDIO_FFS_STOP_REQ *)p_message)->session_id == AUDIO_FFS_SESSION_MELODY_E2_1 )
        {
          return(AUDIO_MELODY_E2_1);
        }
        return(AUDIO_MELODY_E2_NONE);
      }

      case MMI_MELODY0_E2_START_CON:
      case MMI_MELODY0_E2_STOP_CON:
      {
        return(AUDIO_MELODY_E2_0);
      }

      case MMI_MELODY1_E2_START_CON:
      case MMI_MELODY1_E2_STOP_CON:
      {
        return(AUDIO_MELODY_E2_1);
      }

      default:
      {
        return(AUDIO_MELODY_E2_NONE);
        break;
      }

    } /* switch */
  }

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_melody_e2_convert_parameter                        */
  /*                                                                              */
  /*    Purpose:  Convert the melody parameters from the entity to the l1         */
  /*              parameters                                                      */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        session_id of the melody                                              */
  /*        id of the melody                                                      */
  /*                                                                              */
  /*    Output Parameters:                                                        */
  /*        layer 1  melody message                                               */
  /*                                                                              */
  /*    Note:                                                                     */
  /*        None.                                                                 */
  /*                                                                              */
  /*    Revision History:                                                         */
  /*        None.                                                                 */
  /*                                                                              */
  /********************************************************************************/
  void audio_melody_e2_convert_parameter(UINT8 channel_id, T_MMI_MELODY_E2_REQ *l1_parameter, UINT8 melody_id)
  {
    if (melody_id == 0)
    {
      /* loopback mode */
      l1_parameter->loopback =
        p_audio_gbl_var->melody_E2_0.loopback;
    }
    else
    {
      /* loopback mode */
      l1_parameter->loopback =
        p_audio_gbl_var->melody_E2_1.loopback;
    }
    l1_parameter->session_id = channel_id;
  }

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_melody_E2_manager_0                                */
  /*                                                                              */
  /*    Purpose:  This function is called to manage a melody E2 format number 0.  */
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
  void audio_melody_E2_manager_0 (T_RV_HDR *p_message)
  {
    /* Declare local variables */
    T_RVF_MB_STATUS mb_status;

    /**************** audio_melody_e2_manager function begins *********************/

    switch(p_audio_gbl_var->melody_E2_0.state)
    {
      case AUDIO_MELODY_E2_IDLE:
      {
        switch(p_message->msg_id)
        {
          case AUDIO_MELODY_E2_START_REQ:
          {
            T_AUDIO_FFS_FLASH_2_RAM_START *p_send_message;

            /* save the task id of the entity */
            p_audio_gbl_var->melody_E2_0.task_id = p_message->src_addr_id;

            /* save the return path */
            p_audio_gbl_var->melody_E2_0.return_path.callback_func =
              ((T_AUDIO_MELODY_E2_START*)(p_message))->return_path.callback_func;
            p_audio_gbl_var->melody_E2_0.return_path.addr_id   =
              ((T_AUDIO_MELODY_E2_START*)(p_message))->return_path.addr_id;

            /* Before to start any melody, check if the instrument file is download*/
            if (p_audio_gbl_var->melody_E2_load_file_instruments.file_downloaded == FALSE)
            {
               /* The instrument file is not downloaded*/
               audio_melody_E2_send_status (AUDIO_ERROR,((T_AUDIO_MELODY_E2_START *)p_message)->return_path);
               audio_melody_E2_error_trace(AUDIO_ERROR_NOT_DOWNLOAD);
               break;
            }

#if(AS_RFS_API == 1)
				wstrcpy( p_audio_gbl_var->melody_E2_0.melody_name,
                ((T_AUDIO_MELODY_E2_START*)(p_message))->melody_E2_parameter.melody_E2_name);
#else
			  strcpy( p_audio_gbl_var->melody_E2_0.melody_name,
                ((T_AUDIO_MELODY_E2_START*)(p_message))->melody_E2_parameter.melody_E2_name);
#endif

            p_audio_gbl_var->melody_E2_0.loopback =
              ((T_AUDIO_MELODY_E2_START*)(p_message))->melody_E2_parameter.E2_loopback;

            /* Send the Start message to the FFS */
            /* allocate the message buffer */
            mb_status = rvf_get_buf (p_audio_gbl_var->mb_internal,
                                     sizeof (T_AUDIO_FFS_FLASH_2_RAM_START),
                                     (T_RVF_BUFFER **) (&p_send_message));

            /* If insufficient resources, then report a memory error and abort.               */
            if (mb_status == RVF_RED)
            {
              audio_melody_E2_error_trace(AUDIO_ENTITY_NO_MEMORY);
            }

            /* fill the header of the message */
            ((T_AUDIO_FFS_FLASH_2_RAM_START *)p_send_message)->os_hdr.msg_id = AUDIO_FFS_FLASH_2_RAM_START_REQ;

            /* fill the parameters */
            ((T_AUDIO_FFS_FLASH_2_RAM_START *)p_send_message)->audio_ffs_fd =
              ((T_AUDIO_MELODY_E2_START*)p_message)->audio_E2_ffs_fd;
            ((T_AUDIO_FFS_FLASH_2_RAM_START *)p_send_message)->initial_size = AUDIO_MELODY_E2_SIZE;
            ((T_AUDIO_FFS_FLASH_2_RAM_START *)p_send_message)->loop =
              ((T_AUDIO_MELODY_E2_START*)p_message)->melody_E2_parameter.E2_loopback;

            ((T_AUDIO_FFS_FLASH_2_RAM_START *)p_send_message)->session_id = AUDIO_FFS_SESSION_MELODY_E2_0;

            /* send the message to the entity */
            rvf_send_msg (p_audio_gbl_var->audio_ffs_addrId,
                          p_send_message);

            /* change to the state AUDIO_MELODY_E2_WAIT_INIT_FFS */
            p_audio_gbl_var->melody_E2_0.state = AUDIO_MELODY_E2_WAIT_INIT_FFS;
            break;
          } /* case AUDIO_MELODY_E2_START_REQ */
        } /* switch(p_message->msg_id) */
        break;
      } /* case AUDIO_MELODY_E2_IDLE */

      case AUDIO_MELODY_E2_WAIT_INIT_FFS:
      {
        switch(p_message->msg_id)
        {
           case AUDIO_FFS_INIT_DONE:
           {
              DummyStruct *p_send_message;

              /* send the start melody message to the L1 */
              /* allocate the buffer for the message to the L1 */
              p_send_message = audio_allocate_l1_message(sizeof(T_MMI_MELODY_E2_REQ));

              if ( p_send_message != NULL)
              {
                /* Convert the entity parameters to the audio L1 parameters */
                audio_melody_e2_convert_parameter(((T_AUDIO_FFS_INIT *)p_message)->channel_id,
                                                  ((T_MMI_MELODY_E2_REQ *)p_send_message),
                                                  AUDIO_MELODY_E2_0);

                /* send the start command to the audio L1 */
                audio_send_l1_message(MMI_MELODY0_E2_START_REQ, p_send_message);
              }

              /* change to the state AUDIO_MELODY_E2_WAIT_START_CONFIRMATION */
              p_audio_gbl_var->melody_E2_0.state = AUDIO_MELODY_E2_WAIT_START_CON;
              break;
           } /* case AUDIO_FFS_INIT_DONE */

           case AUDIO_MELODY_E2_STOP_REQ:
           {
              /*.Before stopping this task, control that            */
              /* stop task id caller =Melody task id                */
                 if ( p_audio_gbl_var->melody_E2_0.task_id == p_message->src_addr_id)
                 {
                    /* change to the state AUDIO_MELODY_E2_WAIT_INIT_DONE */
                    p_audio_gbl_var->melody_E2_0.state = AUDIO_MELODY_E2_WAIT_INIT_DONE;
                 }
                 else
                 {
                   /* A stop request from an other task is sent during a start connection */
                   /* event error - send an error message  */
                   audio_melody_E2_send_status (AUDIO_ERROR,((T_AUDIO_MELODY_E2_STOP *)(p_message))->return_path);
                   audio_melody_E2_error_trace(AUDIO_ERROR_STOP_EVENT);
                 }
              break;
           } /* case AUDIO_MELODY_E2_STOP_REQ */
        } /* switch(p_message->msg_id) */
        break;
      } /* case AUDIO_MELODY_E2_WAIT_INIT_FFS */

      case AUDIO_MELODY_E2_WAIT_INIT_DONE:
      {
        switch(p_message->msg_id)
        {
           case AUDIO_FFS_INIT_DONE:
           {
              T_AUDIO_FFS_STOP_REQ *p_send_message;

              /* Send FFS stop command */
              /* allocate the message buffer */
              mb_status = rvf_get_buf (p_audio_gbl_var->mb_internal,
                                       sizeof (T_AUDIO_FFS_STOP_REQ),
                                       (T_RVF_BUFFER **) (&p_send_message));

              /* If insufficient resources, then report a memory error and abort.               */
              if (mb_status == RVF_RED)
              {
                audio_melody_E2_error_trace(AUDIO_ENTITY_NO_MEMORY);
              }

              /* fill the header of the message */
              ((T_AUDIO_FFS_STOP_REQ *)p_send_message)->os_hdr.msg_id = AUDIO_FFS_STOP_REQ;

              /* fill the parameter */
              ((T_AUDIO_FFS_STOP_REQ *)p_send_message)->session_id = AUDIO_FFS_SESSION_MELODY_E2_0;

              /* send the message to the entity */
              rvf_send_msg (p_audio_gbl_var->audio_ffs_addrId,
                            p_send_message);

              /* change to the state AUDIO_MELODY_E2_WAIT_STOP_FFS */
              p_audio_gbl_var->melody_E2_0.state = AUDIO_MELODY_E2_WAIT_STOP_FFS;
            break;
           } /* case AUDIO_FFS_INIT_DONE */

           case AUDIO_MELODY_E2_STOP_REQ:
           {
              /* stop event error - send an error message  */
              audio_melody_E2_send_status (AUDIO_ERROR,((T_AUDIO_MELODY_E2_STOP *)(p_message))->return_path);
              audio_melody_E2_error_trace(AUDIO_ERROR_STOP_EVENT);
              break;
            } /* case AUDIO_MELODY_E2_STOP_REQ */
        } /* switch(p_message->msg_id) */
        break;
      } /* case AUDIO_MELODY_E2_WAIT_INIT_DONE*/

      case AUDIO_MELODY_E2_WAIT_START_CON:
      {
        switch(p_message->msg_id)
        {
           case MMI_MELODY0_E2_START_CON:
           {
              /* change to the state AUDIO_MELODY_E2_WAIT_STOP_COMMAND */
              p_audio_gbl_var->melody_E2_0.state = AUDIO_MELODY_E2_WAIT_STOP_COMMAND;
              break;
           } /* case MMI_MELODY0_E2_START_CON */

           case AUDIO_MELODY_E2_STOP_REQ:
           {
              /*.Before stopping this task, control that            */
              /* stop task id caller =Melody task id                */
              if ( p_audio_gbl_var->melody_E2_0.task_id == p_message->src_addr_id)
              {
                  /* change to the state AUDIO_MELODY_E2_WAIT_STOP_CONFIRMATION */
                  p_audio_gbl_var->melody_E2_0.state = AUDIO_MELODY_E2_WAIT_START_CON_TO_STOP;
              }
              else
              {
                /* A stop request from an other task is sent during a start connection */
                /* event error - send an error message  */
                 audio_melody_E2_send_status (AUDIO_ERROR,((T_AUDIO_MELODY_E2_STOP *)(p_message))->return_path);
                 audio_melody_E2_error_trace(AUDIO_ERROR_STOP_EVENT);
              }
              break;
           } /* case AUDIO_MELODY_E2_STOP_REQ */
        } /* switch(p_message->msg_id) */
      break;
      } /* case AUDIO_MELODY_E2_PLAY_WAIT_START_CON */

      case AUDIO_MELODY_E2_WAIT_START_CON_TO_STOP:
      {
        switch(p_message->msg_id)
        {
           case MMI_MELODY0_E2_START_CON:
            {
              DummyStruct *p_send_message;

              /* send the stop command to the audio L1 */
              /* allocate the buffer for the message to the L1 */
              p_send_message = audio_allocate_l1_message(0);
              if ( p_send_message != NULL)
              {
                /* send the stop command to the audio L1 */
                audio_send_l1_message(MMI_MELODY0_E2_STOP_REQ, p_send_message);
              }

              /* change to the state AUDIO_MELODY_E2_WAIT_STOP_CON */
              p_audio_gbl_var->melody_E2_0.state = AUDIO_MELODY_E2_WAIT_STOP_CON;
              break;
            }

           case AUDIO_MELODY_E2_STOP_REQ:
            {
               /* stop event error - send an error message  */
               audio_melody_E2_send_status (AUDIO_ERROR,((T_AUDIO_MELODY_E2_STOP *)p_message)->return_path);
               audio_melody_E2_error_trace(AUDIO_ERROR_STOP_EVENT);
               break;
            } /* case AUDIO_MELODY_E2_STOP_REQ */
        } /* switch(p_message->msg_id) */
        break;
      } /* case AUDIO_MELODY_E2_WAIT_START_CON_TO_STOP */

      case AUDIO_MELODY_E2_WAIT_STOP_COMMAND:
        {
          switch(p_message->msg_id)
          {
             case MMI_MELODY0_E2_STOP_CON:
              {
                T_AUDIO_FFS_STOP_REQ *p_send_message;

                /* Send FFS stop command */
                /* allocate the message buffer */
                mb_status = rvf_get_buf (p_audio_gbl_var->mb_internal,
                                         sizeof (T_AUDIO_FFS_STOP_REQ),
                                         (T_RVF_BUFFER **) (&p_send_message));

                /* If insufficient resources, then report a memory error and abort.               */
                if (mb_status == RVF_RED)
                {
                  audio_melody_E2_error_trace(AUDIO_ENTITY_NO_MEMORY);
                }

                /* fill the header of the message */
                ((T_AUDIO_FFS_STOP_REQ *)p_send_message)->os_hdr.msg_id = AUDIO_FFS_STOP_REQ;

                /* fill the parameter */
                ((T_AUDIO_FFS_STOP_REQ *)p_send_message)->session_id = AUDIO_FFS_SESSION_MELODY_E2_0;

                /* send the message to the entity */
                rvf_send_msg (p_audio_gbl_var->audio_ffs_addrId,
                              p_send_message);

                /* change to the state AUDIO_MELODY_E2_WAIT_STOP_FFS */
                p_audio_gbl_var->melody_E2_0.state = AUDIO_MELODY_E2_WAIT_STOP_FFS;
               break;
              } /* case MMI_MELODY1_E2_STOP_CON */

             case AUDIO_MELODY_E2_STOP_REQ:
              {
                /*.Before stopping this task, control that            */
                /* stop task id caller = Melody task id               */
                if ( p_audio_gbl_var->melody_E2_0.task_id == p_message->src_addr_id)
                {
                  DummyStruct *p_send_message;

                  /* send the stop command to the audio L1 */
                  /* allocate the buffer for the message to the L1 */
                  p_send_message = audio_allocate_l1_message(0);
                  if (p_send_message != NULL)
                  {
                    /* send the start command to the audio L1 */
                    audio_send_l1_message(MMI_MELODY0_E2_STOP_REQ, p_send_message);
                  }

                  /* change to the state AUDIO_MELODY_E2_WAIT_STOP_CONFIRMATION */
                  p_audio_gbl_var->melody_E2_0.state = AUDIO_MELODY_E2_WAIT_STOP_CON;
                }
                else
                {
                 /* A stop request from an other task is sent during a start connection */
                 /* event error - send an error message  */
                 audio_melody_E2_send_status (AUDIO_ERROR,((T_AUDIO_MELODY_E2_STOP *)p_message)->return_path);
                 audio_melody_E2_error_trace(AUDIO_ERROR_STOP_EVENT);
                }
                break;
              } /* case AUDIO_MELODY_E2_STOP_REQ */
          } /* switch(p_message->msg_id) */
        break;
        } /* case AUDIO_MELODY_E2_WAIT_STOP_COMMAND */

        case AUDIO_MELODY_E2_WAIT_STOP_CON:
        {
          switch(p_message->msg_id)
          {
             case MMI_MELODY0_E2_STOP_CON:
              {
                T_AUDIO_FFS_STOP_REQ *p_send_message;

                /* Send FFS stop command */
                /* allocate the message buffer */
                mb_status = rvf_get_buf (p_audio_gbl_var->mb_internal,
                                         sizeof (T_AUDIO_FFS_STOP_REQ),
                                         (T_RVF_BUFFER **) (&p_send_message));

                /* If insufficient resources, then report a memory error and abort.               */
                if (mb_status == RVF_RED)
                {
                  audio_melody_E2_error_trace(AUDIO_ENTITY_NO_MEMORY);
                }

                /* fill the header of the message */
                ((T_AUDIO_FFS_STOP_REQ *)p_send_message)->os_hdr.msg_id = AUDIO_FFS_STOP_REQ;

                /* fill the parameter */
                ((T_AUDIO_FFS_STOP_REQ *)p_send_message)->session_id = AUDIO_FFS_SESSION_MELODY_E2_0;

                /* send the message to the entity */
                rvf_send_msg (p_audio_gbl_var->audio_ffs_addrId,
                              p_send_message);

                /* change to the state AUDIO_MELODY_E2_WAIT_STOP_FFS */
                 p_audio_gbl_var->melody_E2_0.state = AUDIO_MELODY_E2_WAIT_STOP_FFS;
               break;
              }

             case AUDIO_MELODY_E2_STOP_REQ:
              {
                /* stop event error - send an error message  */
                audio_melody_E2_send_status (AUDIO_ERROR,((T_AUDIO_MELODY_E2_STOP *)(p_message))->return_path);
                audio_melody_E2_error_trace(AUDIO_ERROR_STOP_EVENT);
                break;
              } /* case AUDIO_MELODY_E2_STOP_REQ */
          } /* switch(p_message->msg_id) */
        break;
        } /* case AUDIO_MELODY_E2_WAIT_STOP_CON */

        case AUDIO_MELODY_E2_WAIT_STOP_FFS:
        {
          switch(p_message->msg_id)
          {
             case AUDIO_FFS_STOP_CON:
              {
                /* change to the state AUDIO_MELODY_E2_IDLE */
                p_audio_gbl_var->melody_E2_0.state = AUDIO_MELODY_E2_IDLE;

                audio_melody_E2_send_status (AUDIO_OK, p_audio_gbl_var->melody_E2_0.return_path);

                break;
              }

              case AUDIO_MELODY_E2_STOP_REQ:
              {
                /* stop event error - send an error message  */
                audio_melody_E2_send_status (AUDIO_ERROR,((T_AUDIO_MELODY_E2_STOP *)(p_message))->return_path);
                audio_melody_E2_error_trace(AUDIO_ERROR_STOP_EVENT);
                break;
              } /* case AUDIO_MELODY_E2_STOP_REQ */
          } /* switch(p_message->msg_id) */
        break;
        } /* case AUDIO_MELODY_E2_WAIT_STOP_CON */

    } /* switch(p_audio_gbl_var->melody_E2_0.state) */

  } /*********************** End of audio_melody_E2_manager_0 function ************/

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_melody_E2_manager_1                                */
  /*                                                                              */
  /*    Purpose:  This function is called to manage a melody E2 format number 1.  */
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
  void audio_melody_E2_manager_1 (T_RV_HDR *p_message)
  {
    /* Declare local variables */
    T_RVF_MB_STATUS mb_status;

    /**************** audio_melody_e2_manager function begins *********************/

    switch(p_audio_gbl_var->melody_E2_1.state)
    {
      case AUDIO_MELODY_E2_IDLE:
      {
        switch(p_message->msg_id)
        {
          case AUDIO_MELODY_E2_START_REQ:
          {
            T_AUDIO_FFS_FLASH_2_RAM_START *p_send_message;

            /* save the task id of the entity */
            p_audio_gbl_var->melody_E2_1.task_id = p_message->src_addr_id;

            /* save the return path */
            p_audio_gbl_var->melody_E2_1.return_path.callback_func =
              ((T_AUDIO_MELODY_E2_START*)(p_message))->return_path.callback_func;
            p_audio_gbl_var->melody_E2_1.return_path.addr_id   =
              ((T_AUDIO_MELODY_E2_START*)(p_message))->return_path.addr_id;

            /* Before to start any melody, check if the instrument file is download*/
            if (p_audio_gbl_var->melody_E2_load_file_instruments.file_downloaded == FALSE)
            {
               /* The instrument file is not downloaded*/
               audio_melody_E2_send_status (AUDIO_ERROR,((T_AUDIO_MELODY_E2_START *)(p_message))->return_path);
               audio_melody_E2_error_trace(AUDIO_ERROR_NOT_DOWNLOAD);
               break;
            }

            strcpy( p_audio_gbl_var->melody_E2_1.melody_name,
                ((T_AUDIO_MELODY_E2_START*)(p_message))->melody_E2_parameter.melody_E2_name);

            p_audio_gbl_var->melody_E2_1.loopback =
              ((T_AUDIO_MELODY_E2_START*)(p_message))->melody_E2_parameter.E2_loopback;

            /* Send the Start message to the FFS */
            /* allocate the message buffer */
            mb_status = rvf_get_buf (p_audio_gbl_var->mb_internal,
                                     sizeof (T_AUDIO_FFS_FLASH_2_RAM_START),
                                     (T_RVF_BUFFER **) (&p_send_message));

            /* If insufficient resources, then report a memory error and abort.               */
            if (mb_status == RVF_RED)
            {
              audio_melody_E2_error_trace(AUDIO_ENTITY_NO_MEMORY);
            }

            /* fill the header of the message */
            ((T_AUDIO_FFS_FLASH_2_RAM_START *)p_send_message)->os_hdr.msg_id = AUDIO_FFS_FLASH_2_RAM_START_REQ;

            /* fill the parameters */
            ((T_AUDIO_FFS_FLASH_2_RAM_START *)p_send_message)->audio_ffs_fd =
              ((T_AUDIO_MELODY_E2_START*)p_message)->audio_E2_ffs_fd;
            ((T_AUDIO_FFS_FLASH_2_RAM_START *)p_send_message)->initial_size = AUDIO_MELODY_E2_SIZE;
            ((T_AUDIO_FFS_FLASH_2_RAM_START *)p_send_message)->loop =
              ((T_AUDIO_MELODY_E2_START*)p_message)->melody_E2_parameter.E2_loopback;

            ((T_AUDIO_FFS_FLASH_2_RAM_START *)p_send_message)->session_id = AUDIO_FFS_SESSION_MELODY_E2_1;

            /* send the message to the entity */
            rvf_send_msg (p_audio_gbl_var->audio_ffs_addrId,
                          p_send_message);

            /* change to the state AUDIO_MELODY_E2_WAIT_INIT_FFS */
            p_audio_gbl_var->melody_E2_1.state = AUDIO_MELODY_E2_WAIT_INIT_FFS;
            break;
          } /* case AUDIO_MELODY_E2_START_REQ */
        } /* switch(p_message->msg_id) */
        break;
      } /* case AUDIO_MELODY_E2_IDLE */

      case AUDIO_MELODY_E2_WAIT_INIT_FFS:
      {
        switch(p_message->msg_id)
        {
           case AUDIO_FFS_INIT_DONE:
           {
              DummyStruct *p_send_message;

              /* send the start melody message to the L1 */
              /* allocate the buffer for the message to the L1 */
              p_send_message = audio_allocate_l1_message(sizeof(T_MMI_MELODY_E2_REQ));

              if ( p_send_message != NULL)
              {
                /* Convert the entity parameters to the audio L1 parameters */
                audio_melody_e2_convert_parameter(((T_AUDIO_FFS_INIT *)p_message)->channel_id,
                                                  ((T_MMI_MELODY_E2_REQ *)p_send_message),
                                                  AUDIO_MELODY_E2_1);

                /* send the start command to the audio L1 */
                audio_send_l1_message(MMI_MELODY1_E2_START_REQ, p_send_message);
              }

              /* change to the state AUDIO_MELODY_E2_WAIT_START_CONFIRMATION */
              p_audio_gbl_var->melody_E2_1.state = AUDIO_MELODY_E2_WAIT_START_CON;
              break;
           } /* case AUDIO_FFS_INIT_DONE */

           case AUDIO_MELODY_E2_STOP_REQ:
           {
              /*.Before stopping this task, control that            */
              /* stop task id caller =Melody task id                */
                 if ( p_audio_gbl_var->melody_E2_1.task_id == p_message->src_addr_id)
                 {
                    /* change to the state AUDIO_MELODY_E2_WAIT_INIT_DONE */
                    p_audio_gbl_var->melody_E2_1.state = AUDIO_MELODY_E2_WAIT_INIT_DONE;
                 }
                 else
                 {
                   /* A stop request from an other task is sent during a start connection */
                   /* event error - send an error message  */
                   audio_melody_E2_send_status (AUDIO_ERROR,((T_AUDIO_MELODY_E2_STOP *)(p_message))->return_path);
                   audio_melody_E2_error_trace(AUDIO_ERROR_STOP_EVENT);
                 }
              break;
           } /* case AUDIO_MELODY_E2_STOP_REQ */
        } /* switch(p_message->msg_id) */
        break;
      } /* case AUDIO_MELODY_E2_WAIT_INIT_FFS */

      case AUDIO_MELODY_E2_WAIT_INIT_DONE:
      {
        switch(p_message->msg_id)
        {
           case AUDIO_FFS_INIT_DONE:
           {
              T_AUDIO_FFS_STOP_REQ *p_send_message;

              /* Send FFS stop command */
              /* allocate the message buffer */
              mb_status = rvf_get_buf (p_audio_gbl_var->mb_internal,
                                       sizeof (T_AUDIO_FFS_STOP_REQ),
                                       (T_RVF_BUFFER **) (&p_send_message));

              /* If insufficient resources, then report a memory error and abort.               */
              if (mb_status == RVF_RED)
              {
                audio_melody_E2_error_trace(AUDIO_ENTITY_NO_MEMORY);
              }

              /* fill the header of the message */
              ((T_AUDIO_FFS_STOP_REQ *)p_send_message)->os_hdr.msg_id = AUDIO_FFS_STOP_REQ;

              /* fill the parameter */
              ((T_AUDIO_FFS_STOP_REQ *)p_send_message)->session_id = AUDIO_FFS_SESSION_MELODY_E2_1;

              /* send the message to the entity */
              rvf_send_msg (p_audio_gbl_var->audio_ffs_addrId,
                            p_send_message);

              /* change to the state AUDIO_MELODY_E2_WAIT_STOP_FFS */
              p_audio_gbl_var->melody_E2_1.state = AUDIO_MELODY_E2_WAIT_STOP_FFS;
            break;
           } /* case AUDIO_FFS_INIT_DONE */

           case AUDIO_MELODY_E2_STOP_REQ:
           {
              /* stop event error - send an error message  */
              audio_melody_E2_send_status (AUDIO_ERROR,((T_AUDIO_MELODY_E2_STOP *)(p_message))->return_path);
              audio_melody_E2_error_trace(AUDIO_ERROR_STOP_EVENT);
              break;
            } /* case AUDIO_MELODY_E2_STOP_REQ */
        } /* switch(p_message->msg_id) */
        break;
      } /* case AUDIO_MELODY_E2_WAIT_INIT_DONE*/

      case AUDIO_MELODY_E2_WAIT_START_CON:
      {
        switch(p_message->msg_id)
        {
           case MMI_MELODY1_E2_START_CON:
           {
              /* change to the state AUDIO_MELODY_E2_WAIT_STOP_COMMAND */
              p_audio_gbl_var->melody_E2_1.state = AUDIO_MELODY_E2_WAIT_STOP_COMMAND;
              break;
           } /* case MMI_MELODY1_E2_START_CON */

           case AUDIO_MELODY_E2_STOP_REQ:
           {
              /*.Before stopping this task, control that            */
              /* stop task id caller =Melody task id                */
              if ( p_audio_gbl_var->melody_E2_1.task_id == p_message->src_addr_id)
              {
                  /* change to the state AUDIO_MELODY_E2_WAIT_STOP_CONFIRMATION */
                  p_audio_gbl_var->melody_E2_1.state = AUDIO_MELODY_E2_WAIT_START_CON_TO_STOP;
              }
              else
              {
                /* A stop request from an other task is sent during a start connection */
                /* event error - send an error message  */
                 audio_melody_E2_send_status (AUDIO_ERROR,((T_AUDIO_MELODY_E2_STOP *)(p_message))->return_path);
                 audio_melody_E2_error_trace(AUDIO_ERROR_STOP_EVENT);
              }
              break;
           } /* case AUDIO_MELODY_E2_STOP_REQ */
        } /* switch(p_message->msg_id) */
      break;
      } /* case AUDIO_MELODY_E2_PLAY_WAIT_START_CON */

      case AUDIO_MELODY_E2_WAIT_START_CON_TO_STOP:
      {
        switch(p_message->msg_id)
        {
           case MMI_MELODY1_E2_START_CON:
            {
              DummyStruct *p_send_message;

              /* send the stop command to the audio L1 */
              /* allocate the buffer for the message to the L1 */
              p_send_message = audio_allocate_l1_message(0);
              if ( p_send_message != NULL)
              {
                /* send the stop command to the audio L1 */
                audio_send_l1_message(MMI_MELODY1_E2_STOP_REQ, p_send_message);
              }

              /* change to the state AUDIO_MELODY_E2_WAIT_STOP_CON */
              p_audio_gbl_var->melody_E2_1.state = AUDIO_MELODY_E2_WAIT_STOP_CON;
              break;
            }

           case AUDIO_MELODY_E2_STOP_REQ:
            {
               /* stop event error - send an error message  */
               audio_melody_E2_send_status (AUDIO_ERROR,((T_AUDIO_MELODY_E2_STOP *)p_message)->return_path);
               audio_melody_E2_error_trace(AUDIO_ERROR_STOP_EVENT);
               break;
            } /* case AUDIO_MELODY_E2_STOP_REQ */
        } /* switch(p_message->msg_id) */
        break;
      } /* case AUDIO_MELODY_E2_WAIT_START_CON_TO_STOP */

      case AUDIO_MELODY_E2_WAIT_STOP_COMMAND:
        {
          switch(p_message->msg_id)
          {
             case MMI_MELODY1_E2_STOP_CON:
              {
                T_AUDIO_FFS_STOP_REQ *p_send_message;

                /* Send FFS stop command */
                /* allocate the message buffer */
                mb_status = rvf_get_buf (p_audio_gbl_var->mb_internal,
                                         sizeof (T_AUDIO_FFS_STOP_REQ),
                                         (T_RVF_BUFFER **) (&p_send_message));

                /* If insufficient resources, then report a memory error and abort.               */
                if (mb_status == RVF_RED)
                {
                  audio_melody_E2_error_trace(AUDIO_ENTITY_NO_MEMORY);
                }

                /* fill the header of the message */
                ((T_AUDIO_FFS_STOP_REQ *)p_send_message)->os_hdr.msg_id = AUDIO_FFS_STOP_REQ;

                /* fill the parameter */
                ((T_AUDIO_FFS_STOP_REQ *)p_send_message)->session_id = AUDIO_FFS_SESSION_MELODY_E2_1;

                /* send the message to the entity */
                rvf_send_msg (p_audio_gbl_var->audio_ffs_addrId,
                              p_send_message);

                /* change to the state AUDIO_MELODY_E2_WAIT_STOP_FFS */
                p_audio_gbl_var->melody_E2_1.state = AUDIO_MELODY_E2_WAIT_STOP_FFS;
               break;
              } /* case MMI_MELODY1_E2_STOP_CON */

             case AUDIO_MELODY_E2_STOP_REQ:
              {
                /*.Before stopping this task, control that            */
                /* stop task id caller = Melody task id               */
                if ( p_audio_gbl_var->melody_E2_1.task_id == p_message->src_addr_id)
                {
                  DummyStruct *p_send_message;

                  /* send the stop command to the audio L1 */
                  /* allocate the buffer for the message to the L1 */
                  p_send_message = audio_allocate_l1_message(0);
                  if (p_send_message != NULL)
                  {
                    /* send the start command to the audio L1 */
                    audio_send_l1_message(MMI_MELODY1_E2_STOP_REQ, p_send_message);
                  }

                  /* change to the state AUDIO_MELODY_E2_WAIT_STOP_CONFIRMATION */
                  p_audio_gbl_var->melody_E2_1.state = AUDIO_MELODY_E2_WAIT_STOP_CON;
                }
                else
                {
                 /* A stop request from an other task is sent during a start connection */
                 /* event error - send an error message  */
                 audio_melody_E2_send_status (AUDIO_ERROR,((T_AUDIO_MELODY_E2_STOP *)(p_message))->return_path);
                 audio_melody_E2_error_trace(AUDIO_ERROR_STOP_EVENT);
                }
                break;
              } /* case AUDIO_MELODY_E2_STOP_REQ */
          } /* switch(p_message->msg_id) */
        break;
        } /* case AUDIO_MELODY_E2_WAIT_STOP_COMMAND */

        case AUDIO_MELODY_E2_WAIT_STOP_CON:
        {
          switch(p_message->msg_id)
          {
             case MMI_MELODY1_E2_STOP_CON:
              {
                T_AUDIO_FFS_STOP_REQ *p_send_message;

                /* Send FFS stop command */
                /* allocate the message buffer */
                mb_status = rvf_get_buf (p_audio_gbl_var->mb_internal,
                                         sizeof (T_AUDIO_FFS_STOP_REQ),
                                         (T_RVF_BUFFER **) (&p_send_message));

                /* If insufficient resources, then report a memory error and abort.               */
                if (mb_status == RVF_RED)
                {
                  audio_melody_E2_error_trace(AUDIO_ENTITY_NO_MEMORY);
                }

                /* fill the header of the message */
                ((T_AUDIO_FFS_STOP_REQ *)p_send_message)->os_hdr.msg_id = AUDIO_FFS_STOP_REQ;

                /* fill the parameter */
                ((T_AUDIO_FFS_STOP_REQ *)p_send_message)->session_id = AUDIO_FFS_SESSION_MELODY_E2_1;

                /* send the message to the entity */
                rvf_send_msg (p_audio_gbl_var->audio_ffs_addrId,
                              p_send_message);

                /* change to the state AUDIO_MELODY_E2_WAIT_STOP_FFS */
                 p_audio_gbl_var->melody_E2_1.state = AUDIO_MELODY_E2_WAIT_STOP_FFS;
               break;
              }

             case AUDIO_MELODY_E2_STOP_REQ:
              {
                /* stop event error - send an error message  */
                audio_melody_E2_send_status (AUDIO_ERROR,((T_AUDIO_MELODY_E2_STOP *)(p_message))->return_path);
                audio_melody_E2_error_trace(AUDIO_ERROR_STOP_EVENT);
                break;
              } /* case AUDIO_MELODY_E2_STOP_REQ */
          } /* switch(p_message->msg_id) */
        break;
        } /* case AUDIO_MELODY_E2_WAIT_STOP_CON */

        case AUDIO_MELODY_E2_WAIT_STOP_FFS:
        {
          switch(p_message->msg_id)
          {
             case AUDIO_FFS_STOP_CON:
              {
                /* change to the state AUDIO_MELODY_E2_IDLE */
                p_audio_gbl_var->melody_E2_1.state = AUDIO_MELODY_E2_IDLE;

                audio_melody_E2_send_status (AUDIO_OK, p_audio_gbl_var->melody_E2_1.return_path);

                break;
              }

              case AUDIO_MELODY_E2_STOP_REQ:
              {
                /* stop event error - send an error message  */
                audio_melody_E2_send_status (AUDIO_ERROR,((T_AUDIO_MELODY_E2_STOP *)(p_message))->return_path);
                audio_melody_E2_error_trace(AUDIO_ERROR_STOP_EVENT);
                break;
              } /* case AUDIO_MELODY_E2_STOP_REQ */
          } /* switch(p_message->msg_id) */
        break;
        } /* case AUDIO_MELODY_E2_WAIT_STOP_CON */

    } /* switch(p_audio_gbl_var->melody_E2_1.state) */

  } /*********************** End of audio_melody_E2_manager_1 function ************/

    #ifndef _WINDOWS
      /*---------------------------------------------------------*/
      /* audio_background_melody_e2_download_instrument_manager()*/
      /*---------------------------------------------------------*/
      /*                                                         */
      /* Description:                                            */
      /* ------------                                            */
      /* This function is used to load/unload the instrument of  */
      /* the melodies E2.                                        */
      /*                                                         */
      /* Starting messages:                                      */
      /*                L1_BACK_MELODY_E2_LOAD_INSTRUMENT_REQ    */
      /*                L1_BACK_MELODY_E2_UNLOAD_INSTRUMENT_REQ  */
      /*                                                         */
      /* Result messages (input):  none                          */
      /*                                                         */
      /* Result messages (output):                               */
      /*                L1_BACK_MELODY_E2_LOAD_INSTRUMENT_CON    */
      /*                L1_BACK_MELODY_E2_UNLOAD_INSTRUMENT_CON  */
      /*                                                         */
      /* Reset messages (input):   none                          */
      /*                                                         */
      /* Stop message (input):     none.                         */
      /*                                                         */
      /* Stop message (output):                                  */
      /*                L1_BACK_MELODY_E2_LOAD_INSTRUMENT_CON    */
      /*                L1_BACK_MELODY_E2_UNLOAD_INSTRUMENT_CON  */
      /*                                                         */
      /* Rem: to stop immediatly the instrument download flags   */
      /* ---- is created:                                        */
      /* l1a_l1s_com.melody0_e2_task.parameters.emergency_stop   */
      /* l1a_l1s_com.melody1_e2_task.parameters.emergency_stop   */
      /*                                                         */
      /*---------------------------------------------------------*/
      void audio_background_melody_e2_download_instrument_manager(T_RV_HDR *p_message)
      {
      #if (PSP_STANDALONE != 1)
        DummyStruct      *p_confirm_message;
        UINT8            instrument_number, max_number_of_instrument, instrument_id, id;
        UINT16           size;
        UINT32           address;
        DummyStruct      *p_send_message;

        if (p_message->msg_id == L1_BACK_MELODY_E2_LOAD_INSTRUMENT_REQ)
        {
          // Load the instrument

          // Init the first address
          address = ( ((UINT32)(l1s_dsp_com.dsp_ndb_ptr->a_melody_e2_instrument_wave))
            - SC_AUDIO_MCU_API_BEGIN_ADDRESS );
          l1s_dsp_com.dsp_ndb_ptr->a_melody_e2_instrument_ptr[0] =
              (API)( (address>>1) + SC_AUDIO_DSP_API_BEGIN_ADDRESS );

          // Download the instrument
          max_number_of_instrument = ((T_L1_BACK_MELODY_E2_LOAD_INSTRUMENT_REQ *)(p_message))->number_of_instrument;
          for(instrument_number=0; instrument_number < max_number_of_instrument ; instrument_number++)
          {
            // No instrument was previously download
            if (audioback_melody_e2.number_of_user[instrument_number] == 0)
            {
              // load the insturment ID
              instrument_id =
                ((T_L1_BACK_MELODY_E2_LOAD_INSTRUMENT_REQ *)(p_message))->waves_table_id[instrument_number];

              // Find if this instrument was already downloaded
              id = 0;
              while ( (id < SC_AUDIO_MELODY_E2_MAX_NUMBER_OF_INSTRUMENT) &&
                      ((instrument_id != audioback_melody_e2.instrument_id[id]) ||
                       (audioback_melody_e2.number_of_user[id] == 0)) )
              {
                id++;
              }

              if (id < SC_AUDIO_MELODY_E2_MAX_NUMBER_OF_INSTRUMENT)
              {
                // This insturment was already downloaded
                // copy the address of this instrument
                l1s_dsp_com.dsp_ndb_ptr->a_melody_e2_instrument_ptr[instrument_number] =
                  l1s_dsp_com.dsp_ndb_ptr->a_melody_e2_instrument_ptr[id];

                // The size of this instrument is 0
                audioback_melody_e2.instrument_size[instrument_number] = 0;
              }
              else
              {
                // Load the customer instrument
                size = Cust_audio_melody_E2_load_instrument (
                        ((T_L1_BACK_MELODY_E2_LOAD_INSTRUMENT_REQ *)(p_message))->waves_table_id[instrument_number],
                        audioback_melody_e2.API_address,
                        audioback_melody_e2.allowed_size);

                // Added to stop the L1 in case of download error
                if (size == 0)
                {
                  // Send a message to stop the L1
                  /* send the stop command to the audio L1 */
                  /* allocate the buffer for the message to the L1 */
                  p_send_message = audio_allocate_l1_message(0);
                  if ( p_send_message != NULL)
                  {
                    /* send the stop command to the audio L1 */
                    if ( ((T_L1_BACK_MELODY_E2_LOAD_INSTRUMENT_REQ *)(p_message))->melody_id == 0)
                    {
                      audio_send_l1_message(MMI_MELODY0_E2_STOP_REQ, p_send_message);
                    }
                    else
                    {
                      audio_send_l1_message(MMI_MELODY1_E2_STOP_REQ, p_send_message);
                    }
                  }
                }

                // Save the size of this instrument
                audioback_melody_e2.instrument_size[instrument_number]= size;
              }

              // Update the Cust_audio_melody_E2_load_instrument argument
              audioback_melody_e2.API_address   += audioback_melody_e2.instrument_size[instrument_number];
              audioback_melody_e2.allowed_size  -= audioback_melody_e2.instrument_size[instrument_number];

              // Put the DSP address to the NDB API for the next instrument
              if (instrument_number < SC_AUDIO_MELODY_E2_MAX_NUMBER_OF_INSTRUMENT-1)
              {
                address = ( ((UWORD32)(audioback_melody_e2.API_address))
                  - SC_AUDIO_MCU_API_BEGIN_ADDRESS );

                l1s_dsp_com.dsp_ndb_ptr->a_melody_e2_instrument_ptr[instrument_number + 1] =
                    (API)( (address>>1) + SC_AUDIO_DSP_API_BEGIN_ADDRESS );
              }

              // Save the instrument ID
              audioback_melody_e2.instrument_id[instrument_number] = instrument_id;
            }

            // Increase the number of user of this instrument number
            audioback_melody_e2.number_of_user[instrument_number]++;
          }

          // Send the load confirmation message
          p_confirm_message = audio_allocate_l1_message(sizeof(T_L1_BACK_MELODY_E2_LOAD_INSTRUMENT_CON));
          if (p_confirm_message != NULL)
          {
            // Fill the parameter
            ((T_L1_BACK_MELODY_E2_LOAD_INSTRUMENT_CON *)p_confirm_message)->melody_id =
              ((T_L1_BACK_MELODY_E2_LOAD_INSTRUMENT_REQ *)p_message)->melody_id;

            /* send the start command to the audio L1 */
            audio_send_l1_message(L1_BACK_MELODY_E2_LOAD_INSTRUMENT_CON,
                                  p_confirm_message);
          }
        }
        else
        if (p_message->msg_id  == L1_BACK_MELODY_E2_UNLOAD_INSTRUMENT_REQ)
        {
          // Unload the instrument
          max_number_of_instrument = ((T_L1_BACK_MELODY_E2_UNLOAD_INSTRUMENT_REQ *)(p_message))->number_of_instrument;
          for(instrument_number=max_number_of_instrument; instrument_number =0; instrument_number--)
          {
            // Decrease the number of user of this instrument number
            audioback_melody_e2.number_of_user[instrument_number-1]--;

            // Check if the instrument must be removed
            if (audioback_melody_e2.number_of_user[instrument_number-1] == 0)
            {
              // Increase the size and decrease the pointer to the API with the size
              // of the removed instrument
              // Update the Cust_audio_melody_E2_load_instrument argument
              audioback_melody_e2.API_address   -= audioback_melody_e2.instrument_size[instrument_number-1];
              audioback_melody_e2.allowed_size  += audioback_melody_e2.instrument_size[instrument_number-1];
            }
          }

          // Send the unload confirmation message
          p_confirm_message = audio_allocate_l1_message(sizeof(T_L1_BACK_MELODY_E2_UNLOAD_INSTRUMENT_CON));
          if (p_confirm_message != NULL)
          {
            // Fill the parameter
            ((T_L1_BACK_MELODY_E2_UNLOAD_INSTRUMENT_CON *)(p_confirm_message))->melody_id =
              ((T_L1_BACK_MELODY_E2_UNLOAD_INSTRUMENT_REQ *)(p_message))->melody_id;

            /* send the start command to the audio L1 */
            audio_send_l1_message(L1_BACK_MELODY_E2_UNLOAD_INSTRUMENT_CON,
                                  p_confirm_message);
          }
        }
		#else
			return AUDIO_ERROR;
		#endif
      }
    #endif /* _WINDOWS */
  #endif /* MELODY_E2 */
#endif /* RVM_AUDIO_MAIN_SWE */
void dummy_function1(void )
{
;
}

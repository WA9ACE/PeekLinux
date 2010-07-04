/****************************************************************************/
/*                                                                          */
/*  File Name:  audio_melody_E1.c                                           */
/*                                                                          */
/*  Purpose:  This file contains all the functions used to manage the       */
/*            melody_E1.                                                    */
/*                                                                          */
/*  Version   0.1                                                           */
/*                                                                          */
/*  Date        Modification                                                */
/*  ------------------------------------                                    */
/*  18 May 2001 Create                                                      */
/*                                                                          */
/*  Author                                                                  */
/*     Francois Mazard - Stephanie Gerthoux                                 */
/*                                                                          */
/* (C) Copyright 2001 by Texas Instruments Incorporated, All Rights Reserved*/
/****************************************************************************/

#include "rv/rv_defined_swe.h"
#ifdef RVM_AUDIO_MAIN_SWE
  #ifndef _WINDOWS
    #include "swconfig.cfg"
    #include "sys.cfg"
    #include "chipset.cfg"
  #endif

  /* include the usefull L1 header */
  #include "l1_confg.h"

  #if (MELODY_E1)
  #include "rv/rv_general.h"
  #include "rvm/rvm_gen.h"
  #include "Audio/audio_features_i.h"
#if (AS_RFS_API == 1)
  #include "rfs/rfs_api.h"
#else
  #include "Audio/audio_ffs_i.h"
#endif
  #include "Audio/audio_api.h"
  #include "Audio/audio_structs_i.h"
  #include "Audio/audio_var_i.h"
  #include "Audio/audio_messages_i.h"
  #include "rvf/rvf_target.h"
  #include "Audio/audio_const_i.h"
  #include "Audio/audio_error_hdlr_i.h"

  /* include the usefull L1 header */
  #define BOOL_FLAG
  #define CHAR_FLAG
  #include "l1_types.h"
  #include "l1audio_cust.h"
  #include "l1audio_msgty.h"
  #include "l1audio_signa.h"

  #include <string.h>

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_melody_E1_send_status                              */
  /*                                                                              */
  /*    Purpose:  This function sends the melody_E1 status to the entity.         */
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
  void audio_melody_E1_send_status (T_AUDIO_RET status, T_RV_RETURN return_path)
  {
    T_AUDIO_MELODY_E1_STATUS *p_send_message = NULL;
    T_RVF_MB_STATUS mb_status = RVF_RED;

    while (mb_status == RVF_RED)
    {
      /* allocate the message buffer */
      mb_status = rvf_get_buf (p_audio_gbl_var->mb_external,
                               sizeof (T_AUDIO_MELODY_E1_STATUS),
                               (T_RVF_BUFFER **) (&p_send_message));

      /* If insufficient resources, then report a memory error and abort.               */
      /* and wait until more ressource is given */
      if (mb_status == RVF_RED)
      {
        audio_melody_E1_error_trace(AUDIO_ENTITY_NO_MEMORY);
        rvf_delay(RVF_MS_TO_TICKS(1000));
      }
    }
    /*fill the header of the message */
    ((T_AUDIO_MELODY_E1_STATUS *)p_send_message)->os_hdr.msg_id = AUDIO_MELODY_E1_STATUS_MSG;

    /* fill the status parameters */
    ((T_AUDIO_MELODY_E1_STATUS *)p_send_message)->status = status;

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
  /*    Function Name:   audio_melody_E1_message_switch                           */
  /*                                                                              */
  /*    Purpose:  Manage the message supply between the melody manage 1 and       */
  /*              manager 0                                                       */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        start or stop message from the melody E1 features                     */
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
  UINT8 audio_melody_E1_message_switch (T_RV_HDR *p_message)
  {
    switch (p_message->msg_id)
    {
      case AUDIO_MELODY_E1_START_REQ:
      {
        /* Check if the melody E1 number 0 isn't started */
        if (p_audio_gbl_var->melody_E1_0.state == AUDIO_MELODY_E1_IDLE)
        {
          /* Save the mode of the melody */
          p_audio_gbl_var->melody_E1_mode =
            ((T_AUDIO_MELODY_E1_START *)p_message)->melody_E1_parameter.melody_mode;
          return(AUDIO_MELODY_E1_0);
        }
        /* Check if the melody E1 number 1 isn't started and if the current mode is game mode */
        if ( (p_audio_gbl_var->melody_E1_1.state == AUDIO_MELODY_E1_IDLE) &&
             (p_audio_gbl_var->melody_E1_mode == AUDIO_MELODY_GAME_MODE) )
        {
          return(AUDIO_MELODY_E1_1);
        }

        /* This start command is an error  */
        /* error due to a wrong melody mode */
        if ( (p_audio_gbl_var->melody_E1_1.state == AUDIO_MELODY_E1_IDLE) &&
             (p_audio_gbl_var->melody_E1_mode == AUDIO_MELODY_NORMAL_MODE) )
        {
          /* Close the FFS file previously open by the audio start API*/
          #ifndef _WINDOWS
#if(AS_RFS_API == 1)
			if ( rfs_close(((T_AUDIO_MELODY_E1_START *)(p_message))->audio_ffs_fd) != RFS_EOK )
#else
			if ( ffs_close(((T_AUDIO_MELODY_E1_START *)(p_message))->audio_ffs_fd) != EFFS_OK )
#endif
            {
              audio_melody_E1_error_trace(AUDIO_ENTITY_FILE_NO_CLOSE);
            }
          #endif
          audio_melody_E1_send_status (AUDIO_MODE_ERROR, ((T_AUDIO_MELODY_E1_START *)(p_message))->return_path);
          audio_melody_E1_error_trace(AUDIO_ERROR_MODE);
          p_audio_gbl_var->message_processed = TRUE;
          return(AUDIO_MELODY_E1_NONE);
        }

        /* error due to a wrong start */
        /* Close the FFS file previously open by the audio start API*/
        #ifndef _WINDOWS
#if(AS_RFS_API == 1)
		  if ( rfs_close(((T_AUDIO_MELODY_E1_START *)(p_message))->audio_ffs_fd) != RFS_EOK )
#else
		  if ( ffs_close(((T_AUDIO_MELODY_E1_START *)(p_message))->audio_ffs_fd) != EFFS_OK )
#endif
          {
            audio_melody_E1_error_trace(AUDIO_ENTITY_FILE_NO_CLOSE);
          }
        #endif
        audio_melody_E1_send_status (AUDIO_ERROR, ((T_AUDIO_MELODY_E1_START *)(p_message))->return_path);
        audio_melody_E1_error_trace(AUDIO_ERROR_START_EVENT);
        p_audio_gbl_var->message_processed = TRUE;
        return(AUDIO_MELODY_E1_NONE);
      }

      case AUDIO_MELODY_E1_STOP_REQ:
      {
        /* Check if the melody E1 number 0 is running and if the stop command and */
        /* the manager 0 talk about the same melody name */
#if(AS_RFS_API == 1)
		if ( (p_audio_gbl_var->melody_E1_0.state != AUDIO_MELODY_E1_IDLE) &&
             (wstrcmp(p_audio_gbl_var->melody_E1_0.melody_name,
                  ((T_AUDIO_MELODY_E1_STOP*)p_message)->melody_name) == 0) )
#else
		if ( (p_audio_gbl_var->melody_E1_0.state != AUDIO_MELODY_E1_IDLE) &&
             (strcmp(p_audio_gbl_var->melody_E1_0.melody_name,
                  ((T_AUDIO_MELODY_E1_STOP*)p_message)->melody_name) == 0) )
#endif
        {
          return(AUDIO_MELODY_E1_0);
        }
        /* Check if the melody E1 number 1 is running and if the stop command and */
        /* the manager 1 talk about the same melody name */
#if(AS_RFS_API == 1)
		if ( (p_audio_gbl_var->melody_E1_1.state != AUDIO_MELODY_E1_IDLE) &&
             (wstrcmp(p_audio_gbl_var->melody_E1_1.melody_name,
                  ((T_AUDIO_MELODY_E1_STOP*)p_message)->melody_name) == 0) )
#else
		if ( (p_audio_gbl_var->melody_E1_1.state != AUDIO_MELODY_E1_IDLE) &&
             (strcmp(p_audio_gbl_var->melody_E1_1.melody_name,
                  ((T_AUDIO_MELODY_E1_STOP*)p_message)->melody_name) == 0) )
#endif
        {
          return(AUDIO_MELODY_E1_1);
        }

        /* the stop command isn't apply to a melody E1, it's an error */
        /* event error - send an error message*/
        audio_melody_E1_send_status (AUDIO_ERROR, ((T_AUDIO_MELODY_E1_STOP *)(p_message))->return_path);
        audio_melody_E1_error_trace(AUDIO_ERROR_STOP_EVENT);
        p_audio_gbl_var->message_processed = TRUE;
        return(AUDIO_MELODY_E1_NONE);
      }

      case AUDIO_FFS_INIT_DONE:
      {
        /* The FFS message is from the melody E1 manager 0 */
        if ( ((T_AUDIO_FFS_INIT *)p_message)->session_id == AUDIO_FFS_SESSION_MELODY_E1_0 )
        {
          return(AUDIO_MELODY_E1_0);
        }
        /* The FFS message is from the melody E1 manager 1 */
        if ( ((T_AUDIO_FFS_INIT *)p_message)->session_id == AUDIO_FFS_SESSION_MELODY_E1_1 )
        {
          return(AUDIO_MELODY_E1_1);
        }
        return(AUDIO_MELODY_E1_NONE);
      }

      case AUDIO_FFS_STOP_CON:
      {
        /* The FFS message is from the melody E1 manager 0 */
        if ( ((T_AUDIO_FFS_STOP_REQ *)p_message)->session_id == AUDIO_FFS_SESSION_MELODY_E1_0 )
        {
          return(AUDIO_MELODY_E1_0);
        }
        /* The FFS message is from the melody E1 manager 1 */
        if ( ((T_AUDIO_FFS_STOP_REQ *)p_message)->session_id == AUDIO_FFS_SESSION_MELODY_E1_1 )
        {
          return(AUDIO_MELODY_E1_1);
        }
       return(AUDIO_MELODY_E1_NONE);
      }

      case MMI_MELODY0_START_CON:
      case MMI_MELODY0_STOP_CON:
      {
        return(AUDIO_MELODY_E1_0);
      }

      case MMI_MELODY1_START_CON:
      case MMI_MELODY1_STOP_CON:
      {
        return(AUDIO_MELODY_E1_1);
      }

      default:
      {
        return(AUDIO_MELODY_E1_NONE);
      }

    } /* switch */

    // omaps00090550 return(AUDIO_MELODY_E1_NONE);
	
  }
  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_melody_convert_parameter                           */
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
  void audio_melody_e1_convert_parameter(UINT8 channel_id, T_MMI_MELODY_REQ *l1_parameter, UINT8 melody_id)
  {
    if (melody_id == 0)
    {
      if (p_audio_gbl_var->melody_E1_mode == AUDIO_MELODY_GAME_MODE)
      {
        /* oscillators 0, 1, 2, 3 are used for the melody 0 in game mode */
        l1_parameter->oscillator_used_bitmap = 0x0F;
      }
      else
      {
        /* oscillators 0, 1, 2, 3, 4, 5, 6, 7 are used for the melody 0 in normal mode */
        l1_parameter->oscillator_used_bitmap = 0xFF;
      }

      /* loopback mode */
      l1_parameter->loopback =
        p_audio_gbl_var->melody_E1_0.loopback;
    }
    else
    {
      /* oscillators 4, 5, 6, 7 are used for the melody 0 */
      l1_parameter->oscillator_used_bitmap = 0xF0;

      /* loopback mode */
      l1_parameter->loopback =
        p_audio_gbl_var->melody_E1_0.loopback;

    }
    l1_parameter->session_id = channel_id;
  }

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_melody_e1_manager_0                                */
  /*                                                                              */
  /*    Purpose:  This function is called to manage a melody E1 format number 0.  */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        Message to the audo entity                                            */
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
  void audio_melody_E1_manager_0 (T_RV_HDR *p_message)
  {
    /* Declare local variables.                                                   */
    UINT8 i;
    T_RVF_MB_STATUS mb_status;

    /**************** audio_melody_e1_manager function begins *********************/

    switch(p_audio_gbl_var->melody_E1_0.state)
    {
      case AUDIO_MELODY_E1_IDLE:
        {
          switch(p_message->msg_id)
          {
             case AUDIO_MELODY_E1_START_REQ:
              {
                T_AUDIO_FFS_FLASH_2_RAM_START *p_send_message;

                /* save the task id of the entity */
                p_audio_gbl_var->melody_E1_0.task_id = p_message->src_addr_id;

                /* save the return path */
                p_audio_gbl_var->melody_E1_0.return_path.callback_func =
                  ((T_AUDIO_MELODY_E1_START*)p_message)->return_path.callback_func;
                p_audio_gbl_var->melody_E1_0.return_path.addr_id   =
                  ((T_AUDIO_MELODY_E1_START*)p_message)->return_path.addr_id;

                for (i=0; i<AUDIO_PATH_NAME_MAX_SIZE; i++)
                {
                  p_audio_gbl_var->melody_E1_0.melody_name[i] =
                    ((T_AUDIO_MELODY_E1_START*)(p_message))->melody_E1_parameter.melody_name[i];
                }

                p_audio_gbl_var->melody_E1_0.loopback =
                  ((T_AUDIO_MELODY_E1_START*)(p_message))->melody_E1_parameter.loopback;

                /* Send the Start message to the FFS */
                /* allocate the message buffer */
                mb_status = rvf_get_buf (p_audio_gbl_var->mb_internal,
                                         sizeof (T_AUDIO_FFS_FLASH_2_RAM_START),
                                         (T_RVF_BUFFER **) (&p_send_message));

                /* If insufficient resources, then report a memory error and abort.               */
                if (mb_status == RVF_RED)
                {
                  audio_melody_E1_error_trace(AUDIO_ENTITY_NO_MEMORY);
                }

                /* fill the header of the message */
                ((T_AUDIO_FFS_FLASH_2_RAM_START *)p_send_message)->os_hdr.msg_id = AUDIO_FFS_FLASH_2_RAM_START_REQ;

                /* fill the parameters */
                ((T_AUDIO_FFS_FLASH_2_RAM_START *)p_send_message)->audio_ffs_fd =
                  ((T_AUDIO_MELODY_E1_START*)(p_message))->audio_ffs_fd;
                ((T_AUDIO_FFS_FLASH_2_RAM_START *)p_send_message)->initial_size = AUDIO_MELODY_E1_SIZE;
                ((T_AUDIO_FFS_FLASH_2_RAM_START *)p_send_message)->loop =
                  ((T_AUDIO_MELODY_E1_START*)p_message)->melody_E1_parameter.loopback;

                ((T_AUDIO_FFS_FLASH_2_RAM_START *)(p_send_message))->session_id = AUDIO_FFS_SESSION_MELODY_E1_0;

                /* send the message to the entity */
                rvf_send_msg (p_audio_gbl_var->audio_ffs_addrId,
                                p_send_message);

                /* change to the state AUDIO_MELODY_E1_WAIT_INIT_FFS */
                p_audio_gbl_var->melody_E1_0.state = AUDIO_MELODY_E1_WAIT_INIT_FFS;
                break;
               }
          }
        break;
        } /* case AUDIO_MELODY_E1_IDLE */

      case AUDIO_MELODY_E1_WAIT_INIT_FFS:
        {
          switch(p_message->msg_id)
          {
             case AUDIO_FFS_INIT_DONE:
              {
                DummyStruct *p_send_message;

                /* send the start melody message to the L1 */
                /* allocate the buffer for the message to the L1 */
                p_send_message = audio_allocate_l1_message(sizeof(T_MMI_MELODY_REQ));

                if ( p_send_message != NULL)
                {
                  /* Convert the entity parameters to the audio L1 parameters */
                  audio_melody_e1_convert_parameter(((T_AUDIO_FFS_INIT *)p_message)->channel_id,
                                                    (T_MMI_MELODY_REQ *)p_send_message,
                                                    AUDIO_MELODY_E1_0);

                  /* send the start command to the audio L1 */
                  audio_send_l1_message(MMI_MELODY0_START_REQ, p_send_message);
                }

                /* change to the state AUDIO_MELODY_E1_WAIT_START_CONFIRMATION */
                p_audio_gbl_var->melody_E1_0.state = AUDIO_MELODY_E1_WAIT_START_CON;
                break;
             }
             case AUDIO_MELODY_E1_STOP_REQ:
              {
                /*.Before stopping this task, control that            */
                /* stop task id caller =Melody task id                */
                   if ( p_audio_gbl_var->melody_E1_0.task_id == p_message->src_addr_id)
                   {
                      /* change to the state AUDIO_MELODY_E1_WAIT_INIT_DONE */
                      p_audio_gbl_var->melody_E1_0.state = AUDIO_MELODY_E1_WAIT_INIT_DONE;
                   }
                   else
                   {
                     /* A stop request from an other task is sent during a start connection */
                     /* event error - send an error message  */
                     audio_melody_E1_send_status (AUDIO_ERROR,((T_AUDIO_MELODY_E1_STOP *)(p_message))->return_path);
                     audio_melody_E1_error_trace(AUDIO_ERROR_STOP_EVENT);
                   }
                break;
              } /* case AUDIO_MELODY_E1_STOP_REQ */

             case AUDIO_MELODY_E1_START_REQ:
              {
                  /* event error - send an error message*/
                  /* Close the FFS file previously open by the audio start API*/
                  #ifndef _WINDOWS
#if(AS_RFS_API == 1)
					if ( rfs_close(((T_AUDIO_MELODY_E1_START *)(p_message))->audio_ffs_fd) != RFS_EOK )
#else
					if ( ffs_close(((T_AUDIO_MELODY_E1_START *)(p_message))->audio_ffs_fd) != EFFS_OK )
#endif
                    {
                      audio_melody_E1_error_trace(AUDIO_ENTITY_FILE_NO_CLOSE);
                    }
                  #endif
                  audio_melody_E1_send_status (AUDIO_ERROR, ((T_AUDIO_MELODY_E1_START *)(p_message))->return_path);
                  audio_melody_E1_error_trace(AUDIO_ERROR_START_EVENT);
                break;
              }
          }
        break;
        } /* case AUDIO_MELODY_E1_WAIT_INIT_FFS */

      case AUDIO_MELODY_E1_WAIT_INIT_DONE:
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
                  audio_melody_E1_error_trace(AUDIO_ENTITY_NO_MEMORY);
                }

                /* fill the header of the message */
                ((T_AUDIO_FFS_STOP_REQ *)p_send_message)->os_hdr.msg_id = AUDIO_FFS_STOP_REQ;

                /* fill the parameter */
                ((T_AUDIO_FFS_STOP_REQ *)p_send_message)->session_id = AUDIO_FFS_SESSION_MELODY_E1_0;

                /* send the message to the entity */
                rvf_send_msg (p_audio_gbl_var->audio_ffs_addrId,
                                p_send_message);

                /* change to the state AUDIO_MELODY_E1_WAIT_STOP_FFS */
                p_audio_gbl_var->melody_E1_0.state = AUDIO_MELODY_E1_WAIT_STOP_FFS;
              break;
              }
             case AUDIO_MELODY_E1_STOP_REQ:
              {
                /* stop event error - send an error message  */
                audio_melody_E1_send_status (AUDIO_ERROR,((T_AUDIO_MELODY_E1_STOP *)(p_message))->return_path);
                audio_melody_E1_error_trace(AUDIO_ERROR_STOP_EVENT);
                break;
              } /* case AUDIO_MELODY_E1_STOP_REQ */

              case AUDIO_MELODY_E1_START_REQ:
              {
                  /* event error - send an error message*/
                  /* Close the FFS file previously open by the audio start API*/
                  #ifndef _WINDOWS
#if(AS_RFS_API == 1)
					if ( rfs_close(((T_AUDIO_MELODY_E1_START *)(p_message))->audio_ffs_fd) != RFS_EOK )
#else
					if ( ffs_close(((T_AUDIO_MELODY_E1_START *)(p_message))->audio_ffs_fd) != EFFS_OK )
#endif
                    {
                      audio_melody_E1_error_trace(AUDIO_ENTITY_FILE_NO_CLOSE);
                    }
                  #endif
                  audio_melody_E1_send_status (AUDIO_ERROR, ((T_AUDIO_MELODY_E1_START *)(p_message))->return_path);
                  audio_melody_E1_error_trace(AUDIO_ERROR_START_EVENT);
                break;
              }
           }
        break;
        } /* case AUDIO_MELODY_E1_WAIT_INIT_DONE*/

      case AUDIO_MELODY_E1_WAIT_START_CON:
        {
          switch(p_message->msg_id)
          {
             case MMI_MELODY0_START_CON:
              {
                /* change to the state AUDIO_MELODY_E1_WAIT_STOP_COMMAND */
                p_audio_gbl_var->melody_E1_0.state = AUDIO_MELODY_E1_WAIT_STOP_COMMAND;
                break;
              }

             case AUDIO_MELODY_E1_STOP_REQ:
              {
                /*.Before stopping this task, control that            */
                /* stop task id caller =Melody task id                */
                   if ( p_audio_gbl_var->melody_E1_0.task_id == p_message->src_addr_id)
                   {
                      /* change to the state AUDIO_MELODY_E1_WAIT_STOP_CONFIRMATION */
                      p_audio_gbl_var->melody_E1_0.state = AUDIO_MELODY_E1_WAIT_START_CON_TO_STOP;
                   }
                   else
                   {
                     /* A stop request from an other task is sent during a start connection */
                     /* event error - send an error message  */
                     audio_melody_E1_send_status (AUDIO_ERROR,((T_AUDIO_MELODY_E1_STOP *)(p_message))->return_path);
                     audio_melody_E1_error_trace(AUDIO_ERROR_STOP_EVENT);
                   }
                break;
              } /* case AUDIO_MELODY_E1_STOP_REQ */

             case AUDIO_MELODY_E1_START_REQ:
              {
                  /* event error - send an error message*/
                  /* Close the FFS file previously open by the audio start API*/
                  #ifndef _WINDOWS
#if(AS_RFS_API == 1)
					if ( rfs_close(((T_AUDIO_MELODY_E1_START *)(p_message))->audio_ffs_fd) != RFS_EOK )
#else
					if ( ffs_close(((T_AUDIO_MELODY_E1_START *)(p_message))->audio_ffs_fd) != EFFS_OK )
#endif
                    {
                      audio_melody_E1_error_trace(AUDIO_ENTITY_FILE_NO_CLOSE);
                    }
                  #endif
                  audio_melody_E1_send_status (AUDIO_ERROR, ((T_AUDIO_MELODY_E1_START *)(p_message))->return_path);
                  audio_melody_E1_error_trace(AUDIO_ERROR_START_EVENT);
                break;
              }
          }
        break;
        } /* case AUDIO_MELODY_E1_PLAY_WAIT_START_CON */

       case AUDIO_MELODY_E1_WAIT_START_CON_TO_STOP:
        {
          switch(p_message->msg_id)
          {
             case MMI_MELODY0_START_CON:
              {
                DummyStruct *p_send_message;

                /* send the stop command to the audio L1 */
                /* allocate the buffer for the message to the L1 */
                p_send_message = audio_allocate_l1_message(0);
                if ( p_send_message != NULL)
                {
                  /* send the start command to the audio L1 */
                  audio_send_l1_message(MMI_MELODY0_STOP_REQ, p_send_message);
                }

                /* change to the state AUDIO_MELODY_E1_WAIT_STOP_CON */
                p_audio_gbl_var->melody_E1_0.state = AUDIO_MELODY_E1_WAIT_STOP_CON;
               break;
              }

             case AUDIO_MELODY_E1_STOP_REQ:
              {
                 /* stop event error - send an error message  */
                 audio_melody_E1_send_status (AUDIO_ERROR,((T_AUDIO_MELODY_E1_STOP *)(p_message))->return_path);
                 audio_melody_E1_error_trace(AUDIO_ERROR_STOP_EVENT);
                 break;
              } /* case AUDIO_MELODY_E1_STOP_REQ */

             case AUDIO_MELODY_E1_START_REQ:
              {
                  /* event error - send an error message*/
                  /* Close the FFS file previously open by the audio start API*/
                  #ifndef _WINDOWS
#if(AS_RFS_API == 1)
					if ( rfs_close(((T_AUDIO_MELODY_E1_START *)(p_message))->audio_ffs_fd) != RFS_EOK )
#else
					if ( ffs_close(((T_AUDIO_MELODY_E1_START *)(p_message))->audio_ffs_fd) != EFFS_OK )
#endif
                    {
                      audio_melody_E1_error_trace(AUDIO_ENTITY_FILE_NO_CLOSE);
                    }
                  #endif
                  audio_melody_E1_send_status (AUDIO_ERROR, ((T_AUDIO_MELODY_E1_START *)(p_message))->return_path);
                  audio_melody_E1_error_trace(AUDIO_ERROR_START_EVENT);
                break;
              }
          }
        break;
        }

       case AUDIO_MELODY_E1_WAIT_STOP_COMMAND:
        {
          switch(p_message->msg_id)
          {
             case MMI_MELODY0_STOP_CON:
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
                  audio_melody_E1_error_trace(AUDIO_ENTITY_NO_MEMORY);
                }

                /* fill the header of the message */
                ((T_AUDIO_FFS_STOP_REQ *)p_send_message)->os_hdr.msg_id = AUDIO_FFS_STOP_REQ;

                /* fill the parameter */
                ((T_AUDIO_FFS_STOP_REQ *)p_send_message)->session_id = AUDIO_FFS_SESSION_MELODY_E1_0;

                /* send the message to the entity */
                rvf_send_msg (p_audio_gbl_var->audio_ffs_addrId,
                                p_send_message);

                /* change to the state AUDIO_MELODY_E1_WAIT_STOP_FFS */
                p_audio_gbl_var->melody_E1_0.state = AUDIO_MELODY_E1_WAIT_STOP_FFS;
               break;
              }

             case AUDIO_MELODY_E1_STOP_REQ:
              {
                /*.Before stopping this task, control that            */
                /* stop task id caller =Melody task id                */
                if ( p_audio_gbl_var->melody_E1_0.task_id == p_message->src_addr_id)
                {
                  DummyStruct *p_send_message;

                  /* send the stop command to the audio L1 */
                  /* allocate the buffer for the message to the L1 */
                  p_send_message = audio_allocate_l1_message(0);
                  if (p_send_message != NULL)
                  {
                    /* send the start command to the audio L1 */
                    audio_send_l1_message(MMI_MELODY0_STOP_REQ, p_send_message);
                  }

                  /* change to the state AUDIO_MELODY_E1_WAIT_STOP_CONFIRMATION */
                  p_audio_gbl_var->melody_E1_0.state = AUDIO_MELODY_E1_WAIT_STOP_CON;
                }
                else
                {
                 /* A stop request from an other task is sent during a start connection */
                 /* event error - send an error message  */
                 audio_melody_E1_send_status (AUDIO_ERROR,((T_AUDIO_MELODY_E1_STOP *)(p_message))->return_path);
                 audio_melody_E1_error_trace(AUDIO_ERROR_STOP_EVENT);
                }
                break;
              } /* case AUDIO_MELODY_E1_STOP_REQ */

             case AUDIO_MELODY_E1_START_REQ:
              {
                  /* event error - send an error message*/
                  /* Close the FFS file previously open by the audio start API*/
                  #ifndef _WINDOWS
#if(AS_RFS_API == 1)
					if ( rfs_close(((T_AUDIO_MELODY_E1_START *)(p_message))->audio_ffs_fd) != RFS_EOK )
#else
					if ( ffs_close(((T_AUDIO_MELODY_E1_START *)(p_message))->audio_ffs_fd) != EFFS_OK )
#endif
                    {
                      audio_melody_E1_error_trace(AUDIO_ENTITY_FILE_NO_CLOSE);
                    }
                  #endif
                  audio_melody_E1_send_status (AUDIO_ERROR, ((T_AUDIO_MELODY_E1_START *)(p_message))->return_path);
                  audio_melody_E1_error_trace(AUDIO_ERROR_START_EVENT);
                break;
              }
          }
        break;
        } /* case AUDIO_MELODY_E1_WAIT_STOP_COMMAND */

        case AUDIO_MELODY_E1_WAIT_STOP_CON:
        {
          switch(p_message->msg_id)
          {
             case MMI_MELODY0_STOP_CON:
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
                  audio_melody_E1_error_trace(AUDIO_ENTITY_NO_MEMORY);
                }

                /* fill the header of the message */
                ((T_AUDIO_FFS_STOP_REQ *)p_send_message)->os_hdr.msg_id = AUDIO_FFS_STOP_REQ;

                /* fill the parameter */
                ((T_AUDIO_FFS_STOP_REQ *)p_send_message)->session_id = AUDIO_FFS_SESSION_MELODY_E1_0;

                /* send the message to the entity */
                rvf_send_msg (p_audio_gbl_var->audio_ffs_addrId,
                              p_send_message);

                /* change to the state AUDIO_MELODY_0_WAIT_STOP_FFS */
                 p_audio_gbl_var->melody_E1_0.state = AUDIO_MELODY_E1_WAIT_STOP_FFS;
               break;
              }

             case AUDIO_MELODY_E1_STOP_REQ:
              {
                /* stop event error - send an error message  */
                audio_melody_E1_send_status (AUDIO_ERROR,((T_AUDIO_MELODY_E1_STOP *)(p_message))->return_path);
                audio_melody_E1_error_trace(AUDIO_ERROR_STOP_EVENT);
               break;
              } /* case AUDIO_MELODY_E1_STOP_REQ */

             case AUDIO_MELODY_E1_START_REQ:
              {
                  /* event error - send an error message*/
                  /* Close the FFS file previously open by the audio start API*/
                  #ifndef _WINDOWS
#if(AS_RFS_API == 1)
					if ( rfs_close(((T_AUDIO_MELODY_E1_START *)(p_message))->audio_ffs_fd) != RFS_EOK )
#else
					if ( ffs_close(((T_AUDIO_MELODY_E1_START *)(p_message))->audio_ffs_fd) != EFFS_OK )
#endif
                    {
                      audio_melody_E1_error_trace(AUDIO_ENTITY_FILE_NO_CLOSE);
                    }
                  #endif
                  audio_melody_E1_send_status (AUDIO_ERROR, ((T_AUDIO_MELODY_E1_START *)(p_message))->return_path);
                  audio_melody_E1_error_trace(AUDIO_ERROR_START_EVENT);
                break;
              }
          }
        break;
        } /* case AUDIO_MELODY_E1_WAIT_STOP_CON */

        case AUDIO_MELODY_E1_WAIT_STOP_FFS:
        {
          switch(p_message->msg_id)
          {
             case AUDIO_FFS_STOP_CON:
              {
                audio_melody_E1_send_status (AUDIO_OK, p_audio_gbl_var->melody_E1_0.return_path);

                /* change to the state AUDIO_MELODY_0_IDLE */
                p_audio_gbl_var->melody_E1_0.state = AUDIO_MELODY_E1_IDLE;

                break;
              }

              case AUDIO_MELODY_E1_STOP_REQ:
              {
                /* stop event error - send an error message  */
                audio_melody_E1_send_status (AUDIO_ERROR,((T_AUDIO_MELODY_E1_STOP *)(p_message))->return_path);
                audio_melody_E1_error_trace(AUDIO_ERROR_STOP_EVENT);
               break;
              } /* case AUDIO_MELODY_E1_STOP_REQ */

             case AUDIO_MELODY_E1_START_REQ:
              {
                  /* event error - send an error message*/
                  /* Close the FFS file previously open by the audio start API*/
                  #ifndef _WINDOWS
#if(AS_RFS_API == 1)
					if ( rfs_close(((T_AUDIO_MELODY_E1_START *)(p_message))->audio_ffs_fd) != RFS_EOK )
#else
					if ( ffs_close(((T_AUDIO_MELODY_E1_START *)(p_message))->audio_ffs_fd) != EFFS_OK )
#endif
                    {
                      audio_melody_E1_error_trace(AUDIO_ENTITY_FILE_NO_CLOSE);
                    }
                  #endif
                  audio_melody_E1_send_status (AUDIO_ERROR, ((T_AUDIO_MELODY_E1_START *)(p_message))->return_path);
                  audio_melody_E1_error_trace(AUDIO_ERROR_START_EVENT);
                break;
              }

          }
        break;
        } /* case AUDIO_MELODY_E1_WAIT_STOP_CON */
    } /* switch(p_audio_gbl_var->melody_E1_0.state) */
  } /*********************** End of audio_melody_E1_manager_0 function **********************/

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_melody_e1_manager_1                                */
  /*                                                                              */
  /*    Purpose:  This function is called to manage a melody E1 format number 1.  */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        Message to the audo entity                                            */
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
  void audio_melody_E1_manager_1 (T_RV_HDR *p_message)
  {
    /* Declare local variables.                                                   */
    UINT8 i;
    T_RVF_MB_STATUS mb_status;

    /**************** audio_melody_e1_manager function begins *********************/

    switch(p_audio_gbl_var->melody_E1_1.state)
    {
      case AUDIO_MELODY_E1_IDLE:
        {
          switch(p_message->msg_id)
          {
             case AUDIO_MELODY_E1_START_REQ:
              {
                T_AUDIO_FFS_FLASH_2_RAM_START *p_send_message;

                /* save the task id of the entity */
                p_audio_gbl_var->melody_E1_1.task_id = p_message->src_addr_id;

                /* save the return path */
                p_audio_gbl_var->melody_E1_1.return_path.callback_func =
                  ((T_AUDIO_MELODY_E1_START*)(p_message))->return_path.callback_func;
                p_audio_gbl_var->melody_E1_1.return_path.addr_id   =
                  ((T_AUDIO_MELODY_E1_START*)(p_message))->return_path.addr_id;

                for (i=0; i<AUDIO_PATH_NAME_MAX_SIZE; i++)
                {
                  p_audio_gbl_var->melody_E1_1.melody_name[i] =
                    ((T_AUDIO_MELODY_E1_START*)(p_message))->melody_E1_parameter.melody_name[i];
                }

                p_audio_gbl_var->melody_E1_1.loopback =
                  ((T_AUDIO_MELODY_E1_START*)(p_message))->melody_E1_parameter.loopback;

                /* Send the Start message to the FFS */
                /* allocate the message buffer */
                mb_status = rvf_get_buf (p_audio_gbl_var->mb_internal,
                                         sizeof (T_AUDIO_FFS_FLASH_2_RAM_START),
                                         (T_RVF_BUFFER **) (&p_send_message));

                /* If insufficient resources, then report a memory error and abort.               */
                if (mb_status == RVF_RED)
                {
                  audio_melody_E1_error_trace(AUDIO_ENTITY_NO_MEMORY);
                }

                /* fill the header of the message */
                ((T_AUDIO_FFS_FLASH_2_RAM_START *)p_send_message)->os_hdr.msg_id = AUDIO_FFS_FLASH_2_RAM_START_REQ;

                /* fill the parameters */
                ((T_AUDIO_FFS_FLASH_2_RAM_START *)p_send_message)->audio_ffs_fd =
                  ((T_AUDIO_MELODY_E1_START*)p_message)->audio_ffs_fd;
                ((T_AUDIO_FFS_FLASH_2_RAM_START *)p_send_message)->initial_size = AUDIO_MELODY_E1_SIZE;
                ((T_AUDIO_FFS_FLASH_2_RAM_START *)p_send_message)->loop =
                  ((T_AUDIO_MELODY_E1_START*)p_message)->melody_E1_parameter.loopback;

                ((T_AUDIO_FFS_FLASH_2_RAM_START *)p_send_message)->session_id = AUDIO_FFS_SESSION_MELODY_E1_1;

                /* send the message to the entity */
                rvf_send_msg (p_audio_gbl_var->audio_ffs_addrId,
                                p_send_message);

                /* change to the state AUDIO_MELODY_E1_WAIT_INIT_FFS */
                p_audio_gbl_var->melody_E1_1.state = AUDIO_MELODY_E1_WAIT_INIT_FFS;
                break;
               }
          }
        break;
        } /* case AUDIO_MELODY_E1_IDLE */

      case AUDIO_MELODY_E1_WAIT_INIT_FFS:
        {
          switch(p_message->msg_id)
          {
             case AUDIO_FFS_INIT_DONE:
              {
                DummyStruct *p_send_message;

                /* send the start melody message to the L1 */
                /* allocate the buffer for the message to the L1 */
                p_send_message = audio_allocate_l1_message(sizeof(T_MMI_MELODY_REQ));

                if ( p_send_message != NULL)
                {
                  /* Convert the entity parameters to the audio L1 parameters */
                  audio_melody_e1_convert_parameter(((T_AUDIO_FFS_INIT *)p_message)->channel_id,
                                                    (T_MMI_MELODY_REQ *)p_send_message,
                                                    AUDIO_MELODY_E1_1);

                  /* send the start command to the audio L1 */
                  audio_send_l1_message(MMI_MELODY1_START_REQ, p_send_message);
                }

                /* change to the state AUDIO_MELODY_E1_WAIT_START_CONFIRMATION */
                p_audio_gbl_var->melody_E1_1.state = AUDIO_MELODY_E1_WAIT_START_CON;
                break;
             }

             case AUDIO_MELODY_E1_STOP_REQ:
              {
                /*.Before stopping this task, control that            */
                /* stop task id caller =Melody task id                */
                   if ( p_audio_gbl_var->melody_E1_1.task_id == p_message->src_addr_id)
                   {
                      /* change to the state AUDIO_MELODY_E1_WAIT_INIT_DONE */
                      p_audio_gbl_var->melody_E1_1.state = AUDIO_MELODY_E1_WAIT_INIT_DONE;
                   }
                   else
                   {
                     /* A stop request from an other task is sent during a start connection */
                     /* event error - send an error message  */
                     audio_melody_E1_send_status (AUDIO_ERROR,((T_AUDIO_MELODY_E1_STOP *)(p_message))->return_path);
                     audio_melody_E1_error_trace(AUDIO_ERROR_STOP_EVENT);
                   }
                break;
              } /* case AUDIO_MELODY_E1_STOP_REQ */

             case AUDIO_MELODY_E1_START_REQ:
              {
                  /* event error - send an error message*/
                  /* Close the FFS file previously open by the audio start API*/
                  #ifndef _WINDOWS
#if(AS_RFS_API == 1)
					if ( rfs_close(((T_AUDIO_MELODY_E1_START *)(p_message))->audio_ffs_fd) != RFS_EOK )
#else
					if ( ffs_close(((T_AUDIO_MELODY_E1_START *)(p_message))->audio_ffs_fd) != EFFS_OK )
#endif
                    {
                      audio_melody_E1_error_trace(AUDIO_ENTITY_FILE_NO_CLOSE);
                    }
                  #endif
                  audio_melody_E1_send_status (AUDIO_ERROR, ((T_AUDIO_MELODY_E1_START *)(p_message))->return_path);
                  audio_melody_E1_error_trace(AUDIO_ERROR_START_EVENT);
                break;
              }
          }
        break;
        } /* case AUDIO_MELODY_E1_WAIT_INIT_FFS */

      case AUDIO_MELODY_E1_WAIT_INIT_DONE:
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
                  audio_melody_E1_error_trace(AUDIO_ENTITY_NO_MEMORY);
                }

                /* fill the header of the message */
                ((T_AUDIO_FFS_STOP_REQ *)p_send_message)->os_hdr.msg_id = AUDIO_FFS_STOP_REQ;

                /* fill the parameter */
                ((T_AUDIO_FFS_STOP_REQ *)p_send_message)->session_id = AUDIO_FFS_SESSION_MELODY_E1_1;

                /* send the message to the entity */
                rvf_send_msg (p_audio_gbl_var->audio_ffs_addrId,
                                p_send_message);

                /* change to the state AUDIO_MELODY_E1_WAIT_STOP_FFS */
                p_audio_gbl_var->melody_E1_1.state = AUDIO_MELODY_E1_WAIT_STOP_FFS;
              break;
              }

              case AUDIO_MELODY_E1_STOP_REQ:
              {
                /* stop event error - send an error message  */
                audio_melody_E1_send_status (AUDIO_ERROR,((T_AUDIO_MELODY_E1_STOP *)(p_message))->return_path);
                audio_melody_E1_error_trace(AUDIO_ERROR_STOP_EVENT);
                break;
              } /* case AUDIO_MELODY_E1_STOP_REQ */

             case AUDIO_MELODY_E1_START_REQ:
              {
              /* event error - send an error message*/
              /* Close the FFS file previously open by the audio start API*/
              #ifndef _WINDOWS
#if(AS_RFS_API == 1)
				if ( rfs_close(((T_AUDIO_MELODY_E1_START *)p_message)->audio_ffs_fd) != RFS_EOK )
#else
				if ( ffs_close(((T_AUDIO_MELODY_E1_START *)p_message)->audio_ffs_fd) != EFFS_OK )
#endif
                {
                  audio_melody_E1_error_trace(AUDIO_ENTITY_FILE_NO_CLOSE);
                }
              #endif
              audio_melody_E1_send_status (AUDIO_ERROR, ((T_AUDIO_MELODY_E1_START *)p_message)->return_path);
              audio_melody_E1_error_trace(AUDIO_ERROR_START_EVENT);
              break;
              }
          }
        break;
        } /* case AUDIO_MELODY_E1_WAIT_INIT_DONE*/

      case AUDIO_MELODY_E1_WAIT_START_CON:
        {
          switch(p_message->msg_id)
          {
             case MMI_MELODY1_START_CON:
              {
                /* change to the state AUDIO_MELODY_E1_WAIT_STOP_COMMAND */
                p_audio_gbl_var->melody_E1_1.state = AUDIO_MELODY_E1_WAIT_STOP_COMMAND;
                break;
              }

             case AUDIO_MELODY_E1_STOP_REQ:
              {
                /*.Before stopping this task, control that            */
                /* stop task id caller =Melody task id                */
                   if ( p_audio_gbl_var->melody_E1_1.task_id == p_message->src_addr_id)
                   {
                      /* change to the state AUDIO_MELODY_E1_WAIT_STOP_CONFIRMATION */
                      p_audio_gbl_var->melody_E1_1.state = AUDIO_MELODY_E1_WAIT_START_CON_TO_STOP;
                   }
                   else
                   {
                     /* A stop request from an other task is sent during a start connection */
                     /* event error - send an error message  */
                     audio_melody_E1_send_status (AUDIO_ERROR,((T_AUDIO_MELODY_E1_STOP *)p_message)->return_path);
                     audio_melody_E1_error_trace(AUDIO_ERROR_STOP_EVENT);
                   }
                break;
              } /* case AUDIO_MELODY_E1_STOP_REQ */

             case AUDIO_MELODY_E1_START_REQ:
              {
                  /* event error - send an error message*/
                  /* Close the FFS file previously open by the audio start API*/
                  #ifndef _WINDOWS
#if(AS_RFS_API == 1)
					if ( rfs_close(((T_AUDIO_MELODY_E1_START *)p_message)->audio_ffs_fd) != RFS_EOK )
#else
					if ( ffs_close(((T_AUDIO_MELODY_E1_START *)p_message)->audio_ffs_fd) != EFFS_OK )
#endif
                    {
                      audio_melody_E1_error_trace(AUDIO_ENTITY_FILE_NO_CLOSE);
                    }
                  #endif
                  audio_melody_E1_send_status (AUDIO_ERROR, ((T_AUDIO_MELODY_E1_START *)p_message)->return_path);
                  audio_melody_E1_error_trace(AUDIO_ERROR_START_EVENT);
                break;
              }
        }
        break;
        } /* case AUDIO_WAIT_START_CON */

       case AUDIO_MELODY_E1_WAIT_START_CON_TO_STOP:
        {
          switch(p_message->msg_id)
          {
             case MMI_MELODY1_START_CON:
              {
                DummyStruct *p_send_message;

                /* send the stop command to the audio L1 */
                /* allocate the buffer for the message to the L1 */
                p_send_message = audio_allocate_l1_message(0);
                if ( p_send_message != NULL)
                {
                  /* send the start command to the audio L1 */
                  audio_send_l1_message(MMI_MELODY1_STOP_REQ, p_send_message);
                }

                /* change to the state AUDIO_MELODY_E1_WAIT_STOP_CON */
                p_audio_gbl_var->melody_E1_1.state = AUDIO_MELODY_E1_WAIT_STOP_CON;
               break;
              }

             case AUDIO_MELODY_E1_STOP_REQ:
              {
                /* stop event error - send an error message  */
                audio_melody_E1_send_status (AUDIO_ERROR,((T_AUDIO_MELODY_E1_STOP *)(p_message))->return_path);
                audio_melody_E1_error_trace(AUDIO_ERROR_STOP_EVENT);
               break;
              } /* case AUDIO_MELODY_E1_STOP_REQ */

             case AUDIO_MELODY_E1_START_REQ:
              {
                /* start event error - send an error message*/
                /* Close the FFS file previously open by the audio start API*/
                #ifndef _WINDOWS
#if(AS_RFS_API == 1)
				  if ( rfs_close(((T_AUDIO_MELODY_E1_START *)(p_message))->audio_ffs_fd) != RFS_EOK )
#else
				  if ( ffs_close(((T_AUDIO_MELODY_E1_START *)(p_message))->audio_ffs_fd) != EFFS_OK )
#endif
                  {
                    audio_melody_E1_error_trace(AUDIO_ENTITY_FILE_NO_CLOSE);
                  }
                #endif
                audio_melody_E1_send_status (AUDIO_ERROR, ((T_AUDIO_MELODY_E1_START *)(p_message))->return_path);
                audio_melody_E1_error_trace(AUDIO_ERROR_START_EVENT);
               break;
              }
          }
        break;
        }

       case AUDIO_MELODY_E1_WAIT_STOP_COMMAND:
        {
          switch(p_message->msg_id)
          {
             case MMI_MELODY1_STOP_CON:
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
                  audio_melody_E1_error_trace(AUDIO_ENTITY_NO_MEMORY);
                }

                /* fill the header of the message */
                ((T_AUDIO_FFS_STOP_REQ *)p_send_message)->os_hdr.msg_id = AUDIO_FFS_STOP_REQ;

                /* fill the parameter */
                ((T_AUDIO_FFS_STOP_REQ *)p_send_message)->session_id = AUDIO_FFS_SESSION_MELODY_E1_1;

                /* send the message to the entity */
                rvf_send_msg (p_audio_gbl_var->audio_ffs_addrId,
                                p_send_message);

                /* change to the state AUDIO_MELODY_E1_WAIT_STOP_FFS */
                p_audio_gbl_var->melody_E1_1.state = AUDIO_MELODY_E1_WAIT_STOP_FFS;
               break;
              }

             case AUDIO_MELODY_E1_STOP_REQ:
              {
                DummyStruct *p_send_message;

                /*.Before stopping this task, control that            */
                /* stop task id caller =Melody task id                */
                  if ( p_audio_gbl_var->melody_E1_1.task_id == p_message->src_addr_id)
                  {
                    /* send the stop command to the audio L1 */
                    /* allocate the buffer for the message to the L1 */
                    p_send_message = audio_allocate_l1_message(0);
                    if (p_send_message != NULL)
                    {
                      /* send the start command to the audio L1 */
                      audio_send_l1_message(MMI_MELODY1_STOP_REQ, p_send_message);
                    }

                    /* change to the state AUDIO_MELODY_E1_WAIT_STOP_CONFIRMATION */
                    p_audio_gbl_var->melody_E1_1.state = AUDIO_MELODY_E1_WAIT_STOP_CON;
                  }
                  else
                  {
                    /* A stop request from an other task is sent during a start connection */
                    /* event error - send an error message  */
                    audio_melody_E1_send_status (AUDIO_ERROR,((T_AUDIO_MELODY_E1_STOP *)p_message)->return_path);
                    audio_melody_E1_error_trace(AUDIO_ERROR_STOP_EVENT);
                  }
                break;
              } /* case AUDIO_MELODY_E1_STOP_REQ */

             case AUDIO_MELODY_E1_START_REQ:
              {
                  /* event error - send an error message*/
                  /* Close the FFS file previously open by the audio start API*/
                  #ifndef _WINDOWS
#if(AS_RFS_API == 1)
					if ( rfs_close(((T_AUDIO_MELODY_E1_START *)p_message)->audio_ffs_fd) != RFS_EOK )
#else
					if ( ffs_close(((T_AUDIO_MELODY_E1_START *)p_message)->audio_ffs_fd) != EFFS_OK )
#endif
                    {
                      audio_melody_E1_error_trace(AUDIO_ENTITY_FILE_NO_CLOSE);
                    }
                  #endif
                  audio_melody_E1_send_status (AUDIO_ERROR, ((T_AUDIO_MELODY_E1_START *)(p_message))->return_path);
                  audio_melody_E1_error_trace(AUDIO_ERROR_START_EVENT);
                break;
              }
          }
        break;
        } /* case AUDIO_MELODY_E1_WAIT_STOP_COMMAND */

        case AUDIO_MELODY_E1_WAIT_STOP_CON:
        {
          switch(p_message->msg_id)
          {
             case MMI_MELODY1_STOP_CON:
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
                  audio_melody_E1_error_trace(AUDIO_ENTITY_NO_MEMORY);
                }

                /* fill the header of the message */
                ((T_AUDIO_FFS_STOP_REQ *)p_send_message)->os_hdr.msg_id = AUDIO_FFS_STOP_REQ;

                /* fill the parameter */
                ((T_AUDIO_FFS_STOP_REQ *)p_send_message)->session_id = AUDIO_FFS_SESSION_MELODY_E1_1;

                /* send the message to the entity */
                rvf_send_msg (p_audio_gbl_var->audio_ffs_addrId,
                              p_send_message);

                /* change to the state AUDIO_MELODY_1_WAIT_STOP_FFS */
                 p_audio_gbl_var->melody_E1_1.state = AUDIO_MELODY_E1_WAIT_STOP_FFS;
               break;
              }
             case AUDIO_MELODY_E1_STOP_REQ:
              {
                /* event error - send an error message  */
                audio_melody_E1_send_status (AUDIO_ERROR,((T_AUDIO_MELODY_E1_STOP *)p_message)->return_path);
                audio_melody_E1_error_trace(AUDIO_ERROR_STOP_EVENT);
               break;
              } /* case AUDIO_MELODY_E1_STOP_REQ */

             case AUDIO_MELODY_E1_START_REQ:
              {
                  /* event error - send an error message*/
                  /* Close the FFS file previously open by the audio start API*/
                  #ifndef _WINDOWS
#if(AS_RFS_API == 1)
					if ( rfs_close(((T_AUDIO_MELODY_E1_START *)p_message)->audio_ffs_fd) != RFS_EOK )
#else
					if ( ffs_close(((T_AUDIO_MELODY_E1_START *)p_message)->audio_ffs_fd) != EFFS_OK )
#endif
                    {
                      audio_melody_E1_error_trace(AUDIO_ENTITY_FILE_NO_CLOSE);
                    }
                  #endif
                  audio_melody_E1_send_status (AUDIO_ERROR, ((T_AUDIO_MELODY_E1_START *)(p_message))->return_path);
                  audio_melody_E1_error_trace(AUDIO_ERROR_START_EVENT);
                break;
              }

          }
        break;
        } /* case AUDIO_MELODY_E1_WAIT_STOP_CON */

        case AUDIO_MELODY_E1_WAIT_STOP_FFS:
        {
          switch(p_message->msg_id)
          {
             case AUDIO_FFS_STOP_CON:
              {
                audio_melody_E1_send_status (AUDIO_OK, p_audio_gbl_var->melody_E1_1.return_path);

                /* change to the state AUDIO_MELODY_1_IDLE */
                p_audio_gbl_var->melody_E1_1.state = AUDIO_MELODY_E1_IDLE;

                break;
              }
             case AUDIO_MELODY_E1_STOP_REQ:
              {
                 /* event error - send an error message  */
                 audio_melody_E1_send_status (AUDIO_ERROR,((T_AUDIO_MELODY_E1_STOP *)(p_message))->return_path);
                 audio_melody_E1_error_trace(AUDIO_ERROR_STOP_EVENT);
                break;
              } /* case AUDIO_MELODY_E1_STOP_REQ */

             case AUDIO_MELODY_E1_START_REQ:
              {
                  /* event error - send an error message*/
                  /* Close the FFS file previously open by the audio start API*/
                  #ifndef _WINDOWS
#if(AS_RFS_API == 1)
					if ( rfs_close(((T_AUDIO_MELODY_E1_START *)p_message)->audio_ffs_fd) != RFS_EOK )
#else
					if ( ffs_close(((T_AUDIO_MELODY_E1_START *)p_message)->audio_ffs_fd) != EFFS_OK )
#endif
                    {
                      audio_melody_E1_error_trace(AUDIO_ENTITY_FILE_NO_CLOSE);
                    }
                  #endif
                  audio_melody_E1_send_status (AUDIO_ERROR, ((T_AUDIO_MELODY_E1_START *)(p_message))->return_path);
                  audio_melody_E1_error_trace(AUDIO_ERROR_START_EVENT);
                break;
              }
          }
        break;
        } /* case AUDIO_MELODY_E1_WAIT_STOP_CON */
    } /* switch(p_audio_gbl_var->melody_E1_1.state) */
  } /*********************** End of audio_melody_E1_manager_1 function **********************/

  #endif /* MELODY_E1 */
#endif /* RVM_AUDIO_MAIN_SWE */

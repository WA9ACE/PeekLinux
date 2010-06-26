/****************************************************************************/
/*                                                                          */
/*  File Name:  tty.c                                                       */
/*                                                                          */
/*  Purpose:  This file contains all the functions used to manage the       */
/*            TTY feature.                                                  */
/*                                                                          */
/*  Version   0.1                                                           */
/*                                                                          */
/*  Date        Modification                                                */
/*  ------------------------------------                                    */
/*  18 Jan 2003 Create                                                      */
/*                                                                          */
/*  Author                                                                  */
/*     Frederic Turgis                                                      */
/*                                                                          */
/* (C) Copyright 2003 by Texas Instruments Incorporated, All Rights Reserved*/
/****************************************************************************/

#ifndef _WINDOWS
  #include "config/l1sw.cfg"
#endif

#if (L1_GTT == 1)

  #include "tty/tty_api.h"
  #include "tty/tty_i.h"

  #include "rv/rv_general.h"
  #include "rvf/rvf_target.h"

  #include "l1_types.h"
  #include "l1gtt_signa.h"
  #include "l1gtt_msgty.h"
  #include "audio/audio_structs_i.h"
  #include "audio/audio_const_i.h"

  /* extern variables & prototypes */

  extern T_TTY_ENV_CTRL_BLK *tty_env_ctrl_blk_p;
  extern T_AUDIO_ENV_CTRL_BLK *p_audio_gbl_var;

  //extern void    *audio_allocate_l1_message (UINT16 size);
  //extern T_RV_RET audio_send_l1_message     (INT16 message_id, DummyStruct *message);

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   tty_send_status                                          */
  /*                                                                              */
  /*    Purpose:  This function sends the TTY status to the entity.               */
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
  void tty_send_status (T_AUDIO_RET status, T_RV_RETURN return_path)
  {
    T_AUDIO_TTY_STATUS *p_send_message = NULL;
    T_RVF_MB_STATUS mb_status = RVF_RED;

    while (mb_status == RVF_RED)
    {
      /* allocate the message buffer */
      mb_status = rvf_get_buf (p_audio_gbl_var->mb_external,
                               sizeof (T_AUDIO_TTY_STATUS),
                               (T_RVF_BUFFER **) (&p_send_message));

      /* If insufficient resources, then report a memory error and abort. */
      /* and wait until more ressource is given */
      if (mb_status == RVF_RED)
      {
        TTY_SEND_TRACE("TTY: Not Enough Memory (Red!) ",RV_TRACE_LEVEL_ERROR);
        rvf_delay(RVF_MS_TO_TICKS(1000));
      }
    }
    /* fill the header of the message */
    ((T_AUDIO_TTY_STATUS *)p_send_message)->os_hdr.msg_id = AUDIO_TTY_STATUS_MSG;

    /* fill the status parameters */
    ((T_AUDIO_TTY_STATUS *)p_send_message)->status = status;

    if (return_path.callback_func == NULL)
    {
      /* send the message to the entity */
      rvf_send_msg (return_path.addr_id, p_send_message);
    }
    else
    {
      /* call the callback function */
      (*return_path.callback_func)((void *)(p_send_message));
      rvf_free_buf((T_RVF_BUFFER *)p_send_message);
    }
  }

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   tty_manager                                              */
  /*                                                                              */
  /*    Purpose:  This function is called to manage TTY                           */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        message                                                               */
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
  void tty_manager (T_RV_HDR *p_message)
  {
    /* Declare local variables. */
    void *p_send_message;

    /**************** tty_manager function begins *********************/
    switch(tty_env_ctrl_blk_p->state)
    {
      case TTY_IDLE:
      {
        switch(p_message->msg_id)
        {
          case TTY_START_REQ:
          {
            /* save the return path */
            tty_env_ctrl_blk_p->return_path.callback_func = ((T_TTY_START *)p_message)->return_path.callback_func;
            tty_env_ctrl_blk_p->return_path.addr_id       = ((T_TTY_START *)p_message)->return_path.addr_id;

            /* allocate the buffer for the message to the L1 */
            p_send_message = audio_allocate_l1_message(sizeof(T_MMI_GTT_START_REQ));
            ((T_MMI_GTT_START_REQ *)p_send_message)->tty_mode = ((T_TTY_START *)p_message)->tty_mode;

            if (p_send_message != NULL)
            {
              /* send the start command to the audio L1 */
              audio_send_l1_message(MMI_GTT_START_REQ, p_send_message);
            }

            /* change state */
            tty_env_ctrl_blk_p->state = TTY_WAIT_START_CON;
          }
          break;
          case TTY_STOP_REQ:
          {
            TTY_SEND_TRACE("TTY: STOP ERROR EVENT ",RV_TRACE_LEVEL_ERROR);
          }
          break;
        }
      } /* case TTY_IDLE */
      break;

      case TTY_WAIT_START_CON:
      {
        switch(p_message->msg_id)
        {
          case MMI_GTT_START_CON:
          {
            /* change state */
            tty_env_ctrl_blk_p->state = TTY_WAIT_STOP_COMMAND;
          }
          break;
          case TTY_STOP_REQ:
          {
            /* change state */
            tty_env_ctrl_blk_p->state = TTY_WAIT_START_CON_TO_STOP;
          }
          break;
        }
      } /* case TTY_WAIT_START_CON */
      break;

      case TTY_WAIT_START_CON_TO_STOP:
      {
        switch(p_message->msg_id)
        {
          case MMI_GTT_START_CON:
          {
            /* send the stop command to the audio L1 */
            p_send_message = audio_allocate_l1_message(0);
            if (p_send_message != NULL)
            {
              audio_send_l1_message(MMI_GTT_STOP_REQ, p_send_message);
            }

            /* change state */
            tty_env_ctrl_blk_p->state = TTY_WAIT_STOP_CON;
          }
          break;
          case TTY_STOP_REQ:
          {
            TTY_SEND_TRACE("TTY: STOP ERROR EVENT ",RV_TRACE_LEVEL_ERROR);
          }
          break;
        }
      }
      break;

      case TTY_WAIT_STOP_COMMAND:
      {
        switch(p_message->msg_id)
        {
          case TTY_STOP_REQ:
          {
            /* send the stop command to the audio L1 */
            p_send_message = audio_allocate_l1_message(0);
            if (p_send_message != NULL)
            {
              audio_send_l1_message(MMI_GTT_STOP_REQ, p_send_message);
            }

            /* change state */
            tty_env_ctrl_blk_p->state = TTY_WAIT_STOP_CON;
          }
          break;
          case MMI_GTT_STOP_CON:
          {
            tty_send_status (AUDIO_OK, tty_env_ctrl_blk_p->return_path);

            /* change state */
            tty_env_ctrl_blk_p->state = TTY_IDLE;
          }
          break;
        }
      } /* case TTY_WAIT_STOP_COMMAND */
      break;

      case TTY_WAIT_STOP_CON:
      {
        switch(p_message->msg_id)
        {
          case MMI_GTT_STOP_CON:
          {
            tty_send_status (AUDIO_OK, tty_env_ctrl_blk_p->return_path);

            /* change state */
            tty_env_ctrl_blk_p->state = TTY_IDLE;
          }
          break;
          case TTY_STOP_REQ:
          {
            TTY_SEND_TRACE("TTY: STOP ERROR EVENT ",RV_TRACE_LEVEL_ERROR);
          }
          break;
        }
      } /* case TTY_WAIT_STOP_CON */
      break;

    } /* switch(tty_env_ctrl_blk_p->state) */
  }
#endif /* L1_GTT */


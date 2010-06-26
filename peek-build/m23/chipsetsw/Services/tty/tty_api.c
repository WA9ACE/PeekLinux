/**
 * @file	tut_api.c
 *
 * API for TTY SWE.
 *
 * @author	Frederic Turgis (f-turgis@ti.com) & Gerard Cauvy (g-cauvy@ti.com)
 * @version 0.1
 */

/*
 * History:
 *
 * 	Date       	Modification
 *  ------------------------------------
 *  01/27/2003	Create
 *
 * (C) Copyright 2003 by Texas Instruments Incorporated, All Rights Reserved
 */

#ifndef _WINDOWS
  #include "config/l1sw.cfg"
#endif

#include "tty/tty_i.h"

#include "tty/tty_api.h"
#include "audio/audio_structs_i.h"
#include "audio/audio_const_i.h"

/* External declaration */
extern T_AUDIO_ENV_CTRL_BLK* p_audio_gbl_var;

/********************************************************************************/
/*                                                                              */
/*    Function Name:   audio_tty_set_config                                     */
/*                                                                              */
/*    Purpose:  This function is called to configure TTY. Currently, only       */
/*              supports start and stop                                         */
/*                                                                              */
/*    Input Parameters:                                                         */
/*        TTY configuration                                                     */
/*        Return path.                                                          */
/*                                                                              */
/*    Output Parameters:                                                        */
/*         Validation of the parameters.                                        */
/*                                                                              */
/*    Note:                                                                     */
/*        None.                                                                 */
/*                                                                              */
/*    Revision History:                                                         */
/*        None.                                                                 */
/*                                                                              */
/********************************************************************************/
T_AUDIO_RET audio_tty_set_config (T_AUDIO_TTY_CONFIG_PARAMETER *parameter, T_RV_RETURN *return_path)
{
  #if (L1_GTT == 1)
    /* Declare local variables. */
    T_RVF_MB_STATUS   mb_status = RVF_GREEN;
    T_RV_HDR *p_msg = NULL;

    /************************ audio_tty_set_config function begins ******************/
    if (p_audio_gbl_var == NULL )
    {
      TTY_SEND_TRACE("TTY: Error Audio SWE not started ",RV_TRACE_LEVEL_ERROR);
      return(AUDIO_ERROR);
    }

    /* If bad parameters, then report an error and abort.*/
    if ( (parameter->Mode != TTY_STOP)&&
         (parameter->Mode != TTY_EXT_START))
    {
      TTY_SEND_TRACE("TTY: Error bad parameters ",RV_TRACE_LEVEL_ERROR);
      return (AUDIO_ERROR);
    }

    switch (parameter->Mode)
    {
      case TTY_EXT_START:
      {
        /* allocate the memory for the message to send */
        mb_status = rvf_get_buf (p_audio_gbl_var->mb_external,
                                 sizeof (T_TTY_START),
                                 (T_RVF_BUFFER **) (&p_msg));
      }
      break;
      case TTY_STOP:
      {
        /* allocate the memory for the message to send */
        mb_status = rvf_get_buf (p_audio_gbl_var->mb_external,
                                 sizeof (T_TTY_STOP),
                                 (T_RVF_BUFFER **) (&p_msg));
      }
      break;
    }

    /* If insufficient resources, then report a memory error and abort. */
    if (mb_status == RVF_YELLOW)
    {
      /* deallocate the memory */
      rvf_free_buf((T_RVF_BUFFER *)p_msg);
      TTY_SEND_TRACE("TTY: Not Enough Memory (Yellow!) ",RV_TRACE_LEVEL_ERROR);
      return (AUDIO_ERROR);
    }
    else
    if (mb_status == RVF_RED)
    {
      TTY_SEND_TRACE("TTY: Not Enough Memory (Red!) ",RV_TRACE_LEVEL_ERROR);
      return (AUDIO_ERROR);
    }

    /* fill the message id + parameters */
    switch (parameter->Mode)
    {
      case TTY_EXT_START:
        p_msg->msg_id = TTY_START_REQ;

        if (return_path->callback_func == NULL)
        {
          ((T_TTY_START *)p_msg)->return_path.addr_id = return_path->addr_id;
          ((T_TTY_START *)p_msg)->return_path.callback_func = NULL;
        }
        else
        {
          ((T_TTY_START *)p_msg)->return_path.callback_func = return_path->callback_func;
        }
        ((T_TTY_START *)p_msg)->tty_mode = parameter->tty_mode; 
      break;
      case TTY_STOP:
        p_msg->msg_id = TTY_STOP_REQ;
      break;
    }

    /* fill the address source id */
    p_msg->src_addr_id = rvf_get_taskid();
    p_msg->dest_addr_id = p_audio_gbl_var->addrId;

    /* send the messsage to the audio entity */
    rvf_send_msg (p_audio_gbl_var->addrId, p_msg);

    return (AUDIO_OK);

  #else // L1_GTT

    TTY_SEND_TRACE("TTY API not available ", RV_TRACE_LEVEL_ERROR);
    return (AUDIO_ERROR);

  #endif
}

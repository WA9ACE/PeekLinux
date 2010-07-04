/****************************************************************************/
/*                                                                          */
/*  File Name:  audio_mode_volume.c                                         */
/*                                                                          */
/*  Purpose:  This file contains all the functions used for audio mode      */
/*            speaker volume services.                                      */
/*                                                                          */
/*  Version   0.1                                                           */
/*                                                                          */
/*  Date        Modification                                                */
/*  ------------------------------------------------------------------------*/
/*  14 Jan 2002  Create                                                     */
/*                                                                          */
/*  Author      Francois Mazard                                             */
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

  #include "l1_confg.h"
  #include "rv/rv_general.h"
  #include "rvm/rvm_gen.h"
#if (AS_RFS_API == 1)
  #include "rfs/rfs_api.h"
#else
  #include "Audio/audio_ffs_i.h"
#endif
  #include "Audio/audio_api.h"
  #include "Audio/audio_structs_i.h"
  #include "Audio/audio_error_hdlr_i.h"
  #include "Audio/audio_var_i.h"
  #include "Audio/audio_messages_i.h"
  #include "Audio/audio_macro_i.h"
  #include "rvf/rvf_target.h"
  #include "Audio/audio_const_i.h"

  /* include the usefull L1 header */
  #ifdef _WINDOWS
    #define BOOL_FLAG
    //#define CHAR_FLAG
  #endif
  #include "l1_types.h"
  #include "l1audio_const.h"
  #include "l1audio_cust.h"
  #include "l1audio_defty.h"
  #include "l1audio_msgty.h"
  #include "l1audio_signa.h"
  #if TESTMODE
   #include "l1tm_defty.h"
  #endif
  #if (L1_GTT == 1)
    #include "l1gtt_const.h"
    #include "l1gtt_defty.h"
  #endif
  #include "l1_const.h"
  #if (L1_MP3 == 1)
    #include "l1mp3_defty.h"
  #endif
  #if (L1_AAC == 1)
    #include "l1aac_defty.h"
  #endif
  #include "l1_defty.h"
  #include "l1_msgty.h"
  #include "l1_signa.h"
  #ifdef _WINDOWS
    #define L1_ASYNC_C
  #endif
  #include "l1_varex.h"

  #ifdef _WINDOWS
    #include "Audio/tests/audio_test.h"
  #endif

  /* external functions */
  /* write */
  extern T_AUDIO_RET audio_mode_speaker_volume_write          (T_AUDIO_SPEAKER_LEVEL *data);
  /* read */
  extern T_AUDIO_RET audio_mode_speaker_volume_read           (T_AUDIO_SPEAKER_LEVEL *data);
  /* write */
  extern T_AUDIO_RET audio_mode_stereo_speaker_volume_write   (T_AUDIO_STEREO_SPEAKER_LEVEL *data);
  /* read */
  extern T_AUDIO_RET audio_mode_stereo_speaker_volume_read    (T_AUDIO_STEREO_SPEAKER_LEVEL *data);

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_mode_speaker_volume_send_status                    */
  /*                                                                              */
  /*    Purpose:  This function sends the audio mode speaker volume status to     */
  /*              the entity.                                                     */
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
  void audio_mode_speaker_volume_send_status (T_AUDIO_RET status, T_RV_RETURN return_path)
  {
    T_AUDIO_VOLUME_DONE *p_send_message = NULL;
    T_RVF_MB_STATUS mb_status = RVF_RED;

    while (mb_status == RVF_RED)
    {
      /* allocate the message buffer */
      mb_status = rvf_get_buf (p_audio_gbl_var->mb_external,
                               sizeof (T_AUDIO_VOLUME_DONE),
                               (T_RVF_BUFFER **) (&p_send_message));

      /* If insufficient resources, then report a memory error and abort.               */
      /* and wait until more ressource is given */
      if (mb_status == RVF_RED)
      {
        audio_mode_error_trace(AUDIO_ENTITY_NO_MEMORY);
        rvf_delay(RVF_MS_TO_TICKS(1000));
      }
    }
    /*fill the header of the message */
    ((T_AUDIO_VOLUME_DONE *)p_send_message)->os_hdr.msg_id =
      AUDIO_SPEAKER_VOLUME_DONE;

    /* fill the status parameters */
    ((T_AUDIO_VOLUME_DONE *)p_send_message)->status = status;

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
  /*    Function Name:   audio_mode_speaker_volume_manager                        */
  /*                                                                              */
  /*    Purpose:  This function manage the audio mode save services.              */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        Audio message.                                                        */
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
  void audio_mode_speaker_volume_manager (T_RV_HDR *p_message)
  {
    T_AUDIO_SPEAKER_LEVEL volume;
    #ifndef _WINDOWS
#if(AS_RFS_API == 1)
	T_RFS_FD              audio_volume_ffs_fd;
#else
	T_FFS_FD              audio_volume_ffs_fd;
#endif
    #endif
#if(AS_RFS_API == 1)
    T_WCHAR                    audio_volume_path_name[AUDIO_PATH_NAME_MAX_SIZE];
#else
	char                    audio_volume_path_name[AUDIO_PATH_NAME_MAX_SIZE];
#endif

  #if(AS_RFS_API == 1)
		T_WCHAR mp_uc1[10];
  		char *str1 = ".vol";
  #endif
#if(AS_RFS_API == 1)
	convert_u8_to_unicode(str1, mp_uc1);
#endif


    /* get filename by appending .vol */
#if(AS_RFS_API == 1)
	wstrcpy(audio_volume_path_name, p_audio_gbl_var->audio_mode_var.audio_mode_path_name);
     wstrcat(audio_volume_path_name, mp_uc1);
#else
	strcpy(audio_volume_path_name, p_audio_gbl_var->audio_mode_var.audio_mode_path_name);
     strcat(audio_volume_path_name, ".vol");
#endif

    switch (((T_AUDIO_SPEAKER_VOLUME_REQ *)p_message)->volume.volume_action)
    {
      case AUDIO_SPEAKER_VOLUME_INCREASE:
      {
        if ( (audio_mode_speaker_volume_read(&volume)) == AUDIO_ERROR )
        {
          AUDIO_SEND_TRACE("AUDIO MODE SPEAKER VOLUME: can't read the current volume", RV_TRACE_LEVEL_ERROR);
          audio_mode_speaker_volume_send_status (AUDIO_ERROR,
            ((T_AUDIO_SPEAKER_VOLUME_REQ *)p_message)->return_path);
          return;
        }

        if (volume.audio_speaker_level == AUDIO_SPEAKER_VOLUME_MUTE)
        {
          volume.audio_speaker_level = AUDIO_SPEAKER_VOLUME_24dB;
        }
        else
        if (volume.audio_speaker_level == AUDIO_SPEAKER_VOLUME_24dB)
        {
          volume.audio_speaker_level = AUDIO_SPEAKER_VOLUME_18dB;
        }
        else
        if (volume.audio_speaker_level == AUDIO_SPEAKER_VOLUME_0dB)
        {
          volume.audio_speaker_level = AUDIO_SPEAKER_VOLUME_0dB;
        }
        else
        {
          volume.audio_speaker_level += 50;
        }
        break;
      }
      case AUDIO_SPEAKER_VOLUME_DECREASE:
      {
        if ( (audio_mode_speaker_volume_read(&volume)) == AUDIO_ERROR )
        {
          AUDIO_SEND_TRACE("AUDIO MODE SPEAKER VOLUME: can't read the current volume", RV_TRACE_LEVEL_ERROR);
          audio_mode_speaker_volume_send_status (AUDIO_ERROR,
            ((T_AUDIO_SPEAKER_VOLUME_REQ *)p_message)->return_path);
          return;
        }

        if (volume.audio_speaker_level == AUDIO_SPEAKER_VOLUME_MUTE)
        {
          volume.audio_speaker_level = AUDIO_SPEAKER_VOLUME_MUTE;
        }
        else
        if (volume.audio_speaker_level == AUDIO_SPEAKER_VOLUME_24dB)
        {
          volume.audio_speaker_level = AUDIO_SPEAKER_VOLUME_MUTE;
        }
        else
        if (volume.audio_speaker_level == AUDIO_SPEAKER_VOLUME_18dB)
        {
          volume.audio_speaker_level = AUDIO_SPEAKER_VOLUME_24dB;
        }
        else
        {
          volume.audio_speaker_level -= 50;
        }
        break;
      }
      case AUDIO_SPEAKER_VOLUME_SET:
      {
        volume.audio_speaker_level = ((T_AUDIO_SPEAKER_VOLUME_REQ *)p_message)->volume.value;
        break;
      }
    }

    /* Open the volume file */
    #ifndef _WINDOWS
#if(AS_RFS_API == 1)
	audio_volume_ffs_fd = rfs_open((T_WCHAR *)audio_volume_path_name,
          RFS_O_CREAT | RFS_O_WRONLY | RFS_O_TRUNC | RFS_O_APPEND, RFS_RDWREX_ALL);
#else
	audio_volume_ffs_fd = ffs_open(audio_volume_path_name,
          FFS_O_CREATE | FFS_O_WRONLY | FFS_O_TRUNC | FFS_O_APPEND);
#endif
      if (audio_volume_ffs_fd <= 0)
      {
          AUDIO_SEND_TRACE("AUDIO MODE SPEAKER VOLUME: can't open the current volume file", RV_TRACE_LEVEL_ERROR);
          /* Close the file */
#if(AS_RFS_API == 1)
		  rfs_close(audio_volume_ffs_fd);
#else
		  ffs_close(audio_volume_ffs_fd);
#endif

          audio_mode_speaker_volume_send_status (AUDIO_ERROR,
            ((T_AUDIO_SPEAKER_VOLUME_REQ *)p_message)->return_path);
        return;
      }

      /* Save the audio speaker volume structure from the FFS */
#if(AS_RFS_API == 1)
	  if ( (rfs_write (audio_volume_ffs_fd,
                      &volume,
                      sizeof(T_AUDIO_SPEAKER_LEVEL))) < RFS_EOK )
#else
	  if ( (ffs_write (audio_volume_ffs_fd,
                      &volume,
                      sizeof(T_AUDIO_SPEAKER_LEVEL))) < EFFS_OK )
#endif
      {
        AUDIO_SEND_TRACE("AUDIO MODE SPEAKER VOLUME: impossible to save the current speaker volume", RV_TRACE_LEVEL_ERROR);

        /* Close the file */
#if(AS_RFS_API == 1)
		rfs_close(audio_volume_ffs_fd);
#else
		ffs_close(audio_volume_ffs_fd);
#endif

        /* send the status message */
        audio_mode_speaker_volume_send_status (AUDIO_ERROR,
          ((T_AUDIO_SPEAKER_VOLUME_REQ *)p_message)->return_path);
        return;
      }

      /* Close the file */
#if(AS_RFS_API == 1)
	  rfs_close(audio_volume_ffs_fd);
#else
	  ffs_close(audio_volume_ffs_fd);
#endif
    #else
      #if ((AUDIO_REGR == SW_COMPILED) || (AUDIO_MISC == SW_COMPILED))
        p_audio_test->speaker_volume_1.audio_speaker_level = volume.audio_speaker_level;
      #endif
    #endif

    /* Fill the audio volume structure */
    if ( (audio_mode_speaker_volume_write(&volume)) == AUDIO_ERROR)
    {
      AUDIO_SEND_TRACE("AUDIO MODE LOAD: error in the the audio speaker volume set function", RV_TRACE_LEVEL_ERROR);

      /* send the status message */
      audio_mode_speaker_volume_send_status (AUDIO_ERROR,
        ((T_AUDIO_SPEAKER_VOLUME_REQ *)p_message)->return_path);

      return;
    }

    /* send the good status message */
    audio_mode_speaker_volume_send_status (AUDIO_OK,
      ((T_AUDIO_SPEAKER_VOLUME_REQ *)p_message)->return_path);
  }

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_mode_stereo_speaker_volume_send_status             */
  /*                                                                              */
  /*    Purpose:  This function sends the audio mode stereo speaker volume status */
  /*              to the entity.                                                  */
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
  void audio_mode_stereo_speaker_volume_send_status (T_AUDIO_RET status, T_RV_RETURN return_path)
  {
    void *p_send_message = NULL;
    T_RVF_MB_STATUS mb_status = RVF_RED;

    while (mb_status == RVF_RED)
    {
      /* allocate the message buffer */
      mb_status = rvf_get_buf (p_audio_gbl_var->mb_external,
                               sizeof (T_AUDIO_STEREO_VOLUME_DONE),
                               (T_RVF_BUFFER **) (&p_send_message));

      /* If insufficient resources, then report a memory error and abort.               */
      /* and wait until more ressource is given */
      if (mb_status == RVF_RED)
      {
        audio_mode_error_trace(AUDIO_ENTITY_NO_MEMORY);
        rvf_delay(RVF_MS_TO_TICKS(1000));
      }
    }
    /*fill the header of the message */
    ((T_AUDIO_STEREO_VOLUME_DONE *)p_send_message)->os_hdr.msg_id =
      AUDIO_STEREO_SPEAKER_VOLUME_DONE;

    /* fill the status parameters */
    ((T_AUDIO_STEREO_VOLUME_DONE *)p_send_message)->status = status;

    if (return_path.callback_func == NULL)
    {
      /* send the message to the entity */
      rvf_send_msg (return_path.addr_id, p_send_message);
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
  /*    Function Name:   audio_mode_stereo_speaker_volume_manager                 */
  /*                                                                              */
  /*    Purpose:  This function manage the audio mode stereo save services.       */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        Audio message.                                                        */
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
  void audio_mode_stereo_speaker_volume_manager (T_RV_HDR *p_message)
  {
    T_AUDIO_STEREO_SPEAKER_LEVEL volume;
#if(AS_RFS_API == 1)
	T_RFS_FD              audio_stereo_volume_ffs_fd;
#else
	T_FFS_FD              audio_stereo_volume_ffs_fd;
#endif
#if(AS_RFS_API == 1)
	T_WCHAR                  audio_stereo_volume_path_name[AUDIO_PATH_NAME_MAX_SIZE];
#else
   char                  audio_stereo_volume_path_name[AUDIO_PATH_NAME_MAX_SIZE];
#endif


#if(AS_RFS_API == 1)
		T_WCHAR mp_uc1[10];
  		char *str1 = ".volst";
#endif
#if(AS_RFS_API == 1)
	convert_u8_to_unicode(str1, mp_uc1);
#endif

    /* get filename by appending .vol */
#if(AS_RFS_API == 1)
		wstrcpy(audio_stereo_volume_path_name, p_audio_gbl_var->audio_mode_var.audio_mode_path_name);
        wstrcat(audio_stereo_volume_path_name, mp_uc1);
#else
      strcpy(audio_stereo_volume_path_name, p_audio_gbl_var->audio_mode_var.audio_mode_path_name);
      strcat(audio_stereo_volume_path_name, ".volst");
#endif

    switch (((T_AUDIO_STEREO_SPEAKER_VOLUME_REQ *)p_message)->volume.volume_action_left)
    {
      case AUDIO_STEREO_SPEAKER_VOLUME_INCREASE:
      {
        if ( (audio_mode_stereo_speaker_volume_read(&volume)) == AUDIO_ERROR )
        {
          AUDIO_SEND_TRACE("AUDIO MODE STEREO SPEAKER VOLUME: can't read the current volume", RV_TRACE_LEVEL_ERROR);
          audio_mode_stereo_speaker_volume_send_status (AUDIO_ERROR,
            ((T_AUDIO_STEREO_SPEAKER_VOLUME_REQ *)p_message)->return_path);
          return;
        }

        /* we go from mute to -30 dB */
        if (volume.audio_stereo_speaker_level_left == AUDIO_STEREO_SPEAKER_VOLUME_MUTE)
        {
          volume.audio_stereo_speaker_level_left = 30;
        }
        /* or we decrease by 1 if level has not yet reached 0dB */
        else
        if (volume.audio_stereo_speaker_level_left != 0)
        {
          volume.audio_stereo_speaker_level_left--;
        }
      } // case AUDIO_STEREO_SPEAKER_VOLUME_INCREASE:
      break;
      case AUDIO_STEREO_SPEAKER_VOLUME_DECREASE:
      {
        if (audio_mode_stereo_speaker_volume_read(&volume) == AUDIO_ERROR)
        {
          AUDIO_SEND_TRACE("AUDIO MODE STEREO SPEAKER VOLUME: can't read the current volume", RV_TRACE_LEVEL_ERROR);
          audio_mode_stereo_speaker_volume_send_status (AUDIO_ERROR,
            ((T_AUDIO_STEREO_SPEAKER_VOLUME_REQ *)p_message)->return_path);
          return;
        }

        /* -30dB to Mute */
        if (volume.audio_stereo_speaker_level_left == 30)
        {
          volume.audio_stereo_speaker_level_left = AUDIO_STEREO_SPEAKER_VOLUME_MUTE;
        }
        /* increase level by 1 if not in mute state */
        else
        if (volume.audio_stereo_speaker_level_left != AUDIO_STEREO_SPEAKER_VOLUME_MUTE)
        {
          volume.audio_stereo_speaker_level_left++;
        }
      } // case AUDIO_STEREO_SPEAKER_VOLUME_DECREASE:
      break;
      case AUDIO_STEREO_SPEAKER_VOLUME_SET:
      {
        volume.audio_stereo_speaker_level_left = ((T_AUDIO_STEREO_SPEAKER_VOLUME_REQ *)p_message)->volume.value_left;
      } // case AUDIO_STEREO_SPEAKER_VOLUME_SET:
      break;
    } // switch (((T_AUDIO_STEREO_SPEAKER_VOLUME_REQ *)p_message)->volume.volume_action)

    switch (((T_AUDIO_STEREO_SPEAKER_VOLUME_REQ *)p_message)->volume.volume_action_right)
    {
      case AUDIO_STEREO_SPEAKER_VOLUME_INCREASE:
      {
        if ( (audio_mode_stereo_speaker_volume_read(&volume)) == AUDIO_ERROR )
        {
          AUDIO_SEND_TRACE("AUDIO MODE STEREO SPEAKER VOLUME: can't read the current volume", RV_TRACE_LEVEL_ERROR);
          audio_mode_stereo_speaker_volume_send_status (AUDIO_ERROR,
            ((T_AUDIO_STEREO_SPEAKER_VOLUME_REQ *)p_message)->return_path);
          return;
        }

        /* we go from mute to -30 dB */
        if (volume.audio_stereo_speaker_level_right == AUDIO_STEREO_SPEAKER_VOLUME_MUTE)
        {
          volume.audio_stereo_speaker_level_right = 30;
        }
        /* or we decrease by 1 if level has not yet reached 0dB */
        else
        if (volume.audio_stereo_speaker_level_right != 0)
        {
          volume.audio_stereo_speaker_level_right--;
        }
      } // case AUDIO_STEREO_SPEAKER_VOLUME_INCREASE:
      break;
      case AUDIO_STEREO_SPEAKER_VOLUME_DECREASE:
      {
        if (audio_mode_stereo_speaker_volume_read(&volume) == AUDIO_ERROR)
        {
          AUDIO_SEND_TRACE("AUDIO MODE STEREO SPEAKER VOLUME: can't read the current volume", RV_TRACE_LEVEL_ERROR);
          audio_mode_stereo_speaker_volume_send_status (AUDIO_ERROR,
            ((T_AUDIO_STEREO_SPEAKER_VOLUME_REQ *)p_message)->return_path);
          return;
        }

        /* -30dB to Mute */
        if (volume.audio_stereo_speaker_level_right == 30)
        {
          volume.audio_stereo_speaker_level_right = AUDIO_STEREO_SPEAKER_VOLUME_MUTE;
        }
        /* increase level by 1 if not in mute state */
        else
        if (volume.audio_stereo_speaker_level_right != AUDIO_STEREO_SPEAKER_VOLUME_MUTE)
        {
          volume.audio_stereo_speaker_level_right++;
        }
      } // case AUDIO_STEREO_SPEAKER_VOLUME_DECREASE:
      break;
      case AUDIO_STEREO_SPEAKER_VOLUME_SET:
      {
        volume.audio_stereo_speaker_level_right = ((T_AUDIO_STEREO_SPEAKER_VOLUME_REQ *)p_message)->volume.value_right;
      } // case AUDIO_SPEAKER_VOLUME_SET:
      break;
    } // switch (((T_AUDIO_STEREO_SPEAKER_VOLUME_REQ *)p_message)->volume.volume_action)

    /* Open the volume file */
#if(AS_RFS_API == 1)
	audio_stereo_volume_ffs_fd = rfs_open((T_WCHAR *)audio_stereo_volume_path_name,
          RFS_O_CREAT | RFS_O_WRONLY | RFS_O_TRUNC | RFS_O_APPEND, RFS_RDWREX_ALL );
#else
	audio_stereo_volume_ffs_fd = ffs_open(audio_stereo_volume_path_name,
          FFS_O_CREATE | FFS_O_WRONLY | FFS_O_TRUNC | FFS_O_APPEND);
#endif
#if(AS_RFS_API == 1)
	if (audio_stereo_volume_ffs_fd < RFS_EOK)
#else
	if (audio_stereo_volume_ffs_fd < EFFS_OK)
#endif
    {
        AUDIO_SEND_TRACE("AUDIO MODE STEREO SPEAKER VOLUME: can't open the current volume file", RV_TRACE_LEVEL_ERROR);
        /* Close the file */
#if(AS_RFS_API == 1)
		rfs_close(audio_stereo_volume_ffs_fd);
#else
		ffs_close(audio_stereo_volume_ffs_fd);
#endif

        audio_mode_stereo_speaker_volume_send_status (AUDIO_ERROR,
          ((T_AUDIO_STEREO_SPEAKER_VOLUME_REQ *)p_message)->return_path);
      return;
    }

    /* Save the audio speaker volume structure from the FFS */
#if(AS_RFS_API == 1)
	if ( (rfs_write (audio_stereo_volume_ffs_fd,
                    &volume,
                    sizeof(T_AUDIO_STEREO_SPEAKER_LEVEL))) < RFS_EOK )
#else
	if ( (ffs_write (audio_stereo_volume_ffs_fd,
                    &volume,
                    sizeof(T_AUDIO_STEREO_SPEAKER_LEVEL))) < EFFS_OK )
#endif
    {
      AUDIO_SEND_TRACE("AUDIO MODE STEREO SPEAKER VOLUME: impossible to save the current speaker volume", RV_TRACE_LEVEL_ERROR);

      /* Close the file */
#if(AS_RFS_API == 1)
	  rfs_close(audio_stereo_volume_ffs_fd);
#else
	  ffs_close(audio_stereo_volume_ffs_fd);
#endif

      /* send the status message */
      audio_mode_stereo_speaker_volume_send_status (AUDIO_ERROR,
        ((T_AUDIO_STEREO_SPEAKER_VOLUME_REQ *)p_message)->return_path);
      return;
    }

    /* Close the file */
#if(AS_RFS_API == 1)
	rfs_close(audio_stereo_volume_ffs_fd);
#else
	ffs_close(audio_stereo_volume_ffs_fd);
#endif

    /* Fill the audio volume structure */
    if ( (audio_mode_stereo_speaker_volume_write(&volume)) == AUDIO_ERROR)
    {
      AUDIO_SEND_TRACE("AUDIO MODE LOAD: error in the the audio stereo speaker volume set function", RV_TRACE_LEVEL_ERROR);

      /* send the status message */
      audio_mode_stereo_speaker_volume_send_status (AUDIO_ERROR,
        ((T_AUDIO_STEREO_SPEAKER_VOLUME_REQ *)p_message)->return_path);

      return;
    }

    /* send the good status message */
    audio_mode_stereo_speaker_volume_send_status (AUDIO_OK,
      ((T_AUDIO_STEREO_SPEAKER_VOLUME_REQ *)p_message)->return_path);
  }
#endif /* RVM_AUDIO_MAIN_SWE */

/****************************************************************************/
/*                                                                          */
/*  File Name:  audio_mode.c                                                */
/*                                                                          */
/*  Purpose:  This file contains all the functions used for audio mode misc */
/*            services.                                                     */
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
    #include "l1sw.cfg"
  #endif

  #include "l1_confg.h"
  #include "rv/rv_general.h"
  #include "rvm/rvm_gen.h"
  #include "audio/audio_ffs_i.h"
  #include "audio/audio_api.h"
  #include "audio/audio_structs_i.h"
  #include "audio/audio_error_hdlr_i.h"
  #include "audio/audio_var_i.h"
  #include "audio/audio_messages_i.h"
  #include "audio/audio_macro_i.h"
  #include "rvf/rvf_target.h"
  #include "audio/audio_const_i.h"

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

  /* external functions */
  extern void audio_mode_full_access_write_send_status (T_AUDIO_RET status, T_RV_RETURN return_path);
  extern void audio_mode_load_send_status              (T_AUDIO_RET status, T_RV_RETURN return_path);
  extern void audio_mode_save_send_status              (T_AUDIO_RET status, T_RV_RETURN return_path);
  extern void audio_mode_speaker_volume_send_status    (T_AUDIO_RET status, T_RV_RETURN return_path);
  extern void audio_mode_stereo_speaker_volume_send_status (T_AUDIO_RET status, T_RV_RETURN return_path);

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_mode_message_switch                                */
  /*                                                                              */
  /*    Purpose:  This function manages the dispatching of the message usefull    */
  /*              for audio mode services.                                        */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        Audio message.                                                        */
  /*                                                                              */
  /*    Output Parameters:                                                        */
  /*        ID of the message receptor.                                           */
  /*                                                                              */
  /*    Note:                                                                     */
  /*        None.                                                                 */
  /*                                                                              */
  /*    Revision History:                                                         */
  /*        None.                                                                 */
  /*                                                                              */
  /********************************************************************************/
  UINT8 audio_mode_message_switch(T_RV_HDR *p_message)
  {
    switch(p_message->msg_id)
    {
      case AUDIO_MODE_LOAD_REQ :
      {
        if ( (p_audio_gbl_var->audio_mode_var.audio_mode_load_var.state != AUDIO_MODE_LOAD_IDLE) ||
             (p_audio_gbl_var->audio_mode_var.full_access_write_var.state != AUDIO_MODE_FULL_WRITE_IDLE) )
        {
          AUDIO_SEND_TRACE("AUDIO MODE LOAD: an entity already uses this service", RV_TRACE_LEVEL_ERROR);
	   /* Close the files */
          ffs_close(((T_AUDIO_MODE_LOAD_REQ *)p_message)->audio_ffs_fd);
          ffs_close(((T_AUDIO_MODE_LOAD_REQ *)p_message)->audio_volume_ffs_fd);
          #if ((ANLG_FAM == 3) || (ANLG_FAM == 4) || (ANLG_FAM == 11))
            ffs_close(((T_AUDIO_MODE_LOAD_REQ *)p_message)->audio_stereo_volume_ffs_fd);
          #endif

          audio_mode_load_send_status (AUDIO_ERROR, ((T_AUDIO_MODE_LOAD_REQ *)p_message)->return_path);
          p_audio_gbl_var->message_processed = TRUE;
          return (AUDIO_MODE_NONE);
        }
        return(AUDIO_MODE_LOAD);
      }
      case AUDIO_MODE_SAVE_REQ :
      {
        if ( (p_audio_gbl_var->audio_mode_var.audio_mode_load_var.state != AUDIO_MODE_LOAD_IDLE) ||
             (p_audio_gbl_var->audio_mode_var.full_access_write_var.state != AUDIO_MODE_FULL_WRITE_IDLE) )
        {
          AUDIO_SEND_TRACE("AUDIO MODE SAVE: an entity currently changes the audio mode", RV_TRACE_LEVEL_ERROR);
        /* Close the files */
          ffs_close(((T_AUDIO_MODE_SAVE_REQ *)p_message)->audio_ffs_fd);
          ffs_close(((T_AUDIO_MODE_SAVE_REQ *)p_message)->audio_volume_ffs_fd);
          #if ((ANLG_FAM == 3) || (ANLG_FAM == 4) || (ANLG_FAM == 11))
            ffs_close(((T_AUDIO_MODE_SAVE_REQ *)p_message)->audio_stereo_volume_ffs_fd);
          #endif  

          audio_mode_save_send_status (AUDIO_ERROR, ((T_AUDIO_MODE_SAVE_REQ *)p_message)->return_path);
          p_audio_gbl_var->message_processed = TRUE;
          return (AUDIO_MODE_NONE);
        }
        return(AUDIO_MODE_SAVE);
      }
      case AUDIO_MODE_WRITE_REQ :
      {
        if ( (p_audio_gbl_var->audio_mode_var.audio_mode_load_var.state != AUDIO_MODE_LOAD_IDLE) ||
             (p_audio_gbl_var->audio_mode_var.full_access_write_var.state != AUDIO_MODE_FULL_WRITE_IDLE) )
        {
          AUDIO_SEND_TRACE("AUDIO MODE FULL ACCESS WRITE: an entity currently changes the audio mode", RV_TRACE_LEVEL_ERROR);
          audio_mode_full_access_write_send_status (AUDIO_ERROR,
            ((T_AUDIO_MODE_FULL_ACCESS_WRITE_REQ *)p_message)->return_path);
// Nina added to fix mem leakage in this branch
if(((T_AUDIO_MODE_FULL_ACCESS_WRITE_REQ *)p_message)->audio_parameter.data != NULL)
{
		  rvf_free_buf(((T_AUDIO_MODE_FULL_ACCESS_WRITE_REQ *)p_message)->audio_parameter.data);
 }
          p_audio_gbl_var->message_processed = TRUE;
          return (AUDIO_MODE_NONE);
        }
        return(AUDIO_FULL_ACCESS_WRITE);
      }
      case OML1_START_DAI_TEST_CON :
      case OML1_STOP_DAI_TEST_CON :
      #if (AUDIO_MODE)
        case MMI_AUDIO_MODE_CON :
      #endif
      #if (FIR)
        case MMI_AUDIO_FIR_CON :
      #endif
	  #if (L1_AEC == 1)
      case MMI_AEC_CON :
	  #endif
	  #if (L1_AEC == 2)
	  case MMI_AQI_AEC_CON :
	  #endif
      #if (L1_ANR == 1)
        case MMI_ANR_CON :
      #endif
      #if (L1_ANR == 2)
        case MMI_AQI_ANR_CON :
      #endif	  
      #if (L1_ES == 1)
        case MMI_ES_CON :
      #endif
      #if (L1_IIR == 1)
        case MMI_IIR_CON :
      #elif(L1_IIR == 2)
	 case MMI_AQI_IIR_DL_CON :
      #endif
      #if (L1_AGC_UL == 1)
        case MMI_AQI_AGC_UL_CON :
      #endif
      #if (L1_AGC_DL == 1)
        case MMI_AQI_AGC_DL_CON :
      #endif
      #if (L1_DRC == 1)
        case MMI_AQI_DRC_CON :
      #endif	  
      #if (L1_LIMITER == 1)
        case MMI_LIMITER_CON :
      #endif
      #if (AUDIO_DSP_ONOFF == 1 || L1_EXT_MCU_AUDIO_VOICE_ONOFF == 1)
        case MMI_AUDIO_ONOFF_CON:
      #endif
      {
        if (p_audio_gbl_var->audio_mode_var.audio_mode_load_var.state != AUDIO_MODE_LOAD_IDLE)
        {
          return(AUDIO_MODE_LOAD);
        }
        else
        if (p_audio_gbl_var->audio_mode_var.full_access_write_var.state != AUDIO_MODE_FULL_WRITE_IDLE)
        {
          return(AUDIO_FULL_ACCESS_WRITE);
        }
      }
      break;
      case AUDIO_SPEAKER_VOLUME_REQ:
      {
        if ( (p_audio_gbl_var->audio_mode_var.audio_mode_load_var.state != AUDIO_MODE_LOAD_IDLE) ||
             (p_audio_gbl_var->audio_mode_var.full_access_write_var.state != AUDIO_MODE_FULL_WRITE_IDLE) )
        {
          AUDIO_SEND_TRACE("AUDIO MODE SPEAKER VOLUME: an entity currently changes the audio mode", RV_TRACE_LEVEL_ERROR);
          audio_mode_speaker_volume_send_status (AUDIO_ERROR,
            ((T_AUDIO_SPEAKER_VOLUME_REQ *)p_message)->return_path);
          p_audio_gbl_var->message_processed = TRUE;
          return (AUDIO_MODE_NONE);
        }
        return(AUDIO_SPEAKER_VOLUME);
      }
      case AUDIO_STEREO_SPEAKER_VOLUME_REQ:
      {
        if ( (p_audio_gbl_var->audio_mode_var.audio_mode_load_var.state != AUDIO_MODE_LOAD_IDLE) ||
             (p_audio_gbl_var->audio_mode_var.full_access_write_var.state != AUDIO_MODE_FULL_WRITE_IDLE) )
        {
          AUDIO_SEND_TRACE("AUDIO MODE STEREO SPEAKER VOLUME: an entity currently changes the audio mode", RV_TRACE_LEVEL_ERROR);
          audio_mode_stereo_speaker_volume_send_status (AUDIO_ERROR,
            ((T_AUDIO_STEREO_SPEAKER_VOLUME_REQ *)p_message)->return_path);
          p_audio_gbl_var->message_processed = TRUE;
          return (AUDIO_MODE_NONE);
        }
        return(AUDIO_STEREO_SPEAKER_VOLUME);
      }
    }
    return (AUDIO_MODE_NONE);
  }
#endif /*RVM_AUDIO_MAIN_SWE*/

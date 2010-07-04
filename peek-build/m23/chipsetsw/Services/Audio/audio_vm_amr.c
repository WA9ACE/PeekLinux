/****************************************************************************/
/*                                                                          */
/*  File Name:  audio_vm_amr.c		                                        */
/*                                                                          */
/*  Purpose:  This file contains all the functions used to manage the       */
/*            Voice Memorization AMR.                                       */
/*                                                                          */
/*  Version   0.1                                                           */
/*                                                                          */
/*  Date        Modification                                                */
/*  ------------------------------------                                    */
/*  2 November 2002 Create                                                  */
/*                                                                          */
/*  Author                                                                  */
/*     Frederic Turgis                                                      */
/*                                                                          */
/* (C) Copyright 2002 by Texas Instruments Incorporated, All Rights Reserved*/
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

#if (L1_VOICE_MEMO_AMR)
  #include "rv/rv_general.h"
  #include "rvm/rvm_gen.h"
  #include "Audio/audio_features_i.h"
  #include "Audio/audio_ffs_i.h"
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

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_voice_memo_amr_message_switch	                  */
  /*                                                                              */
  /*    Purpose:  Manage the message supply between the voice memo play and the   */
  /*              voice memo record AMR                                           */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        start or stop message from the voice memo features                    */
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
  UINT8 audio_voice_memo_amr_memory_message_switch (T_RV_HDR *p_message)
  {
    switch (p_message->msg_id)
    {
    #if(AUDIO_NEW_FFS_MANAGER)
      case AUDIO_VM_AMR_RECORD_TO_FFS_START_REQ:
      case AUDIO_VM_AMR_RECORD_TO_FFS_STOP_REQ:
      {
        return(AUDIO_VM_AMR_RECORD);
      }
    #endif
    #if(AUDIO_RAM_MANAGER)
      case AUDIO_VM_AMR_RECORD_TO_RAM_START_REQ:
      case AUDIO_VM_AMR_RECORD_TO_RAM_STOP_REQ:
      {
        return(AUDIO_VM_AMR_RECORD);
      }
    #endif
    #if (AUDIO_NEW_FFS_MANAGER)
      case AUDIO_VM_AMR_PLAY_FROM_FFS_START_REQ:
      case AUDIO_VM_AMR_PLAY_FROM_FFS_STOP_REQ:
      case AUDIO_VM_AMR_PLAY_FROM_FFS_PAUSE_REQ:
      case AUDIO_VM_AMR_PLAY_FROM_FFS_RESUME_REQ:
        
      {
        return(AUDIO_VM_AMR_PLAY);
      }
    #endif
    #if (AUDIO_RAM_MANAGER)
      case AUDIO_VM_AMR_PLAY_FROM_RAM_START_REQ:
      case AUDIO_VM_AMR_PLAY_FROM_RAM_STOP_REQ:
	  case AUDIO_VM_AMR_PLAY_FROM_RAM_PAUSE_REQ:
      case AUDIO_VM_AMR_PLAY_FROM_RAM_RESUME_REQ:
      {
        return(AUDIO_VM_AMR_PLAY);
      }
    #endif
      /* driver init => check session_id is VM AMR */
      case AUDIO_DRIVER_INIT_STATUS_MSG:
      {
        UINT8 session_id = ((T_AUDIO_DRIVER_INIT_STATUS *)p_message)->session_id;

        /* session_id is VM AMR */
        if ( (session_id == AUDIO_VM_AMR_PLAY_SESSION_ID)&&
             (p_audio_gbl_var->audio_vm_amr_play.state != AUDIO_IDLE))
        {
          return (AUDIO_VM_AMR_PLAY);
        }

        if ( (session_id == AUDIO_VM_AMR_RECORD_SESSION_ID)&&
             (p_audio_gbl_var->audio_vm_amr_record.state != AUDIO_IDLE))
        {
          return (AUDIO_VM_AMR_RECORD);
        }

        return(AUDIO_VM_AMR_NONE);
      }

      case AUDIO_DRIVER_NOTIFICATION_MSG:
      case AUDIO_DRIVER_LAST_NOTIFICATION_MSG:
      {
        UWORD8 channel_id;
        channel_id = ((T_AUDIO_DRIVER_NOTIFICATION *)p_message)->channel_id;
        if( (channel_id == p_audio_gbl_var->audio_vm_amr_play.channel_id)
          &&(p_audio_gbl_var->audio_vm_amr_play.state > AUDIO_WAIT_CHANNEL_ID) )
          return(AUDIO_VM_AMR_PLAY);
        if( (channel_id == p_audio_gbl_var->audio_vm_amr_record.channel_id)
         && (p_audio_gbl_var->audio_vm_amr_record.state > AUDIO_WAIT_CHANNEL_ID) )
          return(AUDIO_VM_AMR_RECORD);
        return(AUDIO_VM_AMR_NONE);
      }

      case AUDIO_DRIVER_STATUS_MSG:
      {
        UWORD8 channel_id;
        channel_id = ((T_AUDIO_DRIVER_STATUS *)p_message)->channel_id;
        if( (channel_id == p_audio_gbl_var->audio_vm_amr_play.channel_id)
         && (p_audio_gbl_var->audio_vm_amr_play.state > AUDIO_WAIT_CHANNEL_ID) )
          return(AUDIO_VM_AMR_PLAY);
        if( (channel_id == p_audio_gbl_var->audio_vm_amr_record.channel_id)
         && (p_audio_gbl_var->audio_vm_amr_record.state > AUDIO_WAIT_CHANNEL_ID) )
          return(AUDIO_VM_AMR_RECORD);
        return(AUDIO_VM_AMR_NONE);
      }

      default:
        return(AUDIO_VM_AMR_NONE);
    } /* switch */
  }

#endif /* L1_VOICE_MEMO_AMR */
#endif /* RVM_AUDIO_MAIN_SWE */

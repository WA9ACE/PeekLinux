/****************************************************************************/
/*                                                                          */
/*  File Name:  audio_vm.c			                                        */
/*                                                                          */
/*  Purpose:  This file contains all the functions used to manage the       */
/*            Voice Memorization .                                          */
/*                                                                          */
/*  Version   0.1                                                           */
/*                                                                          */
/*  Date        Modification                                                */
/*  ------------------------------------                                    */
/*  2 August 2001 Create                                                    */
/*                                                                          */
/*  Author                                                                  */
/*     Stephanie Gerthoux                                                   */
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

 #if(L1_PCM_EXTRACTION)
  #include "rv/rv_general.h"
  #include "rvm/rvm_gen.h"
  #include "Audio/audio_ffs_i.h"
  #include "Audio/audio_api.h"
  #include "Audio/audio_structs_i.h"
  #include "Audio/audio_macro_i.h"
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
  /*    Function Name:   audio_vbuf_pcm_message_switch	                      */
  /*                                                                              */
  /*    Purpose:  Manage the message supply between the vice memo play and the    */
  /*              voice memo record                                               */
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
  UINT8 audio_vbuf_pcm_message_switch (T_RV_HDR *p_message)
  {
    AUDIO_SEND_TRACE_PARAM("AUDIO VBUF PCM MSG SWITCH msg id:",p_message->msg_id,RV_TRACE_LEVEL_DEBUG_LOW);
    switch (p_message->msg_id)
    {
      case AUDIO_FFS_INIT_DONE:
      {
        /* The FFS message is from the voice memo play */
        if ( ((T_AUDIO_FFS_INIT *)p_message)->session_id == AUDIO_FFS_SESSION_VBUF_PCM_PLAY)
        {
          return(AUDIO_VBUF_PCM_PLAY);
        }
        /* The FFS message is from the voice memo record */
        if ( ((T_AUDIO_FFS_INIT *)p_message)->session_id == AUDIO_FFS_SESSION_VBUF_PCM_RECORD)
        {
          return(AUDIO_VBUF_PCM_RECORD);
        }
        return(AUDIO_VBUF_PCM_NONE);
      }
      case AUDIO_FFS_STOP_CON:
      {
        /* The FFS message is from the voice memo play*/
        if ( ((T_AUDIO_FFS_STOP_REQ *)p_message)->session_id == AUDIO_FFS_SESSION_VBUF_PCM_PLAY )
        {
          return(AUDIO_VBUF_PCM_PLAY);
        }
        /* The FFS message is from the voice memo record */
        if ( ((T_AUDIO_FFS_STOP_REQ *)p_message)->session_id == AUDIO_FFS_SESSION_VBUF_PCM_RECORD )
        {
          return(AUDIO_VBUF_PCM_RECORD);
        }
        return(AUDIO_VBUF_PCM_NONE);
      }
      case AUDIO_VBUF_PCM_PLAY_START_REQ:
      case AUDIO_VBUF_PCM_PLAY_STOP_REQ:
      {
        return(AUDIO_VBUF_PCM_PLAY);
      }

      case AUDIO_VBUF_PCM_RECORD_START_REQ:
      case AUDIO_VBUF_PCM_RECORD_STOP_REQ:
      case AUDIO_VBUF_PCM_PLAY_STOP_CON:
      {
      	 return(AUDIO_VBUF_PCM_RECORD);
      }

      case MMI_PCM_DOWNLOAD_START_CON:
      case MMI_PCM_DOWNLOAD_STOP_CON:
      {
	  	if(p_audio_gbl_var->vmemo_vbuf_session == AUDIO_VBUF_PCM_SESSION)
		{
			AUDIO_SEND_TRACE("AUDIO VBUF PCM MSG SWITCH: Returning AUDIO_VBUF_PCM_PLAY",RV_TRACE_LEVEL_DEBUG_LOW);
		        return(AUDIO_VBUF_PCM_PLAY);
	  	}
      }
      case MMI_PCM_UPLOAD_START_CON:
      case MMI_PCM_UPLOAD_STOP_CON:
      {
	  	if(p_audio_gbl_var->vmemo_vbuf_session == AUDIO_VBUF_PCM_SESSION)
		{
			AUDIO_SEND_TRACE("AUDIO VBUF PCM MSG SWITCH: Returning AUDIO_VBUF_PCM_RECORD",RV_TRACE_LEVEL_DEBUG_LOW);
		        return(AUDIO_VBUF_PCM_RECORD);
	  	}
      }
  //    default:
  //    {
   //     return(AUDIO_VBUF_PCM_NONE);
      //  break;
   //   }

    } /* switch */

    return(AUDIO_VBUF_PCM_NONE);
  }

  #endif /*  L1_PCM_EXTRACTION */
#endif /* RVM_AUDIO_MAIN_SWE */

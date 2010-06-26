/****************************************************************************/
/*                                                                          */
/*  File Name:  audio_cust.c                                                */
/*                                                                          */
/*  Purpose:  This file contains some custom functions                      */
/*                                                                          */
/*  Version   0.1                                                           */
/*                                                                          */
/*  Date          Modification                                              */
/*  ------------------------------------                                    */
/*  2 February 2005   Create                                                */
/*                                                                          */
/*  Author                                                                  */
/*     Stephanie Levieil                                                    */
/*                                                                          */
/* (C) Copyright 2005 by Texas Instruments Incorporated, All Rights Reserved*/
/****************************************************************************/

#include "rv/rv_defined_swe.h"
#ifdef RVM_AUDIO_MAIN_SWE

  /* include the usefull L1 header */
  #include "l1_confg.h"
  #include "l1sw.cfg"

#if ((L1_MP3 == 1) || (L1_AAC == 1))
  #include "rv/rv_general.h"
  #include "rvm/rvm_gen.h"
  #include "audio/audio_features_i.h"
  #include "audio/audio_api.h"
  #include "audio/audio_structs_i.h"
  #include "audio/audio_var_i.h"
  #include "audio/audio_messages_i.h"
  #include "audio/audio_const_i.h"
  #include "audio/audio_error_hdlr_i.h"

  /* include the usefull L1 header */
  #define BOOL_FLAG
  #define CHAR_FLAG
  #include "l1_types.h"
  #include "l1audio_cust.h"

  #if (L1_MP3 == 1)
    #include "l1mp3_signa.h"
    #include "audio/audio_mp3_i.h"
  #endif

  #if (L1_AAC == 1)
    #include "l1aac_signa.h"
    #include "audio/audio_aac_i.h"
  #endif


  #include "audio/audio_macro_i.h"

  /*-------------------------------------------------------*/
  /* Cust_get_pointer_next_buffer                          */
  /*-------------------------------------------------------*/
  /*                                                       */
  /* Parameters : ptr                                      */
  /*              buffer_size                              */
  /*              session_id                               */
  /*                                                       */
  /* Return     : error_id :                               */
  /*                DATA_AVAILABLE: 0, no error occured    */
  /*                SESSION_ERROR:  1, wrong session id    */
  /*                POINTER_ERROR:  2, wrong ptr           */
  /*                DATA_AVAIL_ERROR:3, no more data       */
  /*                                     available         */
  /*                DATA_LAST:      4, last buffer         */
  /*              buffer_size                              */
  /*              ptr                                      */
  /*                                                       */
  /* Description :                                         */
  /* The L1 calls this function to request new data buffer */
  /* (requested size: buffer_size). This function returns  */
  /* the description of the new data buffer                */
  /* (start address: ptr and the size: buffer_size)        */
  /* Moreover, the L1 indicates the position of the last   */
  /* data used via the ptr argument.                       */
  /* Note that this function can be returns an error.      */
  /*                                                       */
  /*-------------------------------------------------------*/
  UWORD8 Cust_get_pointer_next_buffer (UWORD16 **ptr, UWORD16 *buffer_size, UWORD8 session_id)
  {
      UINT8   index_l1;

    #if (L1_AUDIO_DRIVER == 1)
      T_AUDIO_DRIVER_SESSION *p_driver_session;
      UINT8 channel_id;
      UINT8 index_last_buffer=0;

      /* special handling for features in driver */
      if ((session_id == AUDIO_MP3_SESSION_ID) || (session_id == AUDIO_AAC_SESSION_ID))
      {
        channel_id = 0;
        /* find active channel_id associated to session_id */
        while ( (channel_id < AUDIO_DRIVER_MAX_CHANNEL)&&
              ((p_audio_gbl_var->audio_driver_session[channel_id].session_info.state == AUDIO_DRIVER_CHANNEL_WAIT_INIT)||
               (p_audio_gbl_var->audio_driver_session[channel_id].session_req.session_id != session_id)) )
        {
          channel_id++;
        }
	
	if(channel_id >= AUDIO_DRIVER_MAX_CHANNEL)
	  return(SESSION_ERROR);

	/* get driver session */
        p_driver_session = &(p_audio_gbl_var->audio_driver_session[channel_id]);

        /* first buffer, index_l1 = 0 */
        if (*ptr == NULL)
        {
          *ptr = (UWORD16 *)(p_driver_session->session_info.buffer[0].p_start_pointer);
          *buffer_size = (p_driver_session->session_info.buffer[0].size);
          return(DATA_AVAILABLE);
        }
        /* following buffers */
        else
        {
          /* fill message with current buffer for record */
          index_l1 = p_driver_session->session_info.index_l1;

          /* give new buffer = index_l1++ % nb_buffer */
          p_driver_session->session_info.index_l1++;
          if (p_driver_session->session_info.index_l1 == p_driver_session->session_req.nb_buffer)
              p_driver_session->session_info.index_l1 = 0;

          /* Update new current buffer parameters */
          index_l1 = p_driver_session->session_info.index_l1;
          *ptr = (UWORD16 *)(p_driver_session->session_info.buffer[index_l1].p_start_pointer);
          *buffer_size = (p_driver_session->session_info.buffer[index_l1].size);

          // Send confirmation message...
          #if (L1_MP3 == 1)
            if (p_audio_gbl_var->audio_mp3.state != AUDIO_IDLE)
            { /* MP3 is playing... */
              index_last_buffer = p_audio_gbl_var->audio_mp3.index_last_buffer;
            }
          #endif
          #if (L1_AAC == 1)
            /* AAC is playing... */
            if (p_audio_gbl_var->audio_aac.state != AUDIO_IDLE)
            {
              index_last_buffer = p_audio_gbl_var->audio_aac.index_last_buffer;
            }
          #endif

          if (index_last_buffer == 0xFF)
          {
            return (DATA_AVAILABLE);
          }
          else
          {
            /* it's the last buffer*/
            return (DATA_LAST);
          }
        }
      }
      else
      {
        return (SESSION_ERROR);
      }
    #else
      AUDIO_SEND_TRACE("L1_AUDIO_DRIVER not activated",RV_TRACE_LEVEL_ERROR);
    #endif
  }

  /*------------------------------------------------------------------*/
  /* Cust_get_pointer_notify                                          */
  /*------------------------------------------------------------------*/
  /*                                                                  */
  /* Parameters : session_id                                          */
  /*                                                                  */
  /* Return     : n/a                                                 */
  /*                                                                  */
  /* Description :                                                    */
  /* This function sends notification to upper layers to              */
  /* fill next buffer.                                                */
  /* This function should be used with Cust_get_pointer_next_buffer() */
  /*                                                                  */
  /*------------------------------------------------------------------*/
  void Cust_get_pointer_notify(UWORD8 session_id)
  {
    #if (L1_AUDIO_DRIVER == 1)
      T_AUDIO_DRIVER_NOTIFICATION *p_message;
          T_RVF_MB_STATUS mb_status;
      T_RV_RETURN return_path = {0, NULL};

      #if (L1_MP3 == 1)
        if (p_audio_gbl_var->audio_mp3.state != AUDIO_IDLE)
        {
          /* MP3 is playing... */
          return_path = p_audio_gbl_var->audio_driver_session[p_audio_gbl_var->audio_mp3.channel_id].session_req.return_path;
        }
      #endif
      #if (L1_AAC == 1)
        /* AAC is playing... */
        if (p_audio_gbl_var->audio_aac.state != AUDIO_IDLE)
        {
          return_path = p_audio_gbl_var->audio_driver_session[p_audio_gbl_var->audio_aac.channel_id].session_req.return_path;
        }
      #endif

      mb_status=rvf_get_buf(p_audio_gbl_var->mb_internal,
                            sizeof(T_AUDIO_DRIVER_NOTIFICATION),
                            (T_RVF_BUFFER **)(&p_message));

      if(mb_status==RVF_RED)
      {
        AUDIO_SEND_TRACE("AUDIO entity has no memory for driver notification",RV_TRACE_LEVEL_ERROR);
        return;
      }

      ((T_AUDIO_DRIVER_NOTIFICATION *)p_message)->header.msg_id=AUDIO_DRIVER_NOTIFICATION_MSG;

      #if (L1_MP3 == 1)
        if (p_audio_gbl_var->audio_mp3.state != AUDIO_IDLE)
        {
          /* MP3 is playing... */
          ((T_AUDIO_DRIVER_NOTIFICATION *)p_message)->channel_id=p_audio_gbl_var->audio_mp3.channel_id;
        }
      #endif
      #if (L1_AAC == 1)
        /* AAC is playing... */
        if (p_audio_gbl_var->audio_aac.state != AUDIO_IDLE)
        {
          ((T_AUDIO_DRIVER_NOTIFICATION *)p_message)->channel_id=p_audio_gbl_var->audio_aac.channel_id;
        }
      #endif
      ((T_AUDIO_DRIVER_NOTIFICATION *)p_message)->p_buffer=NULL;
      if(return_path.callback_func==NULL)
      {
        rvf_send_msg(return_path.addr_id, p_message);

        #if (L1_MP3 == 1)
          if (p_audio_gbl_var->audio_mp3.state != AUDIO_IDLE)
		  {
            p_audio_gbl_var->audio_mp3.buff_notify_msg = TRUE;
	      }
        #endif

        #if (L1_AAC == 1)
		  if (p_audio_gbl_var->audio_aac.state != AUDIO_IDLE)
		  {
		    p_audio_gbl_var->audio_aac.buff_notify_msg = TRUE;
		  }
        #endif

       }
      else
      {
        (*(return_path.callback_func))((void *)(p_message));
        rvf_free_buf((T_RVF_BUFFER *)p_message);
      }
  #else
    AUDIO_SEND_TRACE("L1_AUDIO_DRIVER not activated",RV_TRACE_LEVEL_ERROR);
  #endif
  }


  /*------------------------------------------------------------------*/
  /* Cust_get_next_buffer_status                                      */
  /*------------------------------------------------------------------*/
  /*                                                                  */
  /* Parameters :                                                     */
  /*                                                                  */
  /* Return     : status                                              */
  /*                                                                  */
  /* Description :                                                    */
  /* This function returns buffer status                              */
  /* fill next buffer.                                                */
  /* This function should be used with Cust_get_pointer_next_buffer() */
  /*                                                                  */
  /*------------------------------------------------------------------*/
UWORD8 Cust_get_next_buffer_status(void)
{

   #if (L1_MP3 == 1)
     if (p_audio_gbl_var->audio_mp3.state != AUDIO_IDLE)
     {
       return(p_audio_gbl_var->audio_mp3.buff_notify_msg);
     }
   #endif

   #if (L1_AAC == 1)
    if (p_audio_gbl_var->audio_aac.state != AUDIO_IDLE)
     {
       return(p_audio_gbl_var->audio_aac.buff_notify_msg);
     }
   #endif
return FALSE;
}


 #endif /* MP3 or AAC */
#endif /* RVM_AUDIO_MAIN_SWE */
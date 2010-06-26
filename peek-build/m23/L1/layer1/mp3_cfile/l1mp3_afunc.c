/************* Revision Controle System Header *************
 *                  GSM Layer 1 software
 * L1MP3_AFUNC.C
 *
 *        Filename l1mp3_afunc.c
 *  Copyright 2004 (C) Texas Instruments
 *
 ************* Revision Controle System Header *************/

#include "l1_confg.h"
#include "l1_types.h"
#include "sys_types.h"
#include "cust_os.h"
#include "l1_macro.h"
#include "l1_const.h"
#if TESTMODE
  #include "l1tm_defty.h"
#endif
#if (AUDIO_TASK == 1)
  #include "l1audio_const.h"
  #include "l1audio_cust.h"
  #include "l1audio_defty.h"
#endif
#if (L1_GTT == 1)
  #include "l1gtt_const.h"
  #include "l1gtt_defty.h"
#endif
#if (OP_RIV_AUDIO == 1)
  #include "rv/rv_general.h"
  #include "audio_api.h"
  #include "audio_structs_i.h"
  #include "audio_var_i.h"
  #include "audio_macro_i.h"
  #include "audio_const_i.h"
#endif
#if (L1_MP3 == 1)
  #include "l1mp3_const.h"
  #include "l1mp3_signa.h"
  #include "l1mp3_msgty.h"
  #include "l1mp3_proto.h"
  #include "l1mp3_defty.h"
#endif
#if (L1_MIDI == 1)
  #include "l1midi_defty.h"
#endif
#include "l1_defty.h"
#include "l1_msgty.h"
#include "l1_proto.h"

#if (L1_MP3 ==1)

// MP3 NDB API
extern T_MP3_MCU_DSP *mp3_ndb;

/*-------------------------------------------------------*/
/* l1a_mp3_send_confirmation()                           */
/*-------------------------------------------------------*/
/*                                                       */
/* Parameters : Signal Code of the message               */
/*                                                       */
/* Return     : n/a                                      */
/*                                                       */
/* Description : Send a confirmation message to the MMI  */
/*               for MP3 task.                           */
/*                                                       */
/*-------------------------------------------------------*/
void l1a_mp3_send_confirmation(UWORD32 SignalCode, UWORD32 error_code)
{
#if (OP_RIV_AUDIO == 1)

  void *p_message= NULL;
  T_RVF_MB_STATUS mb_status; //omaps00090550

  // Translate the l1 message into the Riviera compliant message
      // Allocate the Riviera buffer
mb_status=rvf_get_buf(p_audio_gbl_var->mb_internal,
                            sizeof(T_L1A_MP3_CON),
                            (T_RVF_BUFFER **) (&p_message));
  
  switch (SignalCode)
  {
    case MMI_MP3_START_CON:
    case MMI_MP3_STOP_CON:
    case MMI_MP3_PAUSE_CON:
    case MMI_MP3_RESUME_CON:
    case MMI_MP3_RESTART_CON:
    {
      // Translate the l1 message into the Riviera compliant message
      // Allocate the Riviera buffer
/*
      mb_status=rvf_get_buf(p_audio_gbl_var->mb_internal,
                            sizeof(T_L1A_MP3_CON),
                            (T_RVF_BUFFER **) (&p_message));
                            */

      // If insufficient resources, then report a memory error and abort
      if(mb_status==RVF_RED)
      {
        // the memory is insufficient to continue the non regression test
        AUDIO_SEND_TRACE("AUDIO entity has no memory for MMI_MP3_xxx_CON",RV_TRACE_LEVEL_ERROR);
        return;
      }

      // store error code
      ((T_L1A_MP3_CON *)p_message)->error_code=error_code;
    }

	
    break;

    default:
    {
      AUDIO_SEND_TRACE("AUDIO entity: wrong L1 confirmation message",RV_TRACE_LEVEL_ERROR);
      break;
    }
  }

  // Fill the message ID
  ((T_RV_HDR *)p_message)->msg_id=SignalCode;

  // send the messsage to the audio entity
  rvf_send_msg(p_audio_gbl_var->addrId,p_message);

#else // OP_RIV_AUDIO

  xSignalHeaderRec *msg;

  msg=os_alloc_sig(sizeof(T_L1A_MP3_CON));
  DEBUGMSG(status,NU_ALLOC_ERR)
  msg->SignalCode = SignalCode;
  ((T_L1A_MP3_CON *)(msg->SigP))->error_code=error_code;

  #if (TRACE_TYPE==1) || (TRACE_TYPE==4)
    l1_trace_message(msg);
  #endif

  os_send_sig(msg,MMI_QUEUE);
  DEBUGMSG(status,NU_SEND_QUEUE_ERR)

#endif // OP_RIV_AUDIO!=1
}

/*-------------------------------------------------------*/
/* l1a_mp3_send_header_info()                            */
/*-------------------------------------------------------*/
/*                                                       */
/* Parameters : n/a                                      */
/*                                                       */
/* Return     : n/a                                      */
/*                                                       */
/* Description : Send a confirmation message to the MMI  */
/*               for MP3 task.                           */
/*                                                       */
/*-------------------------------------------------------*/
void l1a_mp3_send_header_info()
{
#if (OP_RIV_AUDIO == 1)

  void *p_message;
  T_RVF_MB_STATUS mb_status; //omaps00090550

  UWORD32 header=(mp3_ndb->d_mp3_api_header[0]<<16)+mp3_ndb->d_mp3_api_header[1];

  // Translate the l1 message into the Riviera compliant message
  // Allocate the Riviera buffer
  mb_status=rvf_get_buf(p_audio_gbl_var->mb_internal,
                        sizeof(T_L1A_MP3_INFO_CON),
                        (T_RVF_BUFFER **)(&p_message));

  // If insufficient resources, then report a memory error and abort
  if(mb_status==RVF_RED)
  {
    // the memory is insufficient to continue the non regression test
    AUDIO_SEND_TRACE("AUDIO entity has no memory for MMI_MP3_INFO_CON",RV_TRACE_LEVEL_ERROR);
    return;
  }

  switch(l1mp3_get_frequencyrate(header))
  {
    case 48000:
      ((T_L1A_MP3_INFO_CON *)p_message)->frequency=C_MP3_HEADER_FREQ_48000;
    break;
    case 44100:
      ((T_L1A_MP3_INFO_CON *)p_message)->frequency=C_MP3_HEADER_FREQ_44100;
    break;
    case 32000:
      ((T_L1A_MP3_INFO_CON *)p_message)->frequency=C_MP3_HEADER_FREQ_32000;
    break;
    case 24000:
      ((T_L1A_MP3_INFO_CON *)p_message)->frequency=C_MP3_HEADER_FREQ_24000;
    break;
    case 22050:
      ((T_L1A_MP3_INFO_CON *)p_message)->frequency=C_MP3_HEADER_FREQ_22050;
    break;
    case 16000:
      ((T_L1A_MP3_INFO_CON *)p_message)->frequency=C_MP3_HEADER_FREQ_16000;
    break;
    case 12000:
      ((T_L1A_MP3_INFO_CON *)p_message)->frequency=C_MP3_HEADER_FREQ_12000;
    break;
    case 11025:
      ((T_L1A_MP3_INFO_CON *)p_message)->frequency=C_MP3_HEADER_FREQ_11025;
    break;
    case 8000:
      ((T_L1A_MP3_INFO_CON *)p_message)->frequency=C_MP3_HEADER_FREQ_8000;
    break;
    default:
      ((T_L1A_MP3_INFO_CON *)p_message)->frequency=C_MP3_HEADER_ERROR;
    break;
  }

  switch(l1mp3_get_bitrate(header))
  {
    case 320:
      ((T_L1A_MP3_INFO_CON *)p_message)->bitrate=C_MP3_HEADER_BITRATE_320;
    break;
    case 256:
      ((T_L1A_MP3_INFO_CON *)p_message)->bitrate=C_MP3_HEADER_BITRATE_256;
    break;
    case 224:
      ((T_L1A_MP3_INFO_CON *)p_message)->bitrate=C_MP3_HEADER_BITRATE_224;
    break;
    case 192:
      ((T_L1A_MP3_INFO_CON *)p_message)->bitrate=C_MP3_HEADER_BITRATE_192;
    break;
    case 160:
      ((T_L1A_MP3_INFO_CON *)p_message)->bitrate=C_MP3_HEADER_BITRATE_160;
    break;
    case 128:
      ((T_L1A_MP3_INFO_CON *)p_message)->bitrate=C_MP3_HEADER_BITRATE_128;
    break;
    case 112:
      ((T_L1A_MP3_INFO_CON *)p_message)->bitrate=C_MP3_HEADER_BITRATE_112;
    break;
    case 96:
      ((T_L1A_MP3_INFO_CON *)p_message)->bitrate=C_MP3_HEADER_BITRATE_96;
    break;
    case 80:
      ((T_L1A_MP3_INFO_CON *)p_message)->bitrate=C_MP3_HEADER_BITRATE_80;
    break;
    case 64:
      ((T_L1A_MP3_INFO_CON *)p_message)->bitrate=C_MP3_HEADER_BITRATE_64;
    break;
    case 56:
      ((T_L1A_MP3_INFO_CON *)p_message)->bitrate=C_MP3_HEADER_BITRATE_56;
    break;
    case 48:
      ((T_L1A_MP3_INFO_CON *)p_message)->bitrate=C_MP3_HEADER_BITRATE_48;
    break;
    case 40:
      ((T_L1A_MP3_INFO_CON *)p_message)->bitrate=C_MP3_HEADER_BITRATE_40;
    break;
    case 32:
      ((T_L1A_MP3_INFO_CON *)p_message)->bitrate=C_MP3_HEADER_BITRATE_32;
    break;
    default:
      ((T_L1A_MP3_INFO_CON *)p_message)->bitrate=C_MP3_HEADER_ERROR;
    break;
  }

  ((T_L1A_MP3_INFO_CON *)p_message)->mpeg_id  =l1mp3_get_mpeg_id(header);
  ((T_L1A_MP3_INFO_CON *)p_message)->layer    =l1mp3_get_layer(header);
  ((T_L1A_MP3_INFO_CON *)p_message)->padding  =l1mp3_get_padding(header);
  ((T_L1A_MP3_INFO_CON *)p_message)->private  =l1mp3_get_private(header);
  ((T_L1A_MP3_INFO_CON *)p_message)->channel  =l1mp3_get_channel_mode(header);
  ((T_L1A_MP3_INFO_CON *)p_message)->copyright=l1mp3_get_copyright(header);
  ((T_L1A_MP3_INFO_CON *)p_message)->original =l1mp3_get_original(header);
  ((T_L1A_MP3_INFO_CON *)p_message)->emphasis =l1mp3_get_emphasis(header);

  // Fill the message ID
  ((T_RV_HDR *)p_message)->msg_id=MMI_MP3_INFO_CON;

  // send the messsage to the audio entity
  rvf_send_msg(p_audio_gbl_var->addrId,p_message);

#else // OP_RIV_AUDIO

  xSignalHeaderRec *msg;
  UWORD32 header=(mp3_ndb->d_mp3_api_header[0]<<16)+mp3_ndb->d_mp3_api_header[1];

  msg=os_alloc_sig(sizeof(T_L1A_MP3_INFO_CON));
  DEBUGMSG(status,NU_ALLOC_ERR)
  msg->SignalCode = MMI_MP3_INFO_CON;
  switch(l1mp3_get_frequencyrate(header))
  {
    case 48000:
      ((T_L1A_MP3_INFO_CON *)(msg->SigP))->frequency=C_MP3_HEADER_FREQ_48000;
    break;
    case 44100:
      ((T_L1A_MP3_INFO_CON *)(msg->SigP))->frequency=C_MP3_HEADER_FREQ_44100;
    break;
    case 32000:
      ((T_L1A_MP3_INFO_CON *)(msg->SigP))->frequency=C_MP3_HEADER_FREQ_32000;
    break;
    case 24000:
      ((T_L1A_MP3_INFO_CON *)(msg->SigP))->frequency=C_MP3_HEADER_FREQ_24000;
    break;
    case 22050:
      ((T_L1A_MP3_INFO_CON *)(msg->SigP))->frequency=C_MP3_HEADER_FREQ_22050;
    break;
    case 16000:
      ((T_L1A_MP3_INFO_CON *)(msg->SigP))->frequency=C_MP3_HEADER_FREQ_16000;
    break;
    case 12000:
      ((T_L1A_MP3_INFO_CON *)(msg->SigP))->frequency=C_MP3_HEADER_FREQ_12000;
    break;
    case 11025:
      ((T_L1A_MP3_INFO_CON *)(msg->SigP))->frequency=C_MP3_HEADER_FREQ_11025;
    break;
    case 8000:
      ((T_L1A_MP3_INFO_CON *)(msg->SigP))->frequency=C_MP3_HEADER_FREQ_8000;
    break;
    default:
      ((T_L1A_MP3_INFO_CON *)(msg->SigP))->frequency=C_MP3_HEADER_ERROR;
    break;
  }

  switch(l1mp3_get_bitrate(header))
  {
    case 320:
      ((T_L1A_MP3_INFO_CON *)(msg->SigP))->bitrate=C_MP3_HEADER_BITRATE_320;
    break;
    case 256:
      ((T_L1A_MP3_INFO_CON *)(msg->SigP))->bitrate=C_MP3_HEADER_BITRATE_256;
    break;
    case 224:
      ((T_L1A_MP3_INFO_CON *)(msg->SigP))->bitrate=C_MP3_HEADER_BITRATE_224;
    break;
    case 192:
      ((T_L1A_MP3_INFO_CON *)(msg->SigP))->bitrate=C_MP3_HEADER_BITRATE_192;
    break;
    case 160:
      ((T_L1A_MP3_INFO_CON *)(msg->SigP))->bitrate=C_MP3_HEADER_BITRATE_160;
    break;
    case 128:
      ((T_L1A_MP3_INFO_CON *)(msg->SigP))->bitrate=C_MP3_HEADER_BITRATE_128;
    break;
    case 112:
      ((T_L1A_MP3_INFO_CON *)(msg->SigP))->bitrate=C_MP3_HEADER_BITRATE_112;
    break;
    case 96:
      ((T_L1A_MP3_INFO_CON *)(msg->SigP))->bitrate=C_MP3_HEADER_BITRATE_96;
    break;
    case 80:
      ((T_L1A_MP3_INFO_CON *)(msg->SigP))->bitrate=C_MP3_HEADER_BITRATE_80;
    break;
    case 64:
      ((T_L1A_MP3_INFO_CON *)(msg->SigP))->bitrate=C_MP3_HEADER_BITRATE_64;
    break;
    case 56:
      ((T_L1A_MP3_INFO_CON *)(msg->SigP))->bitrate=C_MP3_HEADER_BITRATE_56;
    break;
    case 48:
      ((T_L1A_MP3_INFO_CON *)(msg->SigP))->bitrate=C_MP3_HEADER_BITRATE_48;
    break;
    case 40:
      ((T_L1A_MP3_INFO_CON *)(msg->SigP))->bitrate=C_MP3_HEADER_BITRATE_40;
    break;
    case 32:
      ((T_L1A_MP3_INFO_CON *)(msg->SigP))->bitrate=C_MP3_HEADER_BITRATE_32;
    break;
    default:
      ((T_L1A_MP3_INFO_CON *)(msg->SigP))->bitrate=C_MP3_HEADER_ERROR;
    break;
  }

  ((T_L1A_MP3_INFO_CON *)(msg->SigP))->mpeg_id  =l1mp3_get_mpeg_id(header);
  ((T_L1A_MP3_INFO_CON *)(msg->SigP))->layer    =l1mp3_get_layer(header);
  ((T_L1A_MP3_INFO_CON *)(msg->SigP))->padding  =l1mp3_get_padding(header);
  ((T_L1A_MP3_INFO_CON *)(msg->SigP))->private  =l1mp3_get_private(header);
  ((T_L1A_MP3_INFO_CON *)(msg->SigP))->channel  =l1mp3_get_channel_mode(header);
  ((T_L1A_MP3_INFO_CON *)(msg->SigP))->copyright=l1mp3_get_copyright(header);
  ((T_L1A_MP3_INFO_CON *)(msg->SigP))->original =l1mp3_get_original(header);
  ((T_L1A_MP3_INFO_CON *)(msg->SigP))->emphasis =l1mp3_get_emphasis(header);

  #if (TRACE_TYPE==1) || (TRACE_TYPE==4)
    l1_trace_message(msg);
  #endif

  os_send_sig(msg,MMI_QUEUE);
  DEBUGMSG(status,NU_SEND_QUEUE_ERR)

#endif // OP_RIV_AUDIO!=1
}


#endif    // L1_MP3

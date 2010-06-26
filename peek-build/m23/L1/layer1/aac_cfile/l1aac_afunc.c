/************* Revision Controle System Header *************
 *                  GSM Layer 1 software
 * L1AAC_AFUNC.C
 *
 *        Filename l1aac_afunc.c
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
#if (L1_DYN_DSP_DWNLD == 1)
  #include "l1_dyn_dwl_const.h"
  #include "l1_dyn_dwl_defty.h"
#endif
#if (L1_AAC == 1)
  #include "l1aac_const.h"
  #include "l1aac_signa.h"
  #include "l1aac_defty.h"
  #include "l1aac_msgty.h"
  #include "l1aac_proto.h"
#endif
#if (L1_MIDI == 1)
  #include "l1midi_defty.h"
#endif
#if (L1_MP3 == 1)
  #include "l1mp3_defty.h"
#endif
#include "l1_rf61.h"
#include "l1_defty.h"
#include "l1_msgty.h"
#include "l1_proto.h"
#include "l1_varex.h"

#if (L1_AAC ==1)

// AAC NDB API
extern T_AAC_MCU_DSP *aac_ndb;

/*-------------------------------------------------------*/
/* l1a_aac_send_confirmation()                           */
/*-------------------------------------------------------*/
/*                                                       */
/* Parameters : Signal Code of the message               */
/*                                                       */
/* Return     : n/a                                      */
/*                                                       */
/* Description : Send a confirmation message to the MMI  */
/*               for AAC task.                           */
/*                                                       */
/*-------------------------------------------------------*/
void l1a_aac_send_confirmation(UWORD32 SignalCode, UWORD16 aac_format, UWORD32 error_code)
{
#if (OP_RIV_AUDIO == 1)

  void *p_message = NULL;
  T_RVF_MB_STATUS mb_status;//omaps00090550

  switch (SignalCode)
  {
    case MMI_AAC_START_CON:
    {
//Verify here- AAC
      // Translate the l1 message into the Riviera compliant message
      // Allocate the Riviera buffer
      mb_status=rvf_get_buf(p_audio_gbl_var->mb_internal,
                            sizeof(T_L1A_AAC_START_CON),
                            (T_RVF_BUFFER **) (&p_message));

      // If insufficient resources, then report a memory error and abort
      if(mb_status==RVF_RED)
      {
        // the memory is insufficient to continue the non regression test
        AUDIO_SEND_TRACE("AUDIO entity has no memory for MMI_AAC_START_CON",RV_TRACE_LEVEL_ERROR);
        return;
      }

      // store AAC stream format (ADIF/ADTS)
      ((T_L1A_AAC_START_CON *)p_message)->aac_format=aac_format;
    }
    break;
//Till this.. might not be needed
    case MMI_AAC_STOP_CON:
    case MMI_AAC_PAUSE_CON:
    case MMI_AAC_RESUME_CON:
    case MMI_AAC_RESTART_CON:
    {
      // Translate the l1 message into the Riviera compliant message
      // Allocate the Riviera buffer
      mb_status=rvf_get_buf(p_audio_gbl_var->mb_internal,
                            sizeof(T_L1A_AAC_CON),
                            (T_RVF_BUFFER **) (&p_message));

      // If insufficient resources, then report a memory error and abort
      if(mb_status==RVF_RED)
      {
        // the memory is insufficient to continue the non regression test
        AUDIO_SEND_TRACE("AUDIO entity has no memory for MMI_AAC_xxx_CON",RV_TRACE_LEVEL_ERROR);
        return;
      }

      // store error code
      ((T_L1A_AAC_CON *)p_message)->error_code=error_code;
    }
    break;

    default:
    {
      AUDIO_SEND_TRACE("AUDIO entity: wrong L1 confirmation message",RV_TRACE_LEVEL_ERROR);
      break;
    }
  }

  // Fill the message ID
  if(p_message != NULL)
  ((T_RV_HDR *)p_message)->msg_id=SignalCode;

  // send the messsage to the audio entity
  rvf_send_msg(p_audio_gbl_var->addrId,p_message);

#else // OP_RIV_AUDIO

  xSignalHeaderRec *msg;

  switch (SignalCode)
  {
    case MMI_AAC_START_CON:
    {
      msg=os_alloc_sig(sizeof(T_L1A_AAC_START_CON));
      DEBUGMSG(status,NU_ALLOC_ERR)
      msg->SignalCode = SignalCode;
      ((T_L1A_AAC_START_CON *)(msg->SigP))->aac_format=aac_format;
    }
    break;

    case MMI_AAC_STOP_CON:
    case MMI_AAC_PAUSE_CON:
    case MMI_AAC_RESUME_CON:
    case MMI_AAC_RESTART_CON:
    {
      msg=os_alloc_sig(sizeof(T_L1A_AAC_CON));
      DEBUGMSG(status,NU_ALLOC_ERR)
      msg->SignalCode = SignalCode;
      ((T_L1A_AAC_CON *)(msg->SigP))->error_code=error_code;
    }
  }

  #if (TRACE_TYPE==1) || (TRACE_TYPE==4)
    l1_trace_message(msg);
  #endif

  os_send_sig(msg,MMI_QUEUE);
  DEBUGMSG(status,NU_SEND_QUEUE_ERR)

#endif // OP_RIV_AUDIO!=1
}

/*-------------------------------------------------------*/
/* l1a_aac_send_stream_info()                            */
/*-------------------------------------------------------*/
/*                                                       */
/* Parameters : n/a                                      */
/*                                                       */
/* Return     : n/a                                      */
/*                                                       */
/* Description : Send a confirmation message to the MMI  */
/*               for AAC task.                           */
/*                                                       */
/*-------------------------------------------------------*/
void l1a_aac_send_stream_info()
{
#if (OP_RIV_AUDIO == 1)

  void *p_message;
  T_RVF_MB_STATUS mb_status;//omaps00090550

  // Information fields
  UWORD16 aac_format=l1a_apihisr_com.aac.aac_format;
  UWORD16 freq_index=l1a_apihisr_com.aac.frequency_index;
  UWORD16 channels  =l1a_apihisr_com.aac.channels;
  UWORD32 bitrate   =l1a_apihisr_com.aac.bitrate;

  // Translate the l1 message into the Riviera compliant message
  // Allocate the Riviera buffer
  mb_status=rvf_get_buf(p_audio_gbl_var->mb_internal,
                        sizeof(T_L1A_AAC_INFO_CON),
                        (T_RVF_BUFFER **)(&p_message));

  // If insufficient resources, then report a memory error and abort
  if(mb_status==RVF_RED)
  {
    // the memory is insufficient to continue the non regression test
    AUDIO_SEND_TRACE("AUDIO entity has no memory for MMI_AAC_INFO_CON",RV_TRACE_LEVEL_ERROR);
    return;
  }

  switch(l1aac_get_frequencyrate(freq_index))
  {
    case 48000:
      ((T_L1A_AAC_INFO_CON *)p_message)->frequency=C_AAC_STREAM_FREQ_48000;
    break;
    case 44100:
      ((T_L1A_AAC_INFO_CON *)p_message)->frequency=C_AAC_STREAM_FREQ_44100;
    break;
    case 32000:
      ((T_L1A_AAC_INFO_CON *)p_message)->frequency=C_AAC_STREAM_FREQ_32000;
    break;
    case 24000:
      ((T_L1A_AAC_INFO_CON *)p_message)->frequency=C_AAC_STREAM_FREQ_24000;
    break;
    case 22050:
      ((T_L1A_AAC_INFO_CON *)p_message)->frequency=C_AAC_STREAM_FREQ_22050;
    break;
    case 16000:
      ((T_L1A_AAC_INFO_CON *)p_message)->frequency=C_AAC_STREAM_FREQ_16000;
    break;
    case 12000:
      ((T_L1A_AAC_INFO_CON *)p_message)->frequency=C_AAC_STREAM_FREQ_12000;
    break;
    case 11025:
      ((T_L1A_AAC_INFO_CON *)p_message)->frequency=C_AAC_STREAM_FREQ_11025;
    break;
    case 8000:
      ((T_L1A_AAC_INFO_CON *)p_message)->frequency=C_AAC_STREAM_FREQ_8000;
    break;
    default:
      ((T_L1A_AAC_INFO_CON *)p_message)->frequency=C_AAC_STREAM_ERROR;
    break;
  }

  // The 32-bit bitrate is passed directly to upper layers. The DSP can return
  // a non-standard bitrate if it is present in ADIF header. If we need to pass
  // only standard bitrates to upper layers, we need to have a lookup table and
  // then quantize the bitrate and then send it across. This need to be discussed
  // with Fabrice
  ((T_L1A_AAC_INFO_CON *)p_message)->bitrate=bitrate;

  // Number of channels in the AAC stream
  switch(channels)
  {
    case 1:
      ((T_L1A_AAC_INFO_CON *)p_message)->channel=C_AAC_STREAM_MONO;
      break;

    case 2:
      ((T_L1A_AAC_INFO_CON *)p_message)->channel=C_AAC_STREAM_STEREO;
      break;

    default:
      ((T_L1A_AAC_INFO_CON *)p_message)->channel=C_AAC_STREAM_ERROR;
    break;
  }

  // AAC stream format (ADTS/ADIF)
  switch(aac_format)
  {
    case 0:
      ((T_L1A_AAC_INFO_CON *)p_message)->aac_format=C_AAC_ADIF_FORMAT;
      break;

    case 1:
      ((T_L1A_AAC_INFO_CON *)p_message)->aac_format=C_AAC_ADTS_FORMAT;
      break;

    default:
      ((T_L1A_AAC_INFO_CON *)p_message)->aac_format=C_AAC_STREAM_ERROR;
    break;
  }

  // We can add more information fields like profile, copyright etc

  // Fill the message ID
  ((T_RV_HDR *)p_message)->msg_id=MMI_AAC_INFO_CON;

  // send the messsage to the audio entity
  rvf_send_msg(p_audio_gbl_var->addrId,p_message);

#else // OP_RIV_AUDIO

  xSignalHeaderRec *msg;
  // UWORD32 header=(mp3_ndb->d_mp3_api_header[0]<<16)+mp3_ndb->d_mp3_api_header[1];

  // Information fields
  UWORD16 aac_format=l1a_apihisr_com.aac.aac_format;
  UWORD16 freq_index=l1a_apihisr_com.aac.frequency_index;
  UWORD16 channels  =l1a_apihisr_com.aac.channels;
  UWORD32 bitrate   =l1a_apihisr_com.aac.bitrate;

  msg=os_alloc_sig(sizeof(T_L1A_AAC_INFO_CON));
  DEBUGMSG(status,NU_ALLOC_ERR)
  msg->SignalCode = MMI_AAC_INFO_CON;

  switch(l1aac_get_frequencyrate(freq_index))
  {
    case 48000:
      ((T_L1A_AAC_INFO_CON *)(msg->SigP))->frequency=C_AAC_STREAM_FREQ_48000;
    break;
    case 44100:
      ((T_L1A_AAC_INFO_CON *)(msg->SigP))->frequency=C_AAC_STREAM_FREQ_44100;
    break;
    case 32000:
      ((T_L1A_AAC_INFO_CON *)(msg->SigP))->frequency=C_AAC_STREAM_FREQ_32000;
    break;
    case 24000:
      ((T_L1A_AAC_INFO_CON *)(msg->SigP))->frequency=C_AAC_STREAM_FREQ_24000;
    break;
    case 22050:
      ((T_L1A_AAC_INFO_CON *)(msg->SigP))->frequency=C_AAC_STREAM_FREQ_22050;
    break;
    case 16000:
      ((T_L1A_AAC_INFO_CON *)(msg->SigP))->frequency=C_AAC_STREAM_FREQ_16000;
    break;
    case 12000:
      ((T_L1A_AAC_INFO_CON *)(msg->SigP))->frequency=C_AAC_STREAM_FREQ_12000;
    break;
    case 11025:
      ((T_L1A_AAC_INFO_CON *)(msg->SigP))->frequency=C_AAC_STREAM_FREQ_11025;
    break;
    case 8000:
      ((T_L1A_AAC_INFO_CON *)(msg->SigP))->frequency=C_AAC_STREAM_FREQ_8000;
    break;
    default:
      ((T_L1A_AAC_INFO_CON *)(msg->SigP))->frequency=C_AAC_STREAM_ERROR;
    break;
  }

  // The 32-bit bitrate is passed directly to upper layers. The DSP can return
  // a non-standard bitrate if it is present in ADIF header. If we need to pass
  // only standard bitrates to upper layers, we need to have a lookup table and
  // then quantize the bitrate and then send it across. This need to be discussed
  // with Fabrice
  ((T_L1A_AAC_INFO_CON *)(msg->SigP))->bitrate=bitrate;

  // Number of channels in the AAC stream
  switch(channels)
  {
    case 1:
      ((T_L1A_AAC_INFO_CON *)(msg->SigP))->channel=C_AAC_STREAM_MONO;
      break;

    case 2:
      ((T_L1A_AAC_INFO_CON *)(msg->SigP))->channel=C_AAC_STREAM_STEREO;
      break;

    default:
      ((T_L1A_AAC_INFO_CON *)(msg->SigP))->channel=C_AAC_STREAM_ERROR;
    break;
  }

  // AAC stream format (ADTS/ADIF)
  switch(aac_format)
  {
    case 0:
      ((T_L1A_AAC_INFO_CON *)(msg->SigP))->aac_format=C_AAC_ADIF_FORMAT;
      break;

    case 1:
      ((T_L1A_AAC_INFO_CON *)(msg->SigP))->aac_format=C_AAC_ADTS_FORMAT;
      break;

    default:
      ((T_L1A_AAC_INFO_CON *)(msg->SigP))->aac_format=C_AAC_STREAM_ERROR;
    break;
  }

  // We can add more information fields like profile, copyright etc

  #if (TRACE_TYPE==1) || (TRACE_TYPE==4)
    l1_trace_message(msg);
  #endif

  os_send_sig(msg,MMI_QUEUE);
  DEBUGMSG(status,NU_SEND_QUEUE_ERR)

#endif // OP_RIV_AUDIO!=1
}


#endif    // L1_AAC

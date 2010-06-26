/************* Revision Controle System Header *************
 *                  GSM Layer 1 software
 * L1AAC_ASYNC.C
 *
 *        Filename l1aac_async.c
 *  Copyright 2004 (C) Texas Instruments
 *
 ************* Revision Controle System Header *************/
#include <stdio.h>
#include <string.h>

#include "nucleus.h"
#include "l1_confg.h"
#include "l1_types.h"
#include "sys_types.h"
#include "cust_os.h"
#if (AUDIO_TASK == 1)
  #include "l1audio_signa.h"
  #include "l1audio_const.h"
  #include "l1audio_cust.h"
  #include "l1audio_defty.h"
#endif
#include "l1_const.h"
#if TESTMODE
  #include "l1tm_defty.h"
#endif
#if (L1_GTT == 1)
  #include "l1gtt_const.h"
  #include "l1gtt_defty.h"
#endif
#if (OP_RIV_AUDIO == 1)
  #include "rv/rv_general.h"
#endif
#if (L1_DYN_DSP_DWNLD == 1)
  #include "l1_dyn_dwl_const.h"
  #include "l1_dyn_dwl_signa.h"
  #include "l1_dyn_dwl_defty.h"
#endif
#if (L1_AAC == 1)
  #include "l1aac_defty.h"
  #include "l1aac_msgty.h"
  #include "l1aac_const.h"
  #include "l1aac_signa.h"
  #include "l1aac_error.h"
  #include "l1aac_proto.h"
#endif
#if (L1_MIDI == 1)
  #include "l1midi_defty.h"
#endif
#if (L1_MP3 == 1)
  #include "l1mp3_defty.h"
#endif
#if (L1_DYN_DSP_DWNLD == 1)
#include "l1_dyn_dwl_signa.h"
#endif
#include "l1_rf61.h"
#include "l1_defty.h"
#include "l1_varex.h"
#include "l1_trace.h"
#include "sys_dma.h"

#if (L1_DYN_DSP_DWNLD == 1)
#if (CODE_VERSION == SIMULATION)
  extern void trace_fct_simu_dyn_dwnld(CHAR *fct_name);
#endif // CODE_VERSION == SIMULATION
#endif // L1_DYN_DSP_DWNLD == 1

#if(L1_BT_AUDIO ==1)
extern T_L1_BT_AUDIO bt_audio;
extern void l1_audio_bt_init(UINT16 media_buf_size);
#endif
#if (L1_AAC == 1)

// AAC NDB API
T_AAC_MCU_DSP *aac_ndb;
#if (CODE_VERSION == SIMULATION)
  T_AAC_MCU_DSP aac_ndb_sim;
#endif

// AAC decoded output buffer size (in words)
#define C_AAC_OUTPUT_BUFFER_SIZE (2048)
#if (OP_L1_STANDALONE == 1)
// AAC DMA ping-pong buffers
#if (CODE_VERSION == NOT_SIMULATION)
#pragma DATA_SECTION(aac_dma_struct,".l1s_global")
#endif
T_AAC_DMA_PARAM aac_dma_struct;
#endif

UWORD16  a_aac_dma_input_buffer[2][C_AAC_OUTPUT_BUFFER_SIZE];//L1_BT

T_AAC_DMA_PARAM *aac_dma;
// Index for DMA ping-pong buffers
UWORD8   d_aac_dma_current_buffer_id;
extern UWORD8 dma_src_port;
#if (OP_L1_STANDALONE == 0)
extern void * mem_Allocate(UWORD16 size);
extern void    mem_Deallocate(void *memoryBlock);
#endif
/*-------------------------------------------------------*/
/* l1a_mmi_aac_process()                                 */
/*-------------------------------------------------------*/
/*                                                       */
/* Description:                                          */
/* ------------                                          */
/* This function is a state machine which handles the    */
/* AAC feature.                                          */
/*                                                       */
/* Starting messages:        MMI_AAC_START_REQ           */
/*                                                       */
/* Result messages (input):  L1_AAC_ENABLE_CON           */
/*                           L1_AAC_DISABLE_CON          */
/*                                                       */
/* Result messages (output): MMI_AAC_START_CON           */
/*                                                       */
/* Reset messages (input):   none                        */
/*                                                       */
/* Stop message (input):     MMI_AAC_STOP_REQ            */
/*                                                       */
/* Stop message (output):    MMI_AAC_STOP_CON            */
/*                                                       */
/* Rem:                                                  */
/* ----                                                  */
/*                                                       */
/*-------------------------------------------------------*/
#if (L1_DYN_DSP_DWNLD == 1)
void l1a_mmi_aac_process(xSignalHeaderRec *msg)
{
  enum states
  {
    RESET=0,
    WAIT_START_REQ,
    WAIT_DYN_DWNLD,
    AAC_INIT,
    AAC_PLAY,
    WAIT_PAUSE_CON,
    AAC_PAUSE,
    WAIT_RESUME_CON,
    WAIT_STOP
  };

  UWORD8  *state     = &l1a.state[L1A_AAC_STATE];
  UWORD32 SignalCode = msg->SignalCode;

  BOOL end_process = 0;

  while(!end_process)
  {
    switch(*state)
    {
      // *********
      // * RESET *
      // *********
      case RESET:
      {
        // Reset everything
        l1aac_reset();

        // AAC HISR mustn't be activated
        l1a_apihisr_com.aac.running=FALSE;

        // Change state
        *state=WAIT_START_REQ;
      }
      break;

      // ******************
      // * WAIT_START_REQ *
      // ******************
      case WAIT_START_REQ:
      {
        // *----------------------------*
        // * Start command from the MMI *
        // *----------------------------*
        if(SignalCode==MMI_AAC_START_REQ)
        {
          // Must we have loopback or not?
          l1a_l1s_com.aac_task.parameters.loopback  =((T_MMI_AAC_REQ *)(msg->SigP))->loopback;

          // Is the output speaker stereo or mono?
          l1a_l1s_com.aac_task.parameters.stereo    =((T_MMI_AAC_REQ *)(msg->SigP))->stereo;

          // Store the session id
          l1a_l1s_com.aac_task.parameters.session_id=((T_MMI_AAC_REQ *)(msg->SigP))->session_id;

          // Store the DMA channel number
          l1a_l1s_com.aac_task.parameters.dma_channel_number=((T_MMI_AAC_REQ *)(msg->SigP))->dma_channel_number;

          // Reset AAC error code
          l1a.aac_task.error_code=C_AAC_ERR_NONE;

          l1a.aac_task.stereopath_init_pending=FALSE;
#if (OP_L1_STANDALONE == 1)
          aac_dma=&aac_dma_struct;
	        dma_src_port=AUDIO_SP_SOURCE_IMIF;
       #else
aac_dma=mem_Allocate(sizeof(T_AAC_DMA_PARAM));
	 
	   if(aac_dma==NULL)
	          {
		 	l1a_aac_send_confirmation(MMI_AAC_STOP_CON,0,0);
                   end_process = 1;
          	   *state=RESET;
		   return;
            }
	     #endif
	#if(L1_BT_AUDIO == 1)
	  if(bt_audio.connected_status == TRUE)
         l1_audio_bt_init(C_AAC_OUTPUT_BUFFER_SIZE);
	 #endif

          if (l1a.dyn_dwnld.semaphore_vect[AAC_STATE_MACHINE]==GREEN)
          {
             // WARNING: code below must be duplicated in WAIT_DYN_DWNLD state
            // Start the L1S AAC task

            #if ((TRACE_TYPE==1) || (TRACE_TYPE == 4))
               // Disable trace DSP upon AAC activation
              l1_disable_dsp_trace();
            #endif
			 
            // Tell the L1S to start the DSP background task
            l1a_l1s_com.aac_task.command.start=TRUE;

            // AAC can be activated
            l1a_apihisr_com.aac.running=TRUE;

            // Change state
            *state=AAC_INIT;
          }
          else
          {
            // Change state
            *state = WAIT_DYN_DWNLD;

#if ((TRACE_TYPE == 1) || (TRACE_TYPE == 4) || (TRACE_TYPE == 5))
            if((trace_info.current_config->l1_dyn_trace) & (1<<L1_DYN_TRACE_DYN_DWNLD))
            {
              char str[30];
              sprintf(str,"AAC SM blocked\r\n");
#if(CODE_VERSION == SIMULATION)
              trace_fct_simu_dyn_dwnld(str);
#else
              rvt_send_trace_cpy((T_RVT_BUFFER)str,trace_info.l1_trace_user_id,strlen(str),RVT_ASCII_FORMAT);
#endif // CODE_VERSION == SIMULATION
            }
#endif    // (TRACE_TYPE == 1) || (TRACE_TYPE == 4)
          }

        }

        end_process = 1;
      }
      break;

      case WAIT_DYN_DWNLD:
      {
        if((SignalCode==API_L1_DYN_DWNLD_FINISHED) && (l1a.dyn_dwnld.semaphore_vect[AAC_STATE_MACHINE] == GREEN))
        {

          #if ((TRACE_TYPE==1) || (TRACE_TYPE == 4))
            // Disable trace DSP upon AAC activation
            l1_disable_dsp_trace();
          #endif

		
          // Tell the L1S to start the DSP background task
          l1a_l1s_com.aac_task.command.start=TRUE;

          // AAC can be activated
          l1a_apihisr_com.aac.running=TRUE;

          // Change state
          *state=AAC_INIT;

#if ((TRACE_TYPE == 1) || (TRACE_TYPE == 4) || (TRACE_TYPE == 5))
          if((trace_info.current_config->l1_dyn_trace) & (1<<L1_DYN_TRACE_DYN_DWNLD))
          {
            char str[32];
            sprintf(str,"AAC SM un-blocked\r\n");
#if(CODE_VERSION == SIMULATION)
            trace_fct_simu_dyn_dwnld(str);
#else
            rvt_send_trace_cpy((T_RVT_BUFFER)str,trace_info.l1_trace_user_id,strlen(str),RVT_ASCII_FORMAT);
#endif
          }
#endif    // (TRACE_TYPE == 1) || (TRACE_TYPE == 4)
        }
        end_process = 1;
      }
      break;

      // ************
      // * AAC_INIT *
      // ************
      case AAC_INIT:
      {
        // *------------------------------------------------------------*
        // * L1S confirmation of DSP AAC background task initialization *
        // *------------------------------------------------------------*
        if(SignalCode==L1_AAC_ENABLE_CON)
        {
          // Request DSP AAC start to API HISR
          l1a_apihisr_com.aac.command.start=TRUE;

#if (OP_RIV_AUDIO == 1)
          {
            // WARNING: temporary until os_activate_hisr() is declared in L3 functions
            extern NU_HISR apiHISR;
            NU_Activate_HISR(&apiHISR);
          }
#else
          os_activate_hisr(API_HISR);
#endif
        }

        // *-----------------------*
        // * API HISR confirmation *
        // *-----------------------*
        if(SignalCode==API_AAC_START_CON)
        {
          UWORD16 frequency_index = l1a_apihisr_com.aac.frequency_index;
          UWORD16 aac_freq =l1aac_get_frequencyrate(frequency_index);
          UWORD16 stereopath_freq =0; //omaps00090550

   	   #if(L1_BT_AUDIO ==1)
            if(bt_audio.connected_status==TRUE)
            	{
			bt_audio.pcmconfig.bitsPerSample=16;
			bt_audio.pcmconfig.numChannels=  l1a_l1s_com.aac_task.parameters.stereo+1;
			bt_audio.pcmconfig.sampleRate=aac_freq;
			bt_audio.audio_configure_callback(&bt_audio.pcmconfig);
 		}		
	   #endif
#if (TRACE_TYPE == 1) || (TRACE_TYPE == 4)
          // Trace AAC frequency rate
          if((trace_info.current_config->l1_dyn_trace) & (1<<L1_DYN_TRACE_AAC))
          {
            char str[16];
            sprintf(str,"AAC> %d\n\r",aac_freq);
            rvt_send_trace_cpy((T_RVT_BUFFER)str,trace_info.l1_trace_user_id,strlen(str),RVT_ASCII_FORMAT);
          }
#endif    // (TRACE_TYPE == 1) || (TRACE_TYPE == 4)

          // Tell the L1S to enable I2S audio path
          switch(aac_freq)
          {
            case 8000:
              stereopath_freq = AUDIO_SP_FREQ_8;
            break;
            case 11025:
              stereopath_freq = AUDIO_SP_FREQ_11;
            break;
#if (ANLG_FAM == 11)
           //12 khz added in triton
            case 12000:
              stereopath_freq = AUDIO_SP_FREQ_12;
            break;
#endif
            case 16000:
              stereopath_freq = AUDIO_SP_FREQ_16;
            break;
            case 22050:
              stereopath_freq = AUDIO_SP_FREQ_22;
            break;
#if (ANLG_FAM == 11)
           //24 khz added in triton
            case 24000:
              stereopath_freq = AUDIO_SP_FREQ_24;
            break;
#endif			
            case 32000:
              stereopath_freq = AUDIO_SP_FREQ_32;
            break;
            case 44100:
              stereopath_freq = AUDIO_SP_FREQ_44;
            break;
            case 48000:
              stereopath_freq = AUDIO_SP_FREQ_48;
            break;
          }
#if(L1_BT_AUDIO ==1)
if(bt_audio.connected_status==FALSE)
 {
#endif 	
#if (CODE_VERSION == NOT_SIMULATION)
          l1a_l1s_com.stereopath_drv_task.parameters.sampling_frequency    = stereopath_freq;
          l1a_l1s_com.stereopath_drv_task.parameters.DMA_allocation        = AUDIO_SP_DMA_ALLOC_MCU;
          l1a_l1s_com.stereopath_drv_task.parameters.DMA_int_callback_fct  = l1aac_dma_it_handler;
          l1a_l1s_com.stereopath_drv_task.parameters.DMA_channel_number    = l1a_l1s_com.aac_task.parameters.dma_channel_number;
          l1a_l1s_com.stereopath_drv_task.parameters.data_type             = AUDIO_SP_DATA_S16;
          l1a_l1s_com.stereopath_drv_task.parameters.source_port           = dma_src_port;
          l1a_l1s_com.stereopath_drv_task.parameters.source_buffer_address = (WORD8 *)&aac_dma->a_aac_dma_input_buffer[0][0];
          l1a_l1s_com.stereopath_drv_task.parameters.element_number        = 2;
          l1a_l1s_com.stereopath_drv_task.parameters.frame_number          = 2048;
          if(l1a_l1s_com.aac_task.parameters.stereo==1)
            l1a_l1s_com.stereopath_drv_task.parameters.mono_stereo         = AUDIO_SP_STEREO_OUTPUT;
          else
            l1a_l1s_com.stereopath_drv_task.parameters.mono_stereo         = AUDIO_SP_MONO_OUTPUT;
          l1a_l1s_com.stereopath_drv_task.parameters.feature_identifier    = AUDIO_SP_AAC_ID;
#endif
          l1a_l1s_com.stereopath_drv_task.command.start                    = TRUE;
          l1a.aac_task.stereopath_init_pending=TRUE;
#if(L1_BT_AUDIO ==1)
}
else
{
 // Send confirmation to upper layers. Send AAC file format along with start confirmation
            l1a_aac_send_confirmation(MMI_AAC_START_CON,l1a_apihisr_com.aac.aac_format,0);

            // Change state
            *state=AAC_PLAY;
}
#endif
        }

        // *---------------------------------*
        // * AAC HISR reported an error      *
        // *---------------------------------*
        if(SignalCode==API_AAC_STOP_CON)
        {
          // Store AAC API HISR error code
          l1a.aac_task.error_code=((T_L1_API_AAC_STOP_CON *)(msg->SigP))->error_code;

	#if(L1_BT_AUDIO ==1)
        if(bt_audio.connected_status==FALSE)
        {
        #endif
          if(l1a.aac_task.stereopath_init_pending==FALSE)
          {
            // Request DSP background disabling
            l1a_l1s_com.aac_task.command.stop=TRUE;

            // Change state
            *state=WAIT_STOP;
          }
	#if(L1_BT_AUDIO ==1)
        }
       #endif

        }
#if(L1_BT_AUDIO ==1)
if(bt_audio.connected_status==FALSE)
 {
#endif
        // *---------------------------------*
        // * I2S initialization confirmation *
        // *---------------------------------*
        if(SignalCode==L1_STEREOPATH_DRV_START_CON)
        {
          l1a.aac_task.stereopath_init_pending=FALSE;

          // Has an error been reported from AAC API HISR before stereopath init?
          if(l1a.aac_task.error_code!=C_AAC_ERR_NONE)
          {
            // Request audio peripherals disabling
            l1a_l1s_com.stereopath_drv_task.command.stop=TRUE;

            // Change state
            *state=WAIT_STOP;
          }
          else
          {
            // Send confirmation to upper layers. Send AAC file format along with start confirmation
            l1a_aac_send_confirmation(MMI_AAC_START_CON,l1a_apihisr_com.aac.aac_format,0);

            // Change state
            *state=AAC_PLAY;
          }
        }
 #if(L1_BT_AUDIO ==1)		
}
#endif
        end_process = 1;
      }
      break;  // case AAC_INIT

      // ************
      // * AAC_PLAY *
      // ************
      case AAC_PLAY:
      {
        switch(SignalCode)
        {
          // *------------------------*
          // * MMI requests AAC pause *
          // *------------------------*
          case MMI_AAC_PAUSE_REQ:
          {
            // Store pause request in L1A/HISR interface
            l1a_apihisr_com.aac.command.pause=TRUE;

            // Change state
            *state=WAIT_PAUSE_CON;
          }
          break;

          // *-----------------------*
          // * MMI requests AAC stop *
          // *-----------------------*
          case MMI_AAC_STOP_REQ:
          {
            // Store stop request in L1A/HISR interface
            l1a_apihisr_com.aac.command.stop=TRUE;

            // Change state
            *state=WAIT_STOP;
          }
          break;

          // *-------------------------------*
          // * AAC HISR has finished playing *
          // *-------------------------------*
          case API_AAC_STOP_CON:
          {
            // Request audio peripherals disabling
            #if(L1_BT_AUDIO ==1)
		if(bt_audio.connected_status==FALSE)
	     #endif
            l1a_l1s_com.stereopath_drv_task.command.stop=TRUE;
	     #if(L1_BT_AUDIO ==1)
	       else
                l1a_l1s_com.aac_task.command.stop=TRUE;
	     #endif

            // Store AAC API HISR error code
            l1a.aac_task.error_code=((T_L1_API_AAC_STOP_CON *)(msg->SigP))->error_code;

            // Change state
            *state=WAIT_STOP;
          }
          break;

          // *--------------------------------*
          // * AAC stream information request *
          // *--------------------------------*
          case MMI_AAC_INFO_REQ:
          {
            l1a_aac_send_stream_info();
          }
          break;
        }  // switch(SignalCode)

        end_process = 1;
      }
      break;  // case AAC_PLAY

      // ******************
      // * WAIT_PAUSE_CON *
      // ******************
      case WAIT_PAUSE_CON:
      {
        switch(SignalCode)
        {
          // *----------------------------------*
          // * Pause confirmation from AAC HISR *
          // *----------------------------------*
          case API_AAC_PAUSE_CON:
          {
            // Send confirmation to upper layers
            l1a_aac_send_confirmation(MMI_AAC_PAUSE_CON,0,0);

            // Change state
            *state=AAC_PAUSE;
          }
          break;

          // *---------------------------------*
          // * AAC HISR reported an error      *
          // *---------------------------------*
          case API_AAC_STOP_CON:
          {
            // Store AAC API HISR error code
            l1a.aac_task.error_code=((T_L1_API_AAC_STOP_CON *)(msg->SigP))->error_code;

            // Request audio peripherals disabling
           #if(L1_BT_AUDIO ==1)
		if(bt_audio.connected_status==FALSE)
	     #endif
            l1a_l1s_com.stereopath_drv_task.command.stop=TRUE;
	     #if(L1_BT_AUDIO ==1)
	       else
                l1a_l1s_com.aac_task.command.stop=TRUE;
	     #endif

            // Change state
            *state=WAIT_STOP;
          }
          break;
        }

        end_process = 1;
      }   // case WAIT_PAUSE_CON
      break;

      // *************
      // * AAC_PAUSE *
      // *************
      case AAC_PAUSE:
      {
        switch(SignalCode)
        {
          // *-------------------------*
          // * MMI requests AAC resume *
          // *-------------------------*
          case MMI_AAC_RESUME_REQ:
          {
            // Store resume request in L1A/HISR interface
            l1a_apihisr_com.aac.command.resume=TRUE;

            // Change state
            *state=WAIT_RESUME_CON;
          }
          break;

          // *--------------------------*
          // * MMI requests AAC restart *
          // *--------------------------*
          case MMI_AAC_RESTART_REQ:
          {
            // Store restart request in L1A/HISR interface
            l1a_apihisr_com.aac.command.restart=TRUE;

            // Change state
            *state=WAIT_RESUME_CON;
          }
          break;

          // *-----------------------*
          // * MMI requests AAC stop *
          // *-----------------------*
          case MMI_AAC_STOP_REQ:
          {
            // Store stop request in L1A/HISR interface
            l1a_apihisr_com.aac.command.stop=TRUE;

            // Change state
            *state=WAIT_STOP;
          }
          break;

          // *---------------------------------*
          // * AAC HISR reported an error      *
          // *---------------------------------*
          case API_AAC_STOP_CON:
          {
            // Store AAC API HISR error code
            l1a.aac_task.error_code=((T_L1_API_AAC_STOP_CON *)(msg->SigP))->error_code;

            // Request audio peripherals disabling
            #if(L1_BT_AUDIO ==1)
		if(bt_audio.connected_status==FALSE)
	     #endif
            l1a_l1s_com.stereopath_drv_task.command.stop=TRUE;
	     #if(L1_BT_AUDIO ==1)
	       else
                l1a_l1s_com.aac_task.command.stop=TRUE;
	     #endif

            // Change state
            *state=WAIT_STOP;
          }
          break;
        }

        end_process = 1;
      }   // case AAC_PAUSE
      break;

      // *******************
      // * WAIT_RESUME_CON *
      // *******************
      case WAIT_RESUME_CON:
      {
        switch(SignalCode)
        {
          // *-----------------------------------*
          // * Resume confirmation from AAC HISR *
          // *-----------------------------------*
          case API_AAC_RESUME_CON:
          {
            // Send confirmation to upper layers
            l1a_aac_send_confirmation(MMI_AAC_RESUME_CON,0,0);

            // Change state
            *state=AAC_PLAY;
          }
          break;

          // *------------------------------------*
          // * Restart confirmation from AAC HISR *
          // *------------------------------------*
          case API_AAC_RESTART_CON:
          {
            // Send confirmation to upper layers
            l1a_aac_send_confirmation(MMI_AAC_RESTART_CON,0,0);

            // Change state
            *state=AAC_PLAY;
          }
          break;

          // *---------------------------------*
          // * AAC HISR reported an error      *
          // *---------------------------------*
          case API_AAC_STOP_CON:
          {
            // Store AAC API HISR error code
            l1a.aac_task.error_code=((T_L1_API_AAC_STOP_CON *)(msg->SigP))->error_code;

            // Request audio peripherals disabling
          #if(L1_BT_AUDIO ==1)
		if(bt_audio.connected_status==FALSE)
	     #endif
            l1a_l1s_com.stereopath_drv_task.command.stop=TRUE;
	     #if(L1_BT_AUDIO ==1)
	       else
                l1a_l1s_com.aac_task.command.stop=TRUE;
	     #endif

            // Change state
            *state=WAIT_STOP;
          }
          break;
        }

        end_process = 1;
      }   // case WAIT_RESUME_CON
      break;

      // *************
      // * WAIT_STOP *
      // *************
      case WAIT_STOP:
      {
        // *-------------------------------*
        // * AAC HISR has finished playing *
        // *-------------------------------*
        if(SignalCode==API_AAC_STOP_CON)
        {
          // Request audio peripherals disabling
          #if(L1_BT_AUDIO ==1)
		if(bt_audio.connected_status==FALSE)
	     #endif
          l1a_l1s_com.stereopath_drv_task.command.stop=TRUE;
	     #if(L1_BT_AUDIO ==1)
	       else
                l1a_l1s_com.aac_task.command.stop=TRUE;
	     #endif

          // Store AAC API HISR error code
          l1a.aac_task.error_code=((T_L1_API_AAC_STOP_CON *)(msg->SigP))->error_code;
        }

      #if(L1_BT_AUDIO ==1)
	if(bt_audio.connected_status==FALSE)
	{
     #endif
        // *-----------------------*
        // * I2S stop confirmation *
        // *-----------------------*
        if(SignalCode==L1_STEREOPATH_DRV_STOP_CON)
        {
          // Request DSP background disabling
          l1a_l1s_com.aac_task.command.stop=TRUE;
        }
     #if(L1_BT_AUDIO ==1)
	}
     #endif
        // *--------------------------------------------------*
        // * L1S confirmation of DSP AAC background task stop *
        // *--------------------------------------------------*
        if(SignalCode==L1_AAC_DISABLE_CON)
        {
          // Send confirmation to upper layers
          if(l1a.aac_task.error_code!=C_AAC_ERR_NONE)
            l1a_aac_send_confirmation(MMI_AAC_STOP_CON,0,l1a.aac_task.error_code);
          else
            l1a_aac_send_confirmation(MMI_AAC_STOP_CON,0,0);

          // AAC HISR mustn't be activated
          l1a_apihisr_com.aac.running=FALSE;

#if ((TRACE_TYPE == 1) || (TRACE_TYPE == 4))
          // Enable trace DSP upon AAC de-activation
          l1_enable_dsp_trace();
#endif
	   #if (OP_L1_STANDALONE == 0)
	   mem_Deallocate((void*)aac_dma);
    #endif
          // Change state
          *state=RESET;
        }

        end_process = 1;
      }
      break;  // case WAIT_STOP
    }   // switch(*state)
  }   // while(!end_process)
}
#else
void l1a_mmi_aac_process(xSignalHeaderRec *msg)
{
  enum states
  {
    RESET=0,
    WAIT_START_REQ,
    AAC_INIT,
    AAC_PLAY,
    WAIT_PAUSE_CON,
    AAC_PAUSE,
    WAIT_RESUME_CON,
    WAIT_STOP
  };

  UWORD8  *state     = &l1a.state[L1A_AAC_STATE];
  UWORD32 SignalCode = msg->SignalCode;

  BOOL end_process = 0;

  while(!end_process)
  {
    switch(*state)
    {
      // *********
      // * RESET *
      // *********
      case RESET:
      {
        // Reset everything
        l1aac_reset();

        // AAC HISR mustn't be activated
        l1a_apihisr_com.aac.running=FALSE;

        // Change state
        *state=WAIT_START_REQ;
      }
      break;

      // ******************
      // * WAIT_START_REQ *
      // ******************
      case WAIT_START_REQ:
      {
        // *----------------------------*
        // * Start command from the MMI *
        // *----------------------------*
        if(SignalCode==MMI_AAC_START_REQ)
        {
          #if ((TRACE_TYPE==1) || (TRACE_TYPE == 4))
               // Disable trace DSP upon AAC activation
              l1_disable_dsp_trace();
          #endif

          // AAC can be activated
          l1a_apihisr_com.aac.running=TRUE;

          // Must we have loopback or not?
          l1a_l1s_com.aac_task.parameters.loopback  =((T_MMI_AAC_REQ *)(msg->SigP))->loopback;

          // Is the output speaker stereo or mono?
          l1a_l1s_com.aac_task.parameters.stereo    =((T_MMI_AAC_REQ *)(msg->SigP))->stereo;

          // Store the session id
          l1a_l1s_com.aac_task.parameters.session_id=((T_MMI_AAC_REQ *)(msg->SigP))->session_id;

          // Store the DMA channel number
          l1a_l1s_com.aac_task.parameters.dma_channel_number=((T_MMI_AAC_REQ *)(msg->SigP))->dma_channel_number;

          // Reset AAC error code
          l1a.aac_task.error_code                   =C_AAC_ERR_NONE;

          // Tell the L1S to start the DSP background task
          l1a_l1s_com.aac_task.command.start        =TRUE;

          l1a.aac_task.stereopath_init_pending=FALSE;
	  #if (OP_L1_STANDALONE == 1)
          aac_dma=&aac_dma_struct;
	  dma_src_port=AUDIO_SP_SOURCE_IMIF;
       #else
          aac_dma=mem_Allocate(sizeof(T_AAC_DMA_PARAM));
	  if(aac_dma==NULL)
	  {
	  	      l1a_aac_send_confirmation(MMI_AAC_STOP_CON,0,0);
                    *state=RESET;
			end_process = 1;
		  return;
                      
            }
	  #endif

          // Change state
          *state=AAC_INIT;
        }

        end_process = 1;
      }
      break;

      // ************
      // * AAC_INIT *
      // ************
      case AAC_INIT:
      {
        // *------------------------------------------------------------*
        // * L1S confirmation of DSP AAC background task initialization *
        // *------------------------------------------------------------*
        if(SignalCode==L1_AAC_ENABLE_CON)
        {
          // Request DSP AAC start to API HISR
          l1a_apihisr_com.aac.command.start=TRUE;

#if (OP_RIV_AUDIO == 1)
          {
            // WARNING: temporary until os_activate_hisr() is declared in L3 functions
            extern NU_HISR apiHISR;
            NU_Activate_HISR(&apiHISR);
          }
#else
          os_activate_hisr(API_HISR);
#endif
        }

        // *-----------------------*
        // * API HISR confirmation *
        // *-----------------------*
        if(SignalCode==API_AAC_START_CON)
        {
          UWORD16 frequency_index = l1a_apihisr_com.aac.frequency_index;
          UWORD16 aac_freq =l1aac_get_frequencyrate(frequency_index);
          UWORD16 stereopath_freq = 0; //omnaps00090550

#if (TRACE_TYPE == 1) || (TRACE_TYPE == 4)
          // Trace AAC frequency rate
          if((trace_info.current_config->l1_dyn_trace) & (1<<L1_DYN_TRACE_AAC))
          {
            char str[16];
            sprintf(str,"AAC> %d\n\r",aac_freq);
            rvt_send_trace_cpy((T_RVT_BUFFER)str,trace_info.l1_trace_user_id,strlen(str),RVT_ASCII_FORMAT);
          }
#endif    // (TRACE_TYPE == 1) || (TRACE_TYPE == 4)

          // Tell the L1S to enable I2S audio path
          switch(aac_freq)
          {
            case 8000:
              stereopath_freq = AUDIO_SP_FREQ_8;
            break;
            case 11025:
              stereopath_freq = AUDIO_SP_FREQ_11;
            break;
#if (ANLG_FAM == 11)
           //12 khz added in triton
            case 12000:
              stereopath_freq = AUDIO_SP_FREQ_12;
            break;
#endif
            case 16000:
              stereopath_freq = AUDIO_SP_FREQ_16;
            break;
            case 22050:
              stereopath_freq = AUDIO_SP_FREQ_22;
            break;
#if (ANLG_FAM == 11)
           //24 khz added in triton
            case 24000:
              stereopath_freq = AUDIO_SP_FREQ_24;
            break;
#endif
            case 32000:
              stereopath_freq = AUDIO_SP_FREQ_32;
            break;
            case 44100:
              stereopath_freq = AUDIO_SP_FREQ_44;
            break;
            case 48000:
              stereopath_freq = AUDIO_SP_FREQ_48;
            break;
          }
#if (CODE_VERSION == NOT_SIMULATION)
          l1a_l1s_com.stereopath_drv_task.parameters.sampling_frequency    = stereopath_freq;
          l1a_l1s_com.stereopath_drv_task.parameters.DMA_allocation        = AUDIO_SP_DMA_ALLOC_MCU;
          l1a_l1s_com.stereopath_drv_task.parameters.DMA_int_callback_fct  = l1aac_dma_it_handler;
          l1a_l1s_com.stereopath_drv_task.parameters.DMA_channel_number    = l1a_l1s_com.aac_task.parameters.dma_channel_number;
          l1a_l1s_com.stereopath_drv_task.parameters.data_type             = AUDIO_SP_DATA_S16;
          l1a_l1s_com.stereopath_drv_task.parameters.source_port           = dma_src_port;
          l1a_l1s_com.stereopath_drv_task.parameters.source_buffer_address = (WORD8 *)&aac_dma->a_aac_dma_input_buffer[0][0];
          l1a_l1s_com.stereopath_drv_task.parameters.element_number        = 2;
          l1a_l1s_com.stereopath_drv_task.parameters.frame_number          = 2048;
          if(l1a_l1s_com.aac_task.parameters.stereo==1)
            l1a_l1s_com.stereopath_drv_task.parameters.mono_stereo         = AUDIO_SP_STEREO_OUTPUT;
          else
            l1a_l1s_com.stereopath_drv_task.parameters.mono_stereo         = AUDIO_SP_MONO_OUTPUT;
          l1a_l1s_com.stereopath_drv_task.parameters.feature_identifier    = AUDIO_SP_AAC_ID;
#endif
          l1a_l1s_com.stereopath_drv_task.command.start                    = TRUE;
          l1a.aac_task.stereopath_init_pending=TRUE;
        }

        // *---------------------------------*
        // * AAC HISR reported an error      *
        // *---------------------------------*
        if(SignalCode==API_AAC_STOP_CON)
        {
          // Store AAC API HISR error code
          l1a.aac_task.error_code=((T_L1_API_AAC_STOP_CON *)(msg->SigP))->error_code;

          if(l1a.aac_task.stereopath_init_pending==FALSE)
          {
            // Request DSP background disabling
            l1a_l1s_com.aac_task.command.stop=TRUE;

            // Change state
            *state=WAIT_STOP;
          }
        }

        // *---------------------------------*
        // * I2S initialization confirmation *
        // *---------------------------------*
        if(SignalCode==L1_STEREOPATH_DRV_START_CON)
        {
          l1a.aac_task.stereopath_init_pending=FALSE;

          // Has an error been reported from AAC API HISR before stereopath init?
          if(l1a.aac_task.error_code!=C_AAC_ERR_NONE)
          {
            // Request audio peripherals disabling
            l1a_l1s_com.stereopath_drv_task.command.stop=TRUE;

            // Change state
            *state=WAIT_STOP;
          }
          else
          {
            // Send confirmation to upper layers. Send AAC file format along with start confirmation
            l1a_aac_send_confirmation(MMI_AAC_START_CON,l1a_apihisr_com.aac.aac_format,0);

            // Change state
            *state=AAC_PLAY;
          }
        }

        end_process = 1;
      }
      break;  // case AAC_INIT

      // ************
      // * AAC_PLAY *
      // ************
      case AAC_PLAY:
      {
        switch(SignalCode)
        {
          // *------------------------*
          // * MMI requests AAC pause *
          // *------------------------*
          case MMI_AAC_PAUSE_REQ:
          {
            // Store pause request in L1A/HISR interface
            l1a_apihisr_com.aac.command.pause=TRUE;

            // Change state
            *state=WAIT_PAUSE_CON;
          }
          break;

          // *-----------------------*
          // * MMI requests AAC stop *
          // *-----------------------*
          case MMI_AAC_STOP_REQ:
          {
            // Store stop request in L1A/HISR interface
            l1a_apihisr_com.aac.command.stop=TRUE;

            // Change state
            *state=WAIT_STOP;
          }
          break;

          // *-------------------------------*
          // * AAC HISR has finished playing *
          // *-------------------------------*
          case API_AAC_STOP_CON:
          {
            // Request audio peripherals disabling
            l1a_l1s_com.stereopath_drv_task.command.stop=TRUE;

            // Store AAC API HISR error code
            l1a.aac_task.error_code=((T_L1_API_AAC_STOP_CON *)(msg->SigP))->error_code;

            // Change state
            *state=WAIT_STOP;
          }
          break;

          // *--------------------------------*
          // * AAC stream information request *
          // *--------------------------------*
          case MMI_AAC_INFO_REQ:
          {
            l1a_aac_send_stream_info();
          }
          break;
        }  // switch(SignalCode)

        end_process = 1;
      }
      break;  // case AAC_PLAY

      // ******************
      // * WAIT_PAUSE_CON *
      // ******************
      case WAIT_PAUSE_CON:
      {
        switch(SignalCode)
        {
          // *----------------------------------*
          // * Pause confirmation from AAC HISR *
          // *----------------------------------*
          case API_AAC_PAUSE_CON:
          {
            // Send confirmation to upper layers
            l1a_aac_send_confirmation(MMI_AAC_PAUSE_CON,0,0);

            // Change state
            *state=AAC_PAUSE;
          }
          break;

          // *---------------------------------*
          // * AAC HISR reported an error      *
          // *---------------------------------*
          case API_AAC_STOP_CON:
          {
            // Store AAC API HISR error code
            l1a.aac_task.error_code=((T_L1_API_AAC_STOP_CON *)(msg->SigP))->error_code;

            // Request audio peripherals disabling
            l1a_l1s_com.stereopath_drv_task.command.stop=TRUE;

            // Change state
            *state=WAIT_STOP;
          }
          break;
        }

        end_process = 1;
      }   // case WAIT_PAUSE_CON
      break;

      // *************
      // * AAC_PAUSE *
      // *************
      case AAC_PAUSE:
      {
        switch(SignalCode)
        {
          // *-------------------------*
          // * MMI requests AAC resume *
          // *-------------------------*
          case MMI_AAC_RESUME_REQ:
          {
            // Store resume request in L1A/HISR interface
            l1a_apihisr_com.aac.command.resume=TRUE;

            // Change state
            *state=WAIT_RESUME_CON;
          }
          break;

          // *--------------------------*
          // * MMI requests AAC restart *
          // *--------------------------*
          case MMI_AAC_RESTART_REQ:
          {
            // Store restart request in L1A/HISR interface
            l1a_apihisr_com.aac.command.restart=TRUE;

            // Change state
            *state=WAIT_RESUME_CON;
          }
          break;

          // *-----------------------*
          // * MMI requests AAC stop *
          // *-----------------------*
          case MMI_AAC_STOP_REQ:
          {
            // Store stop request in L1A/HISR interface
            l1a_apihisr_com.aac.command.stop=TRUE;

            // Change state
            *state=WAIT_STOP;
          }
          break;

          // *---------------------------------*
          // * AAC HISR reported an error      *
          // *---------------------------------*
          case API_AAC_STOP_CON:
          {
            // Store AAC API HISR error code
            l1a.aac_task.error_code=((T_L1_API_AAC_STOP_CON *)(msg->SigP))->error_code;

            // Request audio peripherals disabling
            l1a_l1s_com.stereopath_drv_task.command.stop=TRUE;

            // Change state
            *state=WAIT_STOP;
          }
          break;
        }

        end_process = 1;
      }   // case AAC_PAUSE
      break;

      // *******************
      // * WAIT_RESUME_CON *
      // *******************
      case WAIT_RESUME_CON:
      {
        switch(SignalCode)
        {
          // *-----------------------------------*
          // * Resume confirmation from AAC HISR *
          // *-----------------------------------*
          case API_AAC_RESUME_CON:
          {
            // Send confirmation to upper layers
            l1a_aac_send_confirmation(MMI_AAC_RESUME_CON,0,0);

            // Change state
            *state=AAC_PLAY;
          }
          break;

          // *------------------------------------*
          // * Restart confirmation from AAC HISR *
          // *------------------------------------*
          case API_AAC_RESTART_CON:
          {
            // Send confirmation to upper layers
            l1a_aac_send_confirmation(MMI_AAC_RESTART_CON,0,0);

            // Change state
            *state=AAC_PLAY;
          }
          break;

          // *---------------------------------*
          // * AAC HISR reported an error      *
          // *---------------------------------*
          case API_AAC_STOP_CON:
          {
            // Store AAC API HISR error code
            l1a.aac_task.error_code=((T_L1_API_AAC_STOP_CON *)(msg->SigP))->error_code;

            // Request audio peripherals disabling
            l1a_l1s_com.stereopath_drv_task.command.stop=TRUE;

            // Change state
            *state=WAIT_STOP;
          }
          break;
        }

        end_process = 1;
      }   // case WAIT_RESUME_CON
      break;

      // *************
      // * WAIT_STOP *
      // *************
      case WAIT_STOP:
      {
        // *-------------------------------*
        // * AAC HISR has finished playing *
        // *-------------------------------*
        if(SignalCode==API_AAC_STOP_CON)
        {
          // Request audio peripherals disabling
          l1a_l1s_com.stereopath_drv_task.command.stop=TRUE;

          // Store AAC API HISR error code
          l1a.aac_task.error_code=((T_L1_API_AAC_STOP_CON *)(msg->SigP))->error_code;
        }

        // *-----------------------*
        // * I2S stop confirmation *
        // *-----------------------*
        if(SignalCode==L1_STEREOPATH_DRV_STOP_CON)
        {
          // Request DSP background disabling
          l1a_l1s_com.aac_task.command.stop=TRUE;
        }

        // *--------------------------------------------------*
        // * L1S confirmation of DSP AAC background task stop *
        // *--------------------------------------------------*
        if(SignalCode==L1_AAC_DISABLE_CON)
        {
          // Send confirmation to upper layers
          if(l1a.aac_task.error_code!=C_AAC_ERR_NONE)
            l1a_aac_send_confirmation(MMI_AAC_STOP_CON,0,l1a.aac_task.error_code);
          else
            l1a_aac_send_confirmation(MMI_AAC_STOP_CON,0,0);

          // AAC HISR mustn't be activated
          l1a_apihisr_com.aac.running=FALSE;

#if ((TRACE_TYPE == 1) || (TRACE_TYPE == 4))
          // Enable trace DSP upon AAC deactivation
          l1_enable_dsp_trace();
#endif
	    #if (OP_L1_STANDALONE == 0)
	   mem_Deallocate((void*)aac_dma);
    #endif
          // Change state
          *state=RESET;
        }

        end_process = 1;
      }
      break;  // case WAIT_STOP
    }   // switch(*state)
  }   // while(!end_process)
}
#endif    // L1_DYN_DSP_DWNLD
#endif    // L1_AAC

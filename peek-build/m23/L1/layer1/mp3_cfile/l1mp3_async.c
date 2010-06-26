/************* Revision Controle System Header *************
 *                  GSM Layer 1 software
 * L1MP3_ASYNC.C
 *
 *        Filename l1mp3_async.c
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
#if (L1_MP3 == 1)
  #include "l1mp3_defty.h"
  #include "l1mp3_msgty.h"
  #include "l1mp3_const.h"
  #include "l1mp3_signa.h"
  #include "l1mp3_error.h"
  #include "l1mp3_proto.h"
#endif
#if (L1_MIDI == 1)
  #include "l1midi_defty.h"
#endif
#if (L1_DYN_DSP_DWNLD == 1)
#include "l1_dyn_dwl_signa.h"
#endif
#include "l1_defty.h"
#include "l1_varex.h"
#include "l1_trace.h"
#include "sys_dma.h"

#if(L1_DYN_DSP_DWNLD == 1)
#if(CODE_VERSION == SIMULATION)
    extern void trace_fct_simu_dyn_dwnld(CHAR *fct_name);
#endif // CODE_VERSION == SIMULATION
#endif // L1_DYN_DSP_DWNLD == 1

#if(L1_BT_AUDIO ==1)
extern T_L1_BT_AUDIO bt_audio;
extern void l1_audio_bt_init(UINT16 media_buf_size);
extern UWORD8   d_mp3_dma_current_buffer_id;
#endif
#if (L1_MP3 == 1)

// MP3 NDB API
T_MP3_MCU_DSP *mp3_ndb;
#if (CODE_VERSION == SIMULATION)
  T_MP3_MCU_DSP mp3_ndb_sim;
#endif

#if (L1_MP3_SIX_BUFFER == 1)
UWORD16  a_mp3_dma_input_buffer[MP3_BUFFER_COUNT][C_MP3_OUTPUT_BUFFER_SIZE];
#endif

#if (OP_L1_STANDALONE == 1)
#if (CODE_VERSION == NOT_SIMULATION)
#pragma DATA_SECTION(mp3_dma_struct,".l1s_global")
#endif
T_MP3_DMA_PARAM mp3_dma_struct;
#endif
T_MP3_DMA_PARAM *mp3_dma;
 extern UWORD8 dma_src_port;
#if (OP_L1_STANDALONE == 0)
extern void * mem_Allocate(UWORD16 size);
extern void    mem_Deallocate(void *memoryBlock);	
#endif

 #if ( (L1_MP3 == 1) || (L1_AAC == 1) || (L1_MIDI == 1) || (L1_DYN_DSP_DWNLD == 1) )
  extern NU_HISR apiHISR;
#endif

/*-------------------------------------------------------*/
/* l1a_mmi_mp3_process()                                 */
/*-------------------------------------------------------*/
/*                                                       */
/* Description:                                          */
/* ------------                                          */
/* This function is a state machine which handles the    */
/* MP3 feature.                                          */
/*                                                       */
/* Starting messages:        MMI_MP3_START_REQ           */
/*                                                       */
/* Result messages (input):  L1_MP3_ENABLE_CON           */
/*                           L1_MP3_DISABLE_CON          */
/*                                                       */
/* Result messages (output): MMI_MP3_START_CON           */
/*                                                       */
/* Reset messages (input):   none                        */
/*                                                       */
/* Stop message (input):     MMI_MP3_STOP_REQ            */
/*                                                       */
/* Stop message (output):    MMI_MP3_STOP_CON            */
/*                                                       */
/* Rem:                                                  */
/* ----                                                  */
/*                                                       */
/*-------------------------------------------------------*/
#if (L1_DYN_DSP_DWNLD == 1)
void l1a_mmi_mp3_process(xSignalHeaderRec *msg)
{
  enum states
  {
    RESET=0,
    WAIT_START_REQ,
    WAIT_DYN_DWNLD,
    MP3_INIT,
    MP3_PLAY,
    WAIT_PAUSE_CON,
    MP3_PAUSE,
    WAIT_RESUME_CON,
    WAIT_STOP
  };

  UWORD8  *state     = &l1a.state[L1A_MP3_STATE];
  UWORD32 SignalCode = msg->SignalCode;

  while(1)
  {
    switch(*state)
    {
      // *********
      // * RESET *
      // *********
      case RESET:
      {
        // Reset everything
        l1mp3_reset();

        // MP3 HISR mustn't be activated
        l1a_apihisr_com.mp3.running=FALSE;

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
        if(SignalCode==MMI_MP3_START_REQ)
        {

          // Must we have loopback or not?
          l1a_l1s_com.mp3_task.parameters.loopback  =((T_MMI_MP3_REQ *)(msg->SigP))->loopback;

          // Is the output speaker stereo or mono?
          l1a_l1s_com.mp3_task.parameters.stereo    =((T_MMI_MP3_REQ *)(msg->SigP))->stereo;

          // Store the session id
          l1a_l1s_com.mp3_task.parameters.session_id=((T_MMI_MP3_REQ *)(msg->SigP))->session_id;

          // Store the DMA channel number
          l1a_apihisr_com.mp3.dma_channel_number    =((T_MMI_MP3_REQ *)(msg->SigP))->dma_channel_number;

          // Reset MP3 error code
          l1a.mp3_task.error_code                   =C_MP3_ERR_NONE;

          l1a.mp3_task.stereopath_init_pending=FALSE;

#if (OP_L1_STANDALONE == 1)
            mp3_dma=&mp3_dma_struct;
	    dma_src_port=AUDIO_SP_SOURCE_IMIF;
         #else
	  mp3_dma=mem_Allocate(sizeof(T_MP3_DMA_PARAM));
	  if(mp3_dma==NULL)
	  {
	  	l1a_mp3_send_confirmation(MMI_MP3_STOP_CON,0);

		// Change state
              *state=RESET;
		 return;
         }
	  #endif


         #if(L1_BT_AUDIO == 1)
	  if(bt_audio.connected_status == TRUE)
         l1_audio_bt_init(C_MP3_OUTPUT_BUFFER_SIZE);
	 #endif	
          if (l1a.dyn_dwnld.semaphore_vect[MP3_STATE_MACHINE]==GREEN)
          {
             // WARNING: code below must be duplicated in WAIT_DYN_DWNLD state
            // Start the L1S MP3 task

	            // DSP Trace is effectively disabled 1 TDMA frame later
           #if ((TRACE_TYPE==1) || (TRACE_TYPE == 4))
            // Disable trace DSP upon MP3 activation
                 l1_disable_dsp_trace();
           #endif
            // Tell the L1S to start the DSP background task
            l1a_l1s_com.mp3_task.command.start        =TRUE;


            // MP3 can be activated
            l1a_apihisr_com.mp3.running=TRUE;

            // Change state
            *state=MP3_INIT;
          }
          else
          {
            *state = WAIT_DYN_DWNLD;

#if ((TRACE_TYPE == 1) || (TRACE_TYPE == 4) || (TRACE_TYPE == 5))
            if((trace_info.current_config->l1_dyn_trace) & (1<<L1_DYN_TRACE_DYN_DWNLD))
            {
              char str[30];
              sprintf(str,"MP3 SM blocked\r\n");
#if(CODE_VERSION == SIMULATION)
              trace_fct_simu_dyn_dwnld(str);
#else
              rvt_send_trace_cpy((T_RVT_BUFFER)str,trace_info.l1_trace_user_id,strlen(str),RVT_ASCII_FORMAT);
#endif // CODE_VERSION == SIMULATION
            }
#endif    // (TRACE_TYPE == 1) || (TRACE_TYPE == 4)
          }
        }

        return;
      }
//omaps00090550      break;
      case WAIT_DYN_DWNLD:
      {
        if((SignalCode==API_L1_DYN_DWNLD_FINISHED) && (l1a.dyn_dwnld.semaphore_vect[MP3_STATE_MACHINE] == GREEN))
        {
            #if ((TRACE_TYPE==1) || (TRACE_TYPE == 4))
               // Disable trace DSP upon MP3 activation
               l1_disable_dsp_trace();
           #endif
          // Tell the L1S to start the DSP background task
          l1a_l1s_com.mp3_task.command.start        =TRUE;


          // MP3 can be activated
          l1a_apihisr_com.mp3.running=TRUE;

          // Change state
          *state=MP3_INIT;

#if ((TRACE_TYPE == 1) || (TRACE_TYPE == 4) || (TRACE_TYPE == 5))
          if((trace_info.current_config->l1_dyn_trace) & (1<<L1_DYN_TRACE_DYN_DWNLD))
          {
            char str[32];
            sprintf(str,"MP3 SM un-blocked\r\n");
#if(CODE_VERSION == SIMULATION)
            trace_fct_simu_dyn_dwnld(str);
#else
            rvt_send_trace_cpy((T_RVT_BUFFER)str,trace_info.l1_trace_user_id,strlen(str),RVT_ASCII_FORMAT);
#endif
          }
#endif    // (TRACE_TYPE == 1) || (TRACE_TYPE == 4)
        }
        return;
      }
//omaps00090550      break;
      // ************
      // * MP3_INIT *
      // ************
      case MP3_INIT:
      {
        // *------------------------------------------------------------*
        // * L1S confirmation of DSP MP3 background task initialization *
        // *------------------------------------------------------------*
        if(SignalCode==L1_MP3_ENABLE_CON)
        {
          // Request DSP MP3 start to API HISR
          l1a_apihisr_com.mp3.command.start=TRUE;

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
        if(SignalCode==API_MP3_START_CON)
        {
          UWORD32 mp3header=l1a_apihisr_com.mp3.header;
          UWORD16 mp3_freq =l1mp3_get_frequencyrate(mp3header);
          UWORD16 stereopath_freq =0 ; //omaps00090550

	   #if(L1_BT_AUDIO ==1)
            if(bt_audio.connected_status==TRUE)
            	{
			bt_audio.pcmconfig.bitsPerSample=16;
			bt_audio.pcmconfig.numChannels= l1a_l1s_com.mp3_task.parameters.stereo+1;
			bt_audio.pcmconfig.sampleRate=mp3_freq;
			bt_audio.audio_configure_callback(&bt_audio.pcmconfig);
             	}		
	   #endif
#if (TRACE_TYPE == 1) || (TRACE_TYPE == 4)
          // Trace MP3 frequency rate
          if((trace_info.current_config->l1_dyn_trace) & (1<<L1_DYN_TRACE_MP3))
          {
            char str[16];
            sprintf(str,"MP3> %d\n\r",l1mp3_get_frequencyrate(mp3header));
            rvt_send_trace_cpy((T_RVT_BUFFER)str,trace_info.l1_trace_user_id,strlen(str),RVT_ASCII_FORMAT);
          }
#endif    // (TRACE_TYPE == 1) || (TRACE_TYPE == 4)
#if (CODE_VERSION != SIMULATION)
          // Tell the L1S to enable I2S audio path
          switch(mp3_freq)
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
#endif //CODE_VERSION
#if(L1_BT_AUDIO ==1)
if(bt_audio.connected_status==FALSE)
 {
#endif 	
#if (CODE_VERSION == NOT_SIMULATION)
#if (L1_MP3_SIX_BUFFER == 1)
          l1a_l1s_com.stereopath_drv_task.parameters.sampling_frequency    = stereopath_freq;
          l1a_l1s_com.stereopath_drv_task.parameters.DMA_allocation        = AUDIO_SP_DMA_ALLOC_MCU;
          l1a_l1s_com.stereopath_drv_task.parameters.DMA_int_callback_fct  = l1mp3_dma_it_handler;
          l1a_l1s_com.stereopath_drv_task.parameters.DMA_channel_number    = l1a_apihisr_com.mp3.dma_channel_number;
          l1a_l1s_com.stereopath_drv_task.parameters.data_type             = AUDIO_SP_DATA_S16;
          l1a_l1s_com.stereopath_drv_task.parameters.source_port           = dma_src_port;
          l1a_l1s_com.stereopath_drv_task.parameters.source_buffer_address = (WORD8 *)&mp3_dma->a_mp3_dma_input_buffer[0][0];
          l1a_l1s_com.stereopath_drv_task.parameters.element_number        = 2;
          l1a_l1s_com.stereopath_drv_task.parameters.frame_number          = 1152*(MP3_BUFFER_COUNT/2);
#else
          l1a_l1s_com.stereopath_drv_task.parameters.sampling_frequency    = stereopath_freq;
          l1a_l1s_com.stereopath_drv_task.parameters.DMA_allocation        = AUDIO_SP_DMA_ALLOC_DSP;
          l1a_l1s_com.stereopath_drv_task.parameters.DMA_int_callback_fct  = f_dma_default_call_back_it;
          l1a_l1s_com.stereopath_drv_task.parameters.DMA_channel_number    = l1a_apihisr_com.mp3.dma_channel_number;
          l1a_l1s_com.stereopath_drv_task.parameters.data_type             = AUDIO_SP_DATA_S16;
          l1a_l1s_com.stereopath_drv_task.parameters.source_port           = AUDIO_SP_SOURCE_API;
          l1a_l1s_com.stereopath_drv_task.parameters.source_buffer_address = (WORD8 *)mp3_ndb->a_mp3_api_output_buffer0;
          l1a_l1s_com.stereopath_drv_task.parameters.element_number        = 2;
          l1a_l1s_com.stereopath_drv_task.parameters.frame_number          = 1152;
#endif		  
          if(l1a_l1s_com.mp3_task.parameters.stereo==1)
            l1a_l1s_com.stereopath_drv_task.parameters.mono_stereo         = AUDIO_SP_STEREO_OUTPUT;
          else
            l1a_l1s_com.stereopath_drv_task.parameters.mono_stereo         = AUDIO_SP_MONO_OUTPUT;
          l1a_l1s_com.stereopath_drv_task.parameters.feature_identifier    = AUDIO_SP_MP3_ID;
#endif
          l1a_l1s_com.stereopath_drv_task.command.start                    = TRUE;
          l1a.mp3_task.stereopath_init_pending=TRUE;
#if(L1_BT_AUDIO ==1)
}
else
{
	  l1a_mp3_send_confirmation(MMI_MP3_START_CON,0);

	// Change state
         *state=MP3_PLAY;
}
#endif
        }

        // *---------------------------------*
        // * MP3 HISR reported an error      *
        // *---------------------------------*
        if(SignalCode==API_MP3_STOP_CON)
        {
          // Store MP3 API HISR error code
          l1a.mp3_task.error_code=((T_L1_API_MP3_STOP_CON *)(msg->SigP))->error_code;

     #if(L1_BT_AUDIO ==1)
        if(bt_audio.connected_status==FALSE)
        {
     #endif
          if(l1a.mp3_task.stereopath_init_pending==FALSE)
          {
            // Request DSP background disabling
            l1a_l1s_com.mp3_task.command.stop=TRUE;

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
          l1a.mp3_task.stereopath_init_pending=FALSE;

          // Has an error been reported from MP3 API HISR before stereopath init?
          if(l1a.mp3_task.error_code!=C_MP3_ERR_NONE)
          {
            // Request audio peripherals disabling
            l1a_l1s_com.stereopath_drv_task.command.stop=TRUE;

            // Change state
            *state=WAIT_STOP;
          }
          else
          {
            // Send confirmation to upper layers
            l1a_mp3_send_confirmation(MMI_MP3_START_CON,0);

            // Change state
            *state=MP3_PLAY;
          }
        }
#if(L1_BT_AUDIO ==1)
}
#endif
        return;
      }
//omaps00090550      break;  // case MP3_INIT

      // ************
      // * MP3_PLAY *
      // ************
      case MP3_PLAY:
      {
        switch(SignalCode)
        {
          // *------------------------*
          // * MMI requests MP3 pause *
          // *------------------------*
          case MMI_MP3_PAUSE_REQ:
          {
            // Store pause request in L1A/HISR interface
            l1a_apihisr_com.mp3.command.pause=TRUE;

            // Change state
            *state=WAIT_PAUSE_CON;
          }
          break;

          // *-----------------------*
          // * MMI requests MP3 stop *
          // *-----------------------*
          case MMI_MP3_STOP_REQ:
          {
            // Store stop request in L1A/HISR interface
            l1a_apihisr_com.mp3.command.stop=TRUE;
           /*  Activating apiHisr so that MCU can send stop req  to DSP */
           /*  if DSP-MCU communication is failed inbetween play        */
	   #if ( (L1_MP3 == 1) || (L1_MIDI == 1) || (L1_AAC == 1) || (L1_DYN_DSP_DWNLD == 1) )
	     NU_Activate_HISR(&apiHISR); 
	  #endif

            // Change state
            *state=WAIT_STOP;
          }
          break;

          // *-------------------------------*
          // * MP3 HISR has finished playing *
          // *-------------------------------*
          case API_MP3_STOP_CON:
          {
            // Request audio peripherals disabling
    #if(L1_BT_AUDIO ==1)
		if(bt_audio.connected_status==FALSE)
	 #endif
            l1a_l1s_com.stereopath_drv_task.command.stop=TRUE;
	#if(L1_BT_AUDIO ==1)
	 else
          l1a_l1s_com.mp3_task.command.stop=TRUE;
	 #endif
            // Store MP3 API HISR error code
            l1a.mp3_task.error_code=((T_L1_API_MP3_STOP_CON *)(msg->SigP))->error_code;

            // Change state
            *state=WAIT_STOP;
          }
          break;

          // *--------------------------------*
          // * MP3 header information request *
          // *--------------------------------*
          case MMI_MP3_INFO_REQ:
          {
            l1a_mp3_send_header_info();
          }
          break;
        }  // switch(SignalCode)

        return;
      }
//omaps00090550      break;  // case MP3_PLAY

      // ******************
      // * WAIT_PAUSE_CON *
      // ******************
      case WAIT_PAUSE_CON:
      {
        switch(SignalCode)
        {
          // *----------------------------------*
          // * Pause confirmation from MP3 HISR *
          // *----------------------------------*
          case API_MP3_PAUSE_CON:
          {
            // Send confirmation to upper layers
            l1a_mp3_send_confirmation(MMI_MP3_PAUSE_CON,0);

            // Change state
            *state=MP3_PAUSE;
          }
          break;

          // *---------------------------------*
          // * MP3 HISR reported an error      *
          // *---------------------------------*
          case API_MP3_STOP_CON:
          {
            // Store MP3 API HISR error code
            l1a.mp3_task.error_code=((T_L1_API_MP3_STOP_CON *)(msg->SigP))->error_code;

            // Request audio peripherals disabling
           #if(L1_BT_AUDIO ==1)
		if(bt_audio.connected_status==FALSE)
	 #endif
            l1a_l1s_com.stereopath_drv_task.command.stop=TRUE;
	#if(L1_BT_AUDIO ==1)
	 else
          l1a_l1s_com.mp3_task.command.stop=TRUE;
	 #endif
            // Change state
            *state=WAIT_STOP;
          }
          break;
        }

        return;
      }   // case WAIT_PAUSE_CON
//omaps00090550      break;

      // *************
      // * MP3_PAUSE *
      // *************
      case MP3_PAUSE:
      {
        switch(SignalCode)
        {
          // *-------------------------*
          // * MMI requests MP3 resume *
          // *-------------------------*
          case MMI_MP3_RESUME_REQ:
          {
            // Store resume request in L1A/HISR interface
            l1a_apihisr_com.mp3.command.resume=TRUE;

            // Change state
            *state=WAIT_RESUME_CON;
          }
          break;

          // *--------------------------*
          // * MMI requests MP3 restart *
          // *--------------------------*
          case MMI_MP3_RESTART_REQ:
          {
             // Store restart request in L1A/HISR interface
             l1a_apihisr_com.mp3.command.restart=TRUE;

            // Change state
            *state=WAIT_RESUME_CON;
          }
          break;

          // *-----------------------*
          // * MMI requests MP3 stop *
          // *-----------------------*
          case MMI_MP3_STOP_REQ:
          {
            // Store stop request in L1A/HISR interface
            l1a_apihisr_com.mp3.command.stop=TRUE;

            // Change state
            *state=WAIT_STOP;
          }
          break;

          // *---------------------------------*
          // * MP3 HISR reported an error      *
          // *---------------------------------*
          case API_MP3_STOP_CON:
          {
            // Store MP3 API HISR error code
            l1a.mp3_task.error_code=((T_L1_API_MP3_STOP_CON *)(msg->SigP))->error_code;

            // Request audio peripherals disabling
           #if(L1_BT_AUDIO ==1)
		if(bt_audio.connected_status==FALSE)
	 #endif
            l1a_l1s_com.stereopath_drv_task.command.stop=TRUE;
	#if(L1_BT_AUDIO ==1)
	 else
          l1a_l1s_com.mp3_task.command.stop=TRUE;
	 #endif
            // Change state
            *state=WAIT_STOP;
          }
          break;
        }

        return;
      }   // case MP3_PAUSE
//omaps00090550      break;

      // *******************
      // * WAIT_RESUME_CON *
      // *******************
      case WAIT_RESUME_CON:
      {
        switch(SignalCode)
        {
          // *-----------------------------------*
          // * Resume confirmation from MP3 HISR *
          // *-----------------------------------*
          case API_MP3_RESUME_CON:
          {
            // Send confirmation to upper layers
            l1a_mp3_send_confirmation(MMI_MP3_RESUME_CON,0);

            // Change state
            *state=MP3_PLAY;
          }
          break;

          // *------------------------------------*
          // * Restart confirmation from MP3 HISR *
          // *------------------------------------*
          case API_MP3_RESTART_CON:
          {
            // Send confirmation to upper layers
            l1a_mp3_send_confirmation(MMI_MP3_RESTART_CON,0);

            // Change state
            *state=MP3_PLAY;
          }
          break;

          // *---------------------------------*
          // * MP3 HISR reported an error      *
          // *---------------------------------*
          case API_MP3_STOP_CON:
          {
            // Store MP3 API HISR error code
            l1a.mp3_task.error_code=((T_L1_API_MP3_STOP_CON *)(msg->SigP))->error_code;

            // Request audio peripherals disabling
           #if(L1_BT_AUDIO ==1)
		if(bt_audio.connected_status==FALSE)
	   #endif
            l1a_l1s_com.stereopath_drv_task.command.stop=TRUE;
          #if(L1_BT_AUDIO ==1)
	      else
               l1a_l1s_com.mp3_task.command.stop=TRUE;
	   #endif
            // Change state
            *state=WAIT_STOP;
          }
          break;
        }

        return;
      }   // case WAIT_RESUME_CON
//omaps00090550      break;

      // *************
      // * WAIT_STOP *
      // *************
      case WAIT_STOP:
      {
        // *-------------------------------*
        // * MP3 HISR has finished playing *
        // *-------------------------------*
        if(SignalCode==API_MP3_STOP_CON)
        {
          // Request audio peripherals disabling
         #if(L1_BT_AUDIO ==1)
		if(bt_audio.connected_status==FALSE)
	 #endif
          l1a_l1s_com.stereopath_drv_task.command.stop=TRUE;
	#if(L1_BT_AUDIO ==1)
	 else
          l1a_l1s_com.mp3_task.command.stop=TRUE;
	 #endif
          // Store MP3 API HISR error code
          l1a.mp3_task.error_code=((T_L1_API_MP3_STOP_CON *)(msg->SigP))->error_code;
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
          l1a_l1s_com.mp3_task.command.stop=TRUE;
        }
        #if(L1_BT_AUDIO ==1)
	 }
	 #endif
        // *--------------------------------------------------*
        // * L1S confirmation of DSP MP3 background task stop *
        // *--------------------------------------------------*
        if(SignalCode==L1_MP3_DISABLE_CON)
        {
           if((bt_audio.connected_status==TRUE))
		 	{  
				bt_audio.pcm_data_end=1;
            	l1_audio_manager((UWORD8 *)&mp3_dma->a_mp3_dma_input_buffer[d_mp3_dma_current_buffer_id][0],C_MP3_OUTPUT_BUFFER_SIZE);
             	}
          // Send confirmation to upper layers
          if(l1a.mp3_task.error_code!=C_MP3_ERR_NONE)
            l1a_mp3_send_confirmation(MMI_MP3_STOP_CON,l1a.mp3_task.error_code);
          else
            l1a_mp3_send_confirmation(MMI_MP3_STOP_CON,0);

          // MP3 HISR mustn't be activated
          l1a_apihisr_com.mp3.running=FALSE;

#if ((TRACE_TYPE==1) || (TRACE_TYPE == 4))
          // Enable trace DSP upon MP3 deactivation
          l1_enable_dsp_trace();
#endif

        #if (OP_L1_STANDALONE == 0)
          mem_Deallocate((void*)mp3_dma);
          #endif
          // Change state
          *state=RESET;
        }

        return;
      }
//omaps00090550      break;  // case WAIT_STOP
    }   // switch(*state)
  }   // while(1)
}
#else
void l1a_mmi_mp3_process(xSignalHeaderRec *msg)
{
  enum states
  {
    RESET=0,
    WAIT_START_REQ,
    MP3_INIT,
    MP3_PLAY,
    WAIT_PAUSE_CON,
    MP3_PAUSE,
    WAIT_RESUME_CON,
    WAIT_STOP
  };

  UWORD8  *state     = &l1a.state[L1A_MP3_STATE];
  UWORD32 SignalCode = msg->SignalCode;

  while(1)
  {
    switch(*state)
    {
      // *********
      // * RESET *
      // *********
      case RESET:
      {
        // Reset everything
        l1mp3_reset();

        // MP3 HISR mustn't be activated
        l1a_apihisr_com.mp3.running=FALSE;

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
        if(SignalCode==MMI_MP3_START_REQ)
        {
           #if ((TRACE_TYPE==1) || (TRACE_TYPE == 4))
             // Disable trace DSP upon MP3 activation
             l1_disable_dsp_trace();
           #endif

          // MP3 can be activated
          l1a_apihisr_com.mp3.running=TRUE;

          // Must we have loopback or not?
          l1a_l1s_com.mp3_task.parameters.loopback  =((T_MMI_MP3_REQ *)(msg->SigP))->loopback;

          // Is the output speaker stereo or mono?
          l1a_l1s_com.mp3_task.parameters.stereo    =((T_MMI_MP3_REQ *)(msg->SigP))->stereo;

          // Store the session id
          l1a_l1s_com.mp3_task.parameters.session_id=((T_MMI_MP3_REQ *)(msg->SigP))->session_id;

          // Store the DMA channel number
          l1a_apihisr_com.mp3.dma_channel_number    =((T_MMI_MP3_REQ *)(msg->SigP))->dma_channel_number;

          // Reset MP3 error code
          l1a.mp3_task.error_code                   =C_MP3_ERR_NONE;

          // Tell the L1S to start the DSP background task
          l1a_l1s_com.mp3_task.command.start        =TRUE;

          l1a.mp3_task.stereopath_init_pending=FALSE;

          // Change state
          *state=MP3_INIT;
        }

        return;
      }
      break;

      // ************
      // * MP3_INIT *
      // ************
      case MP3_INIT:
      {
        // *------------------------------------------------------------*
        // * L1S confirmation of DSP MP3 background task initialization *
        // *------------------------------------------------------------*
        if(SignalCode==L1_MP3_ENABLE_CON)
        {
          // Request DSP MP3 start to API HISR
          l1a_apihisr_com.mp3.command.start=TRUE;

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
        if(SignalCode==API_MP3_START_CON)
        {
          UWORD32 mp3header=l1a_apihisr_com.mp3.header;
          UWORD16 mp3_freq =l1mp3_get_frequencyrate(mp3header);
          UWORD16 stereopath_freq = 0; //omaps00090550

#if (TRACE_TYPE == 1) || (TRACE_TYPE == 4)
          // Trace MP3 frequency rate
          if((trace_info.current_config->l1_dyn_trace) & (1<<L1_DYN_TRACE_MP3))
          {
            char str[16];
            sprintf(str,"MP3> %d\n\r",l1mp3_get_frequencyrate(mp3header));
            rvt_send_trace_cpy((T_RVT_BUFFER)str,trace_info.l1_trace_user_id,strlen(str),RVT_ASCII_FORMAT);
          }
#endif    // (TRACE_TYPE == 1) || (TRACE_TYPE == 4)
#if (CODE_VERSION != SIMULATION)
          // Tell the L1S to enable I2S audio path
          switch(mp3_freq)
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
#endif // CODE_VERSION
#if (CODE_VERSION == NOT_SIMULATION)
#if (L1_MP3_SIX_BUFFER == 1)
          l1a_l1s_com.stereopath_drv_task.parameters.sampling_frequency    = stereopath_freq;
          l1a_l1s_com.stereopath_drv_task.parameters.DMA_allocation        = AUDIO_SP_DMA_ALLOC_MCU;
          l1a_l1s_com.stereopath_drv_task.parameters.DMA_int_callback_fct  = l1mp3_dma_it_handler;
          l1a_l1s_com.stereopath_drv_task.parameters.DMA_channel_number    = l1a_apihisr_com.mp3.dma_channel_number;
          l1a_l1s_com.stereopath_drv_task.parameters.data_type             = AUDIO_SP_DATA_S16;
          l1a_l1s_com.stereopath_drv_task.parameters.source_port           = dma_src_port;
          l1a_l1s_com.stereopath_drv_task.parameters.source_buffer_address = (WORD8 *) &mp3_dma->a_mp3_dma_input_buffer[0][0];
          l1a_l1s_com.stereopath_drv_task.parameters.element_number        = 2;
          l1a_l1s_com.stereopath_drv_task.parameters.frame_number          = 1152*(MP3_BUFFER_COUNT/2);
#else 
          l1a_l1s_com.stereopath_drv_task.parameters.sampling_frequency    = stereopath_freq;
          l1a_l1s_com.stereopath_drv_task.parameters.DMA_allocation        = AUDIO_SP_DMA_ALLOC_DSP;
          l1a_l1s_com.stereopath_drv_task.parameters.DMA_int_callback_fct  = f_dma_default_call_back_it;
          l1a_l1s_com.stereopath_drv_task.parameters.DMA_channel_number    = l1a_apihisr_com.mp3.dma_channel_number;
          l1a_l1s_com.stereopath_drv_task.parameters.data_type             = AUDIO_SP_DATA_S16;
          l1a_l1s_com.stereopath_drv_task.parameters.source_port           = AUDIO_SP_SOURCE_API;
          l1a_l1s_com.stereopath_drv_task.parameters.source_buffer_address = (WORD8 *)mp3_ndb->a_mp3_api_output_buffer0;
          l1a_l1s_com.stereopath_drv_task.parameters.element_number        = 2;
          l1a_l1s_com.stereopath_drv_task.parameters.frame_number          = 1152;
#endif
          if(l1a_l1s_com.mp3_task.parameters.stereo==1)
            l1a_l1s_com.stereopath_drv_task.parameters.mono_stereo         = AUDIO_SP_STEREO_OUTPUT;
          else
            l1a_l1s_com.stereopath_drv_task.parameters.mono_stereo         = AUDIO_SP_MONO_OUTPUT;
          l1a_l1s_com.stereopath_drv_task.parameters.feature_identifier    = AUDIO_SP_MP3_ID;
#endif
          l1a_l1s_com.stereopath_drv_task.command.start                    = TRUE;
          l1a.mp3_task.stereopath_init_pending=TRUE;
        }

        // *---------------------------------*
        // * MP3 HISR reported an error      *
        // *---------------------------------*
        if(SignalCode==API_MP3_STOP_CON)
        {
          // Store MP3 API HISR error code
          l1a.mp3_task.error_code=((T_L1_API_MP3_STOP_CON *)(msg->SigP))->error_code;

          if(l1a.mp3_task.stereopath_init_pending==FALSE)
          {
            // Request DSP background disabling
            l1a_l1s_com.mp3_task.command.stop=TRUE;

            // Change state
            *state=WAIT_STOP;
          }
        }

        // *---------------------------------*
        // * I2S initialization confirmation *
        // *---------------------------------*
        if(SignalCode==L1_STEREOPATH_DRV_START_CON)
        {
          l1a.mp3_task.stereopath_init_pending=FALSE;

          // Has an error been reported from MP3 API HISR before stereopath init?
          if(l1a.mp3_task.error_code!=C_MP3_ERR_NONE)
          {
            // Request audio peripherals disabling
            l1a_l1s_com.stereopath_drv_task.command.stop=TRUE;

            // Change state
            *state=WAIT_STOP;
          }
          else
          {
            // Send confirmation to upper layers
            l1a_mp3_send_confirmation(MMI_MP3_START_CON,0);

            // Change state
            *state=MP3_PLAY;
          }
        }

        return;
      }
      break;  // case MP3_INIT

      // ************
      // * MP3_PLAY *
      // ************
      case MP3_PLAY:
      {
        switch(SignalCode)
        {
          // *------------------------*
          // * MMI requests MP3 pause *
          // *------------------------*
          case MMI_MP3_PAUSE_REQ:
          {
            // Store pause request in L1A/HISR interface
            l1a_apihisr_com.mp3.command.pause=TRUE;

            // Change state
            *state=WAIT_PAUSE_CON;
          }
          break;

          // *-----------------------*
          // * MMI requests MP3 stop *
          // *-----------------------*
          case MMI_MP3_STOP_REQ:
          {
            // Store stop request in L1A/HISR interface
            l1a_apihisr_com.mp3.command.stop=TRUE;

#if (L1_MP3_SIX_BUFFER == 1)			
            // Request audio peripherals disabling
            l1a_l1s_com.stereopath_drv_task.command.stop=TRUE;
#endif
            // Change state
            *state=WAIT_STOP;
          }
          break;

          // *-------------------------------*
          // * MP3 HISR has finished playing *
          // *-------------------------------*
          case API_MP3_STOP_CON:
          {
            // Request audio peripherals disabling
            l1a_l1s_com.stereopath_drv_task.command.stop=TRUE;

            // Store MP3 API HISR error code
            l1a.mp3_task.error_code=((T_L1_API_MP3_STOP_CON *)(msg->SigP))->error_code;

            // Change state
            *state=WAIT_STOP;
          }
          break;

          // *--------------------------------*
          // * MP3 header information request *
          // *--------------------------------*
          case MMI_MP3_INFO_REQ:
          {
            l1a_mp3_send_header_info();
          }
          break;
        }  // switch(SignalCode)

        return;
      }
      break;  // case MP3_PLAY

      // ******************
      // * WAIT_PAUSE_CON *
      // ******************
      case WAIT_PAUSE_CON:
      {
        switch(SignalCode)
        {
          // *----------------------------------*
          // * Pause confirmation from MP3 HISR *
          // *----------------------------------*
          case API_MP3_PAUSE_CON:
          {
            // Send confirmation to upper layers
            l1a_mp3_send_confirmation(MMI_MP3_PAUSE_CON,0);

            // Change state
            *state=MP3_PAUSE;
          }
          break;

          // *---------------------------------*
          // * MP3 HISR reported an error      *
          // *---------------------------------*
          case API_MP3_STOP_CON:
          {
            // Store MP3 API HISR error code
            l1a.mp3_task.error_code=((T_L1_API_MP3_STOP_CON *)(msg->SigP))->error_code;

            // Request audio peripherals disabling
            l1a_l1s_com.stereopath_drv_task.command.stop=TRUE;

            // Change state
            *state=WAIT_STOP;
          }
          break;
        }

        return;
      }   // case WAIT_PAUSE_CON
      break;

      // *************
      // * MP3_PAUSE *
      // *************
      case MP3_PAUSE:
      {
        switch(SignalCode)
        {
          // *-------------------------*
          // * MMI requests MP3 resume *
          // *-------------------------*
          case MMI_MP3_RESUME_REQ:
          {
            // Store resume request in L1A/HISR interface
            l1a_apihisr_com.mp3.command.resume=TRUE;

            // Change state
            *state=WAIT_RESUME_CON;
          }
          break;

          // *--------------------------*
          // * MMI requests MP3 restart *
          // *--------------------------*
          case MMI_MP3_RESTART_REQ:
          {
             // Store restart request in L1A/HISR interface
             l1a_apihisr_com.mp3.command.restart=TRUE;

            // Change state
            *state=WAIT_RESUME_CON;
          }
          break;

          // *-----------------------*
          // * MMI requests MP3 stop *
          // *-----------------------*
          case MMI_MP3_STOP_REQ:
          {
            // Store stop request in L1A/HISR interface
            l1a_apihisr_com.mp3.command.stop=TRUE;

            // Change state
            *state=WAIT_STOP;
          }
          break;

          // *---------------------------------*
          // * MP3 HISR reported an error      *
          // *---------------------------------*
          case API_MP3_STOP_CON:
          {
            // Store MP3 API HISR error code
            l1a.mp3_task.error_code=((T_L1_API_MP3_STOP_CON *)(msg->SigP))->error_code;

            // Request audio peripherals disabling
            l1a_l1s_com.stereopath_drv_task.command.stop=TRUE;

            // Change state
            *state=WAIT_STOP;
          }
          break;
        }

        return;
      }   // case MP3_PAUSE
      break;

      // *******************
      // * WAIT_RESUME_CON *
      // *******************
      case WAIT_RESUME_CON:
      {
        switch(SignalCode)
        {
          // *-----------------------------------*
          // * Resume confirmation from MP3 HISR *
          // *-----------------------------------*
          case API_MP3_RESUME_CON:
          {
            // Send confirmation to upper layers
            l1a_mp3_send_confirmation(MMI_MP3_RESUME_CON,0);

            // Change state
            *state=MP3_PLAY;
          }
          break;

          // *------------------------------------*
          // * Restart confirmation from MP3 HISR *
          // *------------------------------------*
          case API_MP3_RESTART_CON:
          {
            // Send confirmation to upper layers
            l1a_mp3_send_confirmation(MMI_MP3_RESTART_CON,0);

            // Change state
            *state=MP3_PLAY;
          }
          break;

          // *---------------------------------*
          // * MP3 HISR reported an error      *
          // *---------------------------------*
          case API_MP3_STOP_CON:
          {
            // Store MP3 API HISR error code
            l1a.mp3_task.error_code=((T_L1_API_MP3_STOP_CON *)(msg->SigP))->error_code;

            // Request audio peripherals disabling
            l1a_l1s_com.stereopath_drv_task.command.stop=TRUE;

            // Change state
            *state=WAIT_STOP;
          }
          break;
        }

        return;
      }   // case WAIT_RESUME_CON
      break;

      // *************
      // * WAIT_STOP *
      // *************
      case WAIT_STOP:
      {
        // *-------------------------------*
        // * MP3 HISR has finished playing *
        // *-------------------------------*
        if(SignalCode==API_MP3_STOP_CON)
        {
          // Request audio peripherals disabling
          l1a_l1s_com.stereopath_drv_task.command.stop=TRUE;

          // Store MP3 API HISR error code
          l1a.mp3_task.error_code=((T_L1_API_MP3_STOP_CON *)(msg->SigP))->error_code;
        }

        // *-----------------------*
        // * I2S stop confirmation *
        // *-----------------------*
        if(SignalCode==L1_STEREOPATH_DRV_STOP_CON)
        {
          // Request DSP background disabling
          l1a_l1s_com.mp3_task.command.stop=TRUE;
        }

        // *--------------------------------------------------*
        // * L1S confirmation of DSP MP3 background task stop *
        // *--------------------------------------------------*
        if(SignalCode==L1_MP3_DISABLE_CON)
        {
          // Send confirmation to upper layers
          if(l1a.mp3_task.error_code!=C_MP3_ERR_NONE)
            l1a_mp3_send_confirmation(MMI_MP3_STOP_CON,l1a.mp3_task.error_code);
          else
            l1a_mp3_send_confirmation(MMI_MP3_STOP_CON,0);

          // MP3 HISR mustn't be activated
          l1a_apihisr_com.mp3.running=FALSE;

#if ((TRACE_TYPE==1) || (TRACE_TYPE == 4))
          // Enable trace DSP upon MP3 deactivation
          l1_enable_dsp_trace();
#endif

          *state=RESET;
       
          // Change state

        return;
      }
      break;  // case WAIT_STOP
    }   // switch(*state)
  }   // while(1)
}
#endif // L1_DYN_DSP_DWNLD
#endif    // L1_MP3

/************* Revision Controle System Header *************
 *                  GSM Layer 1 software
 * L1AAC_APIHISR.C
 *
 *        Filename l1aac_apihisr.c
 *  Copyright 2004 (C) Texas Instruments
 *
 ************* Revision Controle System Header *************/

#include <stdio.h>
#include <string.h>
#include "l1_confg.h"
#if (OP_RIV_AUDIO == 1)
  #include "rv/rv_general.h"
#endif//omaps00090550
#include "nucleus.h"
#include "sys_types.h"
#include "l1_types.h"
#if (AUDIO_TASK == 1)
  #include "l1audio_cust.h"
  #include "l1audio_defty.h"
  #include "l1audio_const.h"
  #include "l1audio_signa.h"
  #include "l1audio_cust.h"
#endif
#include "l1_const.h"
#if TESTMODE
  #include "l1tm_defty.h"
#endif
#if (L1_GTT == 1)
  #include "l1gtt_const.h"
  #include "l1gtt_defty.h"
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
  #include "l1aac_error.h"
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
#include "cust_os.h"
#include "l1_varex.h"
#include "l1_macro.h"
#include "l1_api_hisr.h"
#include "l1_trace.h"

#if (L1_AAC == 1)

// Return value of l1aac_apihisr_fill_dsp_buffer() function
#define AAC_FILL_OK        0
#define AAC_FILL_END       1
#define AAC_FILL_LOOPBACK  2
#define AAC_FILL_DMA_ERROR 3  // if a critical DMA error has occured during DSP fill buffer

// AAC decoded output buffer size (in words)
#define C_AAC_OUTPUT_BUFFER_SIZE (2048)

// AAC NDB API
extern T_AAC_MCU_DSP *aac_ndb;
extern T_AAC_DMA_PARAM *aac_dma;

// External prototypes
extern UWORD8 Cust_get_pointer_next_buffer(UWORD16 **ptr,UWORD16 *buffer_size,UWORD8 session_id);
extern void Cust_get_pointer_notify(UWORD8 session_id);
extern UWORD8 Cust_get_next_buffer_status(void);

#if(L1_BT_AUDIO ==1)
extern  void l1_audio_manager(UWORD8 *src, UWORD16 size);
#endif
#if(L1_BT_AUDIO ==1)
extern T_L1_BT_AUDIO bt_audio;
#endif



// AAC DMA ping-pong buffers
extern UWORD16  a_aac_dma_input_buffer[][C_AAC_OUTPUT_BUFFER_SIZE]; // L1_BT

// Index for DMA ping-pong buffers
extern UWORD8   d_aac_dma_current_buffer_id;

/*-------------------------------------------------------------*/
/* l1aac_apihisr()                                             */
/*-------------------------------------------------------------*/
/*                                                             */
/* Parameters : none                                           */
/*                                                             */
/* Return     : n/a                                            */
/*                                                             */
/* Description : implements AAC HISR                           */
/*                                                             */
/*-------------------------------------------------------------*/
void l1aac_apihisr()
{
  enum states
  {
    RESET=0,
    WAIT_DSP_INIT,
    WAIT_DATA_REQUEST,
    WAIT_PAUSE_ACK,
    WAIT_RESUME,
    WAIT_STOP_ACK,
    WAIT_END_ACK,
    WAIT_LOOPBACK_ACK
  };

  UWORD8 *state=&l1_apihisr.aac.state;
  static UWORD8 Cust_get_pointer_status;
  static UWORD8 *aac_buffer_ptr,*aac_buffer_end;
  xSignalHeaderRec *conf_msg;
  UWORD32 error_code;

  // Read the info variables (bitrate, channels etc) from API memory
  l1a_apihisr_com.aac.bitrate=(aac_ndb->d_aac_api_bitrate[0]<<16)+aac_ndb->d_aac_api_bitrate[1];
  l1a_apihisr_com.aac.channels=(aac_ndb->d_aac_api_channels);

  // Check error_code from DSP and DMA
  // Do not check for an error in WAIT_STOP_ACK state
  if((*state>RESET) && (*state!=WAIT_STOP_ACK))
  {
    if(l1aac_apihisr_error_handler(&error_code)==TRUE)
    {
      // Critical error occured
      l1_apihisr.aac.error_code=(UWORD32)error_code;

      // Reset the ping-pong DMA input buffers
      l1aac_apihisr_reset_buffer((UWORD16*)&aac_dma->a_aac_dma_input_buffer[0][0], 2*C_AAC_OUTPUT_BUFFER_SIZE);

      // Set the ping-pong buffer index to 1
      d_aac_dma_current_buffer_id=1;

      // Request stop to the DSP
      aac_ndb->d_aac_api_stop=(API)1;

      // Change state
      *state=WAIT_STOP_ACK;

      // Generate API interrupt
      l1_trigger_api_interrupt();

      return;
    }
    else
    {
      if(error_code!=C_AAC_ERR_NONE)
      {
        // Trace non-critical errors
#if (TRACE_TYPE == 1) || (TRACE_TYPE == 4)
        if((trace_info.current_config->l1_dyn_trace) & (1<<L1_DYN_TRACE_AAC))
        {
          char str[25];
          sprintf(str,"AAC warning: 0x%08X\n\r",error_code);
          rvt_send_trace_cpy((T_RVT_BUFFER)str,trace_info.l1_trace_user_id,strlen(str),RVT_ASCII_FORMAT);
        }
#endif    // (TRACE_TYPE == 1) || (TRACE_TYPE == 4)
      }
    }
  }


  // STATE MACHINE
  switch(*state)
  {
    // *********
    // * RESET *
    // *********
    case RESET:
    {
      // *----------------------------*
      // * Start command from the L1A *
      // *----------------------------*
      if(l1a_apihisr_com.aac.command.start==TRUE)
      {
        UWORD16 size;

        // Reset the command
        l1a_apihisr_com.aac.command.start=FALSE;
        l1_apihisr.aac.wait_data = FALSE;

        // Set DSP init command & generate API interrupt
        aac_ndb->d_aac_api_init=(API)1;
        l1_trigger_api_interrupt();

        // Reset the ping-pong DMA input buffers
        l1aac_apihisr_reset_buffer((UWORD16*)&aac_dma->a_aac_dma_input_buffer[0][0], 2*C_AAC_OUTPUT_BUFFER_SIZE);

        // Set the ping-pong buffer index to 1
        d_aac_dma_current_buffer_id=1;

        // Fill initial AAC buffers
        aac_buffer_ptr=NULL;
        Cust_get_pointer_status=Cust_get_pointer_next_buffer((UWORD16 **)&aac_buffer_ptr,&size,l1a_l1s_com.aac_task.parameters.session_id);
        aac_buffer_end=aac_buffer_ptr+size;   // size is in bytes, and we use pointer to 8-bit data

#if (TRACE_TYPE == 1) || (TRACE_TYPE == 4)
        if(size<1648)
        {
          if((trace_info.current_config->l1_dyn_trace) & (1<<L1_DYN_TRACE_AAC))
          {
            char str[]="AAC warning: input buffer may be too small\n\r";
            rvt_send_trace_cpy((T_RVT_BUFFER)str,trace_info.l1_trace_user_id,strlen(str),RVT_ASCII_FORMAT);
          }
        }
#endif

        // Change state
        *state=WAIT_DSP_INIT;
      }
    }   // end case RESET
    break;

    /*****************/
    /* WAIT_DSP_INIT */
    /*****************/
    case WAIT_DSP_INIT:
    {
      // *---------------------------*
      // * Data request from the DSP *
      // *---------------------------*
      if((aac_ndb->d_aac_api_play==0) && (aac_ndb->d_aac_api_request_size>0) && (aac_ndb->d_aac_api_init==1))
      {
        UWORD8 status=l1aac_apihisr_fill_dsp_buffer(&aac_buffer_ptr,&aac_buffer_end,&Cust_get_pointer_status,TRUE);
        switch(status)
        {
          case AAC_FILL_END:
            // No more data, wait for stop acknowledgement from DSP
            *state=WAIT_STOP_ACK;
          break;
          case AAC_FILL_LOOPBACK:
            *state=WAIT_LOOPBACK_ACK;
          break;
        }
        return;
      }

      /*------------------------------------*/
      /* Start acknowledgement from the DSP */
      /*------------------------------------*/
      if(aac_ndb->d_aac_api_init==0)
      {
        // Read AAC format and frequency_index information
        l1a_apihisr_com.aac.aac_format=(aac_ndb->d_aac_api_aac_format);
        l1a_apihisr_com.aac.frequency_index=(aac_ndb->d_aac_api_frequency_index);

        // Send notification to L1A
        conf_msg=os_alloc_sig(0);
        DEBUGMSG(status,NU_ALLOC_ERR)
        conf_msg->SignalCode=API_AAC_START_CON;
        os_send_sig(conf_msg,L1C1_QUEUE);
        DEBUGMSG(status,NU_SEND_QUEUE_ERR)

        // Change state
        *state=WAIT_DATA_REQUEST;
      }
    }   // end case WAIT_DSP_INIT
    break;

    /*********************/
    /* WAIT_DATA_REQUEST */
    /*********************/
    case WAIT_DATA_REQUEST:
    {
      // *---------------------------*
      // * Stop request from the L1A *
      // *---------------------------*
      if(l1a_apihisr_com.aac.command.stop==TRUE)
      {
        // Reset the command
        l1a_apihisr_com.aac.command.stop=FALSE;

        // Reset the ping-pong DMA input buffers
        l1aac_apihisr_reset_buffer((UWORD16*)&aac_dma->a_aac_dma_input_buffer[0][0], 2*C_AAC_OUTPUT_BUFFER_SIZE);

        // Set the ping-pong buffer index to 1
        d_aac_dma_current_buffer_id=1;

        // Request stop to the DSP
        aac_ndb->d_aac_api_stop=(API)1;

        // Change state
        *state=WAIT_STOP_ACK;

        // Generate API interrupt
        l1_trigger_api_interrupt();

        return;
      }

      // *----------------------------*
      // * Pause request from the L1A *
      // *----------------------------*
      if(l1a_apihisr_com.aac.command.pause==TRUE)
      {
        // Reset the command
        l1a_apihisr_com.aac.command.pause=FALSE;

        // Update the DMA input buffer index
        l1aac_apihisr_update_dma_buffer_index(l1_apihisr.aac.dma_csr);

        // Copy the decoded output from API buffer to DMA input buffer
        l1aac_apihisr_api_buffer_copy((UWORD16*)&aac_dma->a_aac_dma_input_buffer[d_aac_dma_current_buffer_id][0],
                                      (API *)aac_ndb->a_aac_api_output_buffer,
                                      C_AAC_OUTPUT_BUFFER_SIZE);
	#if(L1_BT_AUDIO ==1)
	l1_audio_manager((UWORD8 *)&aac_dma->a_aac_dma_input_buffer[d_aac_dma_current_buffer_id][0],C_AAC_OUTPUT_BUFFER_SIZE);
       #endif

        // Proceed further only if there are no critical DMA errors
        if(l1aac_apihisr_dma_error_handler()==FALSE)
        {
          // Request pause to the DSP
          aac_ndb->d_aac_api_pause=(API)1;

          // Change state
          *state=WAIT_PAUSE_ACK;

          // Generate API interrupt
          l1_trigger_api_interrupt();
        }
        else
        {
          // Request stop to the DSP
          aac_ndb->d_aac_api_stop=(API)1;

          // Change state
          *state=WAIT_STOP_ACK;

          // Generate API interrupt
          l1_trigger_api_interrupt();
        }

        return;
      }


      // *---------------------------*
      // * Data request from the DSP *
      // *---------------------------*
      if((aac_ndb->d_aac_api_play==0) &&
         (aac_ndb->d_aac_api_request_size>0))
      {
        // Update the DMA input buffer index
        l1aac_apihisr_update_dma_buffer_index(l1_apihisr.aac.dma_csr);

        // Copy the decoded output from API buffer to DMA input buffer
#if  (OP_L1_STANDALONE == 1) 
        l1aac_apihisr_api_buffer_copy((UWORD16 *)&aac_dma->a_aac_dma_input_buffer[d_aac_dma_current_buffer_id][0],
                                      (API *)aac_ndb->a_aac_api_output_buffer,
                                      C_AAC_OUTPUT_BUFFER_SIZE);





#else
         l1aac_apihisr_api_buffer_copy((UWORD16 *)&aac_dma->a_aac_dma_input_buffer[d_aac_dma_current_buffer_id][0],
                                      (API *)aac_ndb->a_aac_api_output_buffer,
                                      C_AAC_OUTPUT_BUFFER_SIZE);



	
#endif

       #if(L1_BT_AUDIO ==1)
	l1_audio_manager((UWORD8 *)&aac_dma->a_aac_dma_input_buffer[d_aac_dma_current_buffer_id][0],C_AAC_OUTPUT_BUFFER_SIZE);
       #endif

        // Proceed further only if there are no critical DMA errors
        if(l1aac_apihisr_dma_error_handler()==FALSE)
        {
          // Fill input buffer for DSP
          UWORD8 status=l1aac_apihisr_fill_dsp_buffer(&aac_buffer_ptr,&aac_buffer_end,&Cust_get_pointer_status,FALSE);
          switch(status)
          {
            case AAC_FILL_END:
              // No more data, wait for end acknowledgement from DSP
              *state=WAIT_END_ACK;
            break;
            case AAC_FILL_LOOPBACK:
              *state=WAIT_LOOPBACK_ACK;
            break;
            case AAC_FILL_DMA_ERROR:
              // Critical DMA error. Stop request has been sent to DSP
              *state=WAIT_STOP_ACK;
            break;
          }
        }
        else
        {
          // Request stop to the DSP
          aac_ndb->d_aac_api_stop=(API)1;

          // Change state
          *state=WAIT_STOP_ACK;

          // Generate API interrupt
          l1_trigger_api_interrupt();
        }
      }
    }   // end case WAIT_DATA_REQUEST
    break;

    /***************/
    /* WAIT_RESUME */
    /***************/
    case WAIT_RESUME:
    {
      // *---------------------------*
      // * Stop request from the L1A *
      // *---------------------------*
      if(l1a_apihisr_com.aac.command.stop==TRUE)
      {
        // Reset the command
        l1a_apihisr_com.aac.command.stop=FALSE;

        // Reset the ping-pong DMA input buffers
        l1aac_apihisr_reset_buffer((UWORD16 *)&aac_dma->a_aac_dma_input_buffer[0][0], 2*C_AAC_OUTPUT_BUFFER_SIZE);

        // Set the ping-pong buffer index to 1
        d_aac_dma_current_buffer_id=1;

        // Request stop to the DSP
        aac_ndb->d_aac_api_stop=(API)1;

        // Change state
        *state=WAIT_STOP_ACK;

        // Generate API interrupt
        l1_trigger_api_interrupt();

        return;
      }

      // *-----------------------------*
      // * Resume request from the L1A *
      // *-----------------------------*
      if(l1a_apihisr_com.aac.command.resume==TRUE)
      {
        UWORD8 status;

        // Reset the command
        l1a_apihisr_com.aac.command.resume=FALSE;

        // Update the DMA input buffer index
        l1aac_apihisr_update_dma_buffer_index(l1_apihisr.aac.dma_csr);

        // Reset the DMA input buffer
        l1aac_apihisr_reset_buffer((UWORD16 *)&aac_dma->a_aac_dma_input_buffer[d_aac_dma_current_buffer_id][0],C_AAC_OUTPUT_BUFFER_SIZE);
        #if(L1_BT_AUDIO ==1)
	l1_audio_manager((UWORD8 *)&aac_dma->a_aac_dma_input_buffer[d_aac_dma_current_buffer_id][0],C_AAC_OUTPUT_BUFFER_SIZE);
        #endif

        // Fill input buffer for DSP
        status=l1aac_apihisr_fill_dsp_buffer(&aac_buffer_ptr,&aac_buffer_end,&Cust_get_pointer_status,FALSE);

        // Change state
        switch(status)
        {
          case AAC_FILL_END:
            // No more data, wait for end acknowledgement from DSP
            *state=WAIT_END_ACK;
          break;
          case AAC_FILL_LOOPBACK:
            *state=WAIT_LOOPBACK_ACK;
          break;
          case AAC_FILL_DMA_ERROR:
            *state=WAIT_STOP_ACK;
          break;
          default:
            *state=WAIT_DATA_REQUEST;
          break;
        }

        // Send notification to L1A, in case of no critical DMA errors
        // In case of critical DMA error, stop request would have been sent to DSP
        if(status!=AAC_FILL_DMA_ERROR)
        {
          conf_msg=os_alloc_sig(0);
          DEBUGMSG(status,NU_ALLOC_ERR)
          conf_msg->SignalCode=API_AAC_RESUME_CON;
          os_send_sig(conf_msg,L1C1_QUEUE);
          DEBUGMSG(status,NU_SEND_QUEUE_ERR)
        }

        return;
      }

      // *------------------------------*
      // * Restart request from the L1A *
      // *------------------------------*
      if(l1a_apihisr_com.aac.command.restart==TRUE)
      {
        UWORD16 buffer_size;

        // Reset the command
        l1a_apihisr_com.aac.command.restart=FALSE;

        // Update the DMA input buffer index
        l1aac_apihisr_update_dma_buffer_index(l1_apihisr.aac.dma_csr);

        // Reset the DMA input buffer
        l1aac_apihisr_reset_buffer((UWORD16 *)&aac_dma->a_aac_dma_input_buffer[d_aac_dma_current_buffer_id][0],C_AAC_OUTPUT_BUFFER_SIZE);

        // Note: We are not checking for DMA interrupt, as we will be playing out zeroes
        // in WAIT_RESUME state anyway. Also, the DSP will raise an HINT for every API-D
        // in this state

        // Change state
        *state=WAIT_DATA_REQUEST;

        // Request restart to the DSP
        aac_ndb->d_aac_api_restart=(API)1;

        // Reset AAC file
        aac_buffer_ptr=NULL;
        Cust_get_pointer_status=Cust_get_pointer_next_buffer((UWORD16 **)&aac_buffer_ptr,&buffer_size,l1a_l1s_com.aac_task.parameters.session_id);
        aac_buffer_end=aac_buffer_ptr+buffer_size;   // size is in bytes, and we use pointer to 8-bit data

        // Generate API interrupt
        l1_trigger_api_interrupt();

        // Send notification to L1A
        conf_msg=os_alloc_sig(0);
        DEBUGMSG(status,NU_ALLOC_ERR)
        conf_msg->SignalCode=API_AAC_RESTART_CON;
        os_send_sig(conf_msg,L1C1_QUEUE);
        DEBUGMSG(status,NU_SEND_QUEUE_ERR)

        return;
      }

      // Processing the dummy HINT received from DSP during Pause

      // Update the DMA input buffer index
      l1aac_apihisr_update_dma_buffer_index(l1_apihisr.aac.dma_csr);

      // Reset the DMA input buffer
      l1aac_apihisr_reset_buffer((UWORD16 *)&aac_dma->a_aac_dma_input_buffer[d_aac_dma_current_buffer_id][0],C_AAC_OUTPUT_BUFFER_SIZE);
      #if(L1_BT_AUDIO ==1)
	l1_audio_manager((UWORD8 *)&aac_dma->a_aac_dma_input_buffer[d_aac_dma_current_buffer_id][0],C_AAC_OUTPUT_BUFFER_SIZE);
      #endif
    }   // end case WAIT_RESUME
    break;

    // ******************
    // * WAIT_PAUSE_ACK *
    // ******************
    case WAIT_PAUSE_ACK:
    {
      // *------------------------------------*
      // * Pause acknowledgement from the DSP *
      // *------------------------------------*
      if(aac_ndb->d_aac_api_pause==0)
      {
        // Send notification to L1A
        conf_msg=os_alloc_sig(0);
        DEBUGMSG(status,NU_ALLOC_ERR)
        conf_msg->SignalCode=API_AAC_PAUSE_CON;
        os_send_sig(conf_msg,L1C1_QUEUE);
        DEBUGMSG(status,NU_SEND_QUEUE_ERR)

        // Change state
        *state=WAIT_RESUME;
      }
    }   // end case WAIT_PAUSE_ACK
    break;

    // *****************
    // * WAIT_STOP_ACK *
    // *****************
    case WAIT_STOP_ACK:
    {
      // *-----------------------------------*
      // * Stop acknowledgement from the DSP *
      // *-----------------------------------*
      if(aac_ndb->d_aac_api_stop==0)
      {
        // Send notification to L1A
        conf_msg=os_alloc_sig(sizeof(T_L1_API_AAC_STOP_CON));
        DEBUGMSG(status,NU_ALLOC_ERR)
        conf_msg->SignalCode=API_AAC_STOP_CON;
        ((T_L1_API_AAC_STOP_CON *)(conf_msg->SigP))->error_code=l1_apihisr.aac.error_code;
        os_send_sig(conf_msg,L1C1_QUEUE);
        DEBUGMSG(status,NU_SEND_QUEUE_ERR)

        // Change state
        *state=RESET;
      }
    }   // end case WAIT_STOP_ACK
    break;

    // ****************
    // * WAIT_END_ACK *
    // ****************
    case WAIT_END_ACK:
    {
      // *----------------------------------*
      // * End acknowledgement from the DSP *
      // *----------------------------------*
      if(aac_ndb->d_aac_api_end==0)
      {
        // Reset the ping-pong DMA input buffers
        l1aac_apihisr_reset_buffer((UWORD16 *)&aac_dma->a_aac_dma_input_buffer[0][0], 2*C_AAC_OUTPUT_BUFFER_SIZE);

        // Set the DMA ping-pong buffer index to 1
        d_aac_dma_current_buffer_id=1;

        // Send notification to L1A
        conf_msg=os_alloc_sig(0);
        DEBUGMSG(status,NU_ALLOC_ERR)
        conf_msg->SignalCode=API_AAC_STOP_CON;
        ((T_L1_API_AAC_STOP_CON *)(conf_msg->SigP))->error_code=C_AAC_ERR_NONE;
        os_send_sig(conf_msg,L1C1_QUEUE);
        DEBUGMSG(status,NU_SEND_QUEUE_ERR)

        #if(L1_BT_AUDIO ==1)
          if(bt_audio.connected_status == TRUE)
	      bt_audio.pcm_data_end=1;
	#endif	
        // Change state
        *state=RESET;
      }
      else
      {
        // Update the DMA input buffer index
        l1aac_apihisr_update_dma_buffer_index(l1_apihisr.aac.dma_csr);

        // Copy the decoded output from API buffer to DMA input buffer
        l1aac_apihisr_api_buffer_copy((UWORD16 *)&aac_dma->a_aac_dma_input_buffer[d_aac_dma_current_buffer_id][0],
                                      (API *)aac_ndb->a_aac_api_output_buffer,
                                      C_AAC_OUTPUT_BUFFER_SIZE);
       #if(L1_BT_AUDIO ==1)
	l1_audio_manager((UWORD8 *)&aac_dma->a_aac_dma_input_buffer[d_aac_dma_current_buffer_id][0],C_AAC_OUTPUT_BUFFER_SIZE);
       #endif

        // Generate API interrupt only if there are no critical DMA errors
        if(l1aac_apihisr_dma_error_handler()==FALSE)
        {
          // Generate dummy API interrupt
          l1_trigger_api_interrupt();
        }
        else
        {
          // Request stop to the DSP
          aac_ndb->d_aac_api_stop=(API)1;

          // Change state
          *state=WAIT_STOP_ACK;

          // Generate API interrupt
          l1_trigger_api_interrupt();
        }
      }
    }   // end case WAIT_END_ACK
    break;

    case WAIT_LOOPBACK_ACK:
    {
      // *----------------------------------*
      // * End acknowledgement from the DSP *
      // *----------------------------------*
      if(aac_ndb->d_aac_api_end==0)
      {
        UWORD16 size;

        // Reset the ping-pong DMA input buffers
        l1aac_apihisr_reset_buffer((UWORD16 *)&aac_dma->a_aac_dma_input_buffer[0][0], 2*C_AAC_OUTPUT_BUFFER_SIZE);

        // Set the DMA ping-pong buffer index to 1
        d_aac_dma_current_buffer_id=1;

        // Set DSP init command & generate API interrupt
        aac_ndb->d_aac_api_init=(API)1;
        l1_trigger_api_interrupt();

        // Fill initial AAC buffers
        Cust_get_pointer_status=Cust_get_pointer_next_buffer((UWORD16 **)&aac_buffer_ptr,&size,l1a_l1s_com.aac_task.parameters.session_id);
        Cust_get_pointer_notify(l1a_l1s_com.aac_task.parameters.session_id);
        aac_buffer_end=aac_buffer_ptr+size;   // size is in bytes, and we use pointer to 8-bit data

        // Change state
        *state=WAIT_DSP_INIT;
      }
      else
      {
        // Update the DMA input buffer index
        l1aac_apihisr_update_dma_buffer_index(l1_apihisr.aac.dma_csr);

        // Copy the decoded output from API buffer to DMA input buffer
        l1aac_apihisr_api_buffer_copy((UWORD16 *)&aac_dma->a_aac_dma_input_buffer[d_aac_dma_current_buffer_id][0],
                                      (API *)aac_ndb->a_aac_api_output_buffer,
                                      C_AAC_OUTPUT_BUFFER_SIZE);
       #if(L1_BT_AUDIO ==1)
	l1_audio_manager((UWORD8 *)&aac_dma->a_aac_dma_input_buffer[d_aac_dma_current_buffer_id][0],C_AAC_OUTPUT_BUFFER_SIZE);
       #endif  

        // Generate API interrupt only if there are no critical DMA errors
        if(l1aac_apihisr_dma_error_handler()==FALSE)
        {
          // Generate dummy API interrupt
          l1_trigger_api_interrupt();
        }
        else
        {
          // Request stop to the DSP
          aac_ndb->d_aac_api_stop=(API)1;

          // Change state
          *state=WAIT_STOP_ACK;

          // Generate API interrupt
          l1_trigger_api_interrupt();
        }
      }
    }   // end case WAIT_LOOPBACK_ACK
    break;
  }   // switch(*state)
}


/*-------------------------------------------------------------*/
/* l1aac_apihisr_error_handler()                               */
/*-------------------------------------------------------------*/
/*                                                             */
/* Parameters : error_code (OUT) error_code received from DSP  */
/*                                                             */
/* Return     : TRUE if errors are critical, FALSE otherwise   */
/*                                                             */
/* Description : DMA and DSP error handling                    */
/*                                                             */
/*-------------------------------------------------------------*/
BOOL l1aac_apihisr_error_handler(UWORD32 *error_code)
{
  BOOL critical=FALSE;
  UWORD16 dma_csr=0;

#if(L1_BT_AUDIO ==1)
if(bt_audio.connected_status == FALSE)
{
#endif		
  // Read the DMA_CSR value saved by the DMA ISR
  dma_csr=l1_apihisr.aac.dma_csr;

  // Check for DMA errors, if any
  if(dma_csr & C_AAC_DMA_CSR_ERROR_CHECK_MASK)
  {
    if((dma_csr & C_AAC_DMA_CSR_ERROR_CHECK_MASK)==C_AAC_DMA_CSR_DROP)
    {
      *error_code=C_AAC_ERR_DMA_DROP;
    }
    else if((dma_csr & C_AAC_DMA_CSR_ERROR_CHECK_MASK)==C_AAC_DMA_CSR_TOUT_SRC)
    {
      *error_code=C_AAC_ERR_DMA_TOUT_SRC;
    }
    else
    {
      *error_code=C_AAC_ERR_DMA_TOUT_DST;
    }
    #if(L1_BT_AUDIO ==1)
        if(bt_audio.connected_status == TRUE)
          // bt_audio.pcm_data_failed =1;
   #endif
    critical=TRUE;
    return critical;
  }
#if(L1_BT_AUDIO ==1)
}
#endif	

  // Returns DSP error_code
  *error_code=(aac_ndb->d_aac_api_error_code[0]<<16)+aac_ndb->d_aac_api_error_code[1];

  // Acknowledge possible error codes
  if((*error_code)&C_AAC_ERR_NONE)
  {
    aac_ndb->d_aac_api_error_code[1]&=~C_AAC_ERR_NONE;
  }
  if((*error_code)&C_AAC_SYNC_WORD_NOT_FOUND)
  {
    aac_ndb->d_aac_api_error_code[1]&=~C_AAC_SYNC_WORD_NOT_FOUND;
  }
  if((*error_code)&C_AAC_ADTS_HEADER_HAS_INVALID_SYNCWORD)
  {
    aac_ndb->d_aac_api_error_code[1]&=~C_AAC_ADTS_HEADER_HAS_INVALID_SYNCWORD;
  }
  if((*error_code)&C_AAC_ADTS_LAYER_DATA_ERROR)
  {
    aac_ndb->d_aac_api_error_code[1]&=~C_AAC_ADTS_LAYER_DATA_ERROR;
    critical=TRUE;
  }
  if((*error_code)&C_AAC_NUM_CHANNELS_EXCEEDED)
  {
    aac_ndb->d_aac_api_error_code[1]&=~C_AAC_NUM_CHANNELS_EXCEEDED;
    critical=TRUE;
  }
  if((*error_code)&C_AAC_PREDICTION_DETECTED)
  {
    aac_ndb->d_aac_api_error_code[1]&=~C_AAC_PREDICTION_DETECTED;
    critical=TRUE;
  }
  if((*error_code)&C_AAC_LFE_CHANNEL_DETECTED)
  {
    aac_ndb->d_aac_api_error_code[1]&=~C_AAC_LFE_CHANNEL_DETECTED;
    critical=TRUE;
  }
  if((*error_code)&C_AAC_GAIN_CONTROL_DETECTED)
  {
    aac_ndb->d_aac_api_error_code[1]&=~C_AAC_GAIN_CONTROL_DETECTED;
    critical=TRUE;
  }
  if((*error_code)&C_AAC_CHANNEL_ELEMENT_PARSE_ERROR)
  {
    aac_ndb->d_aac_api_error_code[1]&=~C_AAC_CHANNEL_ELEMENT_PARSE_ERROR;
    critical=TRUE;
  }
  if((*error_code)&C_AAC_PULSE_DATA_NOT_ALWD_SHORT_BLK)
  {
    aac_ndb->d_aac_api_error_code[1]&=~C_AAC_PULSE_DATA_NOT_ALWD_SHORT_BLK;
    critical=TRUE;
  }
  if((*error_code)&C_AAC_MAX_SFB_TOO_LARGE_SHORT)
  {
    aac_ndb->d_aac_api_error_code[1]&=~C_AAC_MAX_SFB_TOO_LARGE_SHORT;
    critical=TRUE;
  }
  if((*error_code)&C_AAC_MAX_SFB_TOO_LARGE_LONG)
  {
    aac_ndb->d_aac_api_error_code[1]&=~C_AAC_MAX_SFB_TOO_LARGE_LONG;
    critical=TRUE;
  }
  if((*error_code)&C_AAC_ERROR_ON_DATA_CHANNEL)
  {
    aac_ndb->d_aac_api_error_code[1]&=~C_AAC_ERROR_ON_DATA_CHANNEL;
    critical=TRUE;
  }
  if((*error_code)&C_AAC_COUPLING_CHANNEL_DETECTED)
  {
    aac_ndb->d_aac_api_error_code[1]&=~C_AAC_COUPLING_CHANNEL_DETECTED;
    critical=TRUE;
  }
  if((*error_code)&C_AAC_ADTS_PROFILE_ERROR)
  {
    aac_ndb->d_aac_api_error_code[1]&=~C_AAC_ADTS_PROFILE_ERROR;
    critical=TRUE;
  }
  if((*error_code)&C_AAC_ADIF_PROFILE_ERROR)
  {
    aac_ndb->d_aac_api_error_code[1]&=~C_AAC_ADIF_PROFILE_ERROR;
    critical=TRUE;
  }
  if((*error_code)&C_AAC_INVALID_ELEMENT_ID)
  {
    aac_ndb->d_aac_api_error_code[0]&=~(C_AAC_INVALID_ELEMENT_ID >> 16);
    critical=TRUE;
  }
  if((*error_code)&C_AAC_SAMP_FREQ_NOT_SUPPORTED)
  {
    aac_ndb->d_aac_api_error_code[0]&=~(C_AAC_SAMP_FREQ_NOT_SUPPORTED >> 16);
    critical=TRUE;
  }
  if((*error_code)&C_AAC_ADTS_FRAME_LEN_INCORRECT)
  {
    aac_ndb->d_aac_api_error_code[0]&=~(C_AAC_ADTS_FRAME_LEN_INCORRECT >> 16);
  }
  if((*error_code)&C_AAC_ALG_ERROR)
  {
    aac_ndb->d_aac_api_error_code[0]&=~(C_AAC_ALG_ERROR >> 16);
    critical=TRUE;
  }
  if((*error_code)&C_AAC_CHECK_BUFFER_KO)
  {
    aac_ndb->d_aac_api_error_code[0]&=~(C_AAC_CHECK_BUFFER_KO >> 16);
  }
  #if(L1_BT_AUDIO ==1)
    //if(critical ==TRUE && bt_audio.connected_status == TRUE)
    // bt_audio.pcm_data_failed =1;
  #endif
  return critical;
}


/*---------------------------------------------------------------*/
/* l1aac_apihisr_fill_dsp_buffer()                               */
/*---------------------------------------------------------------*/
/*                                                               */
/* Parameters : aac_buffer_ptr (IN/OUT): pointer to current pos  */
/*                                       in current AAC buffer   */
/*              aac_buffer_end (IN/OUT): pointer to end of       */
/*                                       current AAC buffer      */
/*              Cust_get_pointer_status (IN/OUT): status of data */
/*                        manager (DATA_AVAILABLE or DATA_LAST)  */
/*                                                               */
/*              init:  is decoder in init phase?                 */
/*                                                               */
/* Return     : AAC_FILL_OK                                      */
/*              AAC_FILL_END                                     */
/*              AAC_FILL_LOOPBACK                                */
/*              AAC_FILL_DMA_ERROR                               */
/*                                                               */
/* Description :                                                 */
/*                                                               */
/*---------------------------------------------------------------*/
UWORD8 l1aac_apihisr_fill_dsp_buffer(UWORD8 **aac_buffer_ptr, UWORD8 **aac_buffer_end, UWORD8 *Cust_get_pointer_status, BOOL init)
{
  UWORD16 buffer_size;
  UWORD8  status=AAC_FILL_OK;
  BOOL    loopback_reinit;
  UWORD16 size2;
  UWORD16 size1;
  UWORD8  buf_status;

  if(*aac_buffer_ptr+(2*aac_ndb->d_aac_api_request_size) > *aac_buffer_end)
  {
    size1=(*aac_buffer_end-*aac_buffer_ptr); // in bytes

    if((*Cust_get_pointer_status==DATA_AVAILABLE) || (*Cust_get_pointer_status==WAIT_FOR_DATA))
	{
	  if((*Cust_get_pointer_status==DATA_AVAILABLE))
      {

          // Flush current buffer
         l1aac_apihisr_memcpy((UWORD16 *)aac_ndb->a_aac_api_input_buffer+aac_ndb->d_aac_api_request_index,
                              (UWORD16 *)*aac_buffer_ptr,
                               size1);
      }

       buf_status = Cust_get_next_buffer_status();

	  if(buf_status == TRUE)
	  {
	    l1_apihisr.aac.wait_data = TRUE;
	    *Cust_get_pointer_status=WAIT_FOR_DATA;//omaps00090550
	    return status;
      }


      *Cust_get_pointer_status=Cust_get_pointer_next_buffer((UWORD16 **)aac_buffer_ptr,&buffer_size,l1a_l1s_com.aac_task.parameters.session_id);
      size2=aac_ndb->d_aac_api_request_size*2-size1; // in bytes

      // Get pointer to next buffer
      *aac_buffer_end=*aac_buffer_ptr+buffer_size;   // size is in bytes, and we use pointer to 8-bit data

      loopback_reinit=FALSE;

      if(size2>buffer_size)
      {
        // Last buffer to be played
        size2=buffer_size;
        if(init==TRUE)
          aac_ndb->d_aac_api_stop=(API)1;
        else
          aac_ndb->d_aac_api_end=(API)1;
        if(l1a_l1s_com.aac_task.parameters.loopback==FALSE)
          status=AAC_FILL_END;
        else
        {
          status=AAC_FILL_LOOPBACK;
          loopback_reinit=TRUE;
        }
      }

      // Copy remaining requested data to DSP
      l1aac_apihisr_memcpy((UWORD16 *)aac_ndb->a_aac_api_input_buffer+aac_ndb->d_aac_api_request_index+size1/2,
                           (UWORD16 *)*aac_buffer_ptr,
                           size2);

      *aac_buffer_ptr+=size2;   // size is in bytes, and we use pointer to 8-bit data

      aac_ndb->d_aac_api_provided_size=(size1+size2+1)/2;   // size is in bytes but DSP expect size in words (to round off in case of odd number of bytes)

      // Check whether DMA interrupt has occurred during DSP buffer fill and
      // if it has, check for DMA errors
      if(l1aac_apihisr_dma_error_handler()==FALSE)
      {
        // Set play and generate an MCU->DSP interrupt
        aac_ndb->d_aac_api_play=(API)1;

        // Generate API interrupt
        l1_trigger_api_interrupt();
      }
      else
      {
        // Set status to indicate DMA error
        status=AAC_FILL_DMA_ERROR;

        // Request stop to the DSP
        aac_ndb->d_aac_api_stop=(API)1;

        // Generate API interrupt
        l1_trigger_api_interrupt();
      }

      // Send notification to upper layers
      if((loopback_reinit==TRUE) || (*Cust_get_pointer_status==DATA_AVAILABLE))
        Cust_get_pointer_notify(l1a_l1s_com.aac_task.parameters.session_id);
    }
    else
    {

	  l1aac_apihisr_memcpy((UWORD16 *)aac_ndb->a_aac_api_input_buffer+aac_ndb->d_aac_api_request_index,
                           (UWORD16 *)*aac_buffer_ptr,
                           size1);

      // Last buffer to be played
      aac_ndb->d_aac_api_provided_size=(size1+1)/2;   // size is in bytes, but DSP expects size in words (to round off in case of odd number of bytes)
      if(init==TRUE)
        aac_ndb->d_aac_api_stop=(API)1;
      else
        aac_ndb->d_aac_api_end=(API)1;

      // Check whether DMA interrupt has occurred during DSP buffer fill and
      // if it has, check for DMA errors
      if(l1aac_apihisr_dma_error_handler()==FALSE)
      {
        // Set play and generate an MCU->DSP interrupt
        aac_ndb->d_aac_api_play=(API)1;

        // Generate API interrupt
        l1_trigger_api_interrupt();

        // Send notification to upper layers and update the status
        if(l1a_l1s_com.aac_task.parameters.loopback==TRUE)
        {
          status=AAC_FILL_LOOPBACK;
          Cust_get_pointer_notify(l1a_l1s_com.aac_task.parameters.session_id);
        }
        else
        {
          status=AAC_FILL_END;
        }
      }
      else
      {
        // Set status to indicate DMA error
        status=AAC_FILL_DMA_ERROR;

        // Request stop to the DSP
        aac_ndb->d_aac_api_stop=(API)1;

        // Generate API interrupt
        l1_trigger_api_interrupt();

        // Send notification to upper layers
        if(l1a_l1s_com.aac_task.parameters.loopback==TRUE)
        {
          Cust_get_pointer_notify(l1a_l1s_com.aac_task.parameters.session_id);
        }
      }
    }
  }
  else
  {
    // Current AAC buffer is enough to fill DSP request
    l1aac_apihisr_memcpy((UWORD16 *)aac_ndb->a_aac_api_input_buffer+aac_ndb->d_aac_api_request_index,
                         (UWORD16 *)*aac_buffer_ptr,
                         2*aac_ndb->d_aac_api_request_size);

    *aac_buffer_ptr+=(2*aac_ndb->d_aac_api_request_size); // pointer to 8-bit data

    aac_ndb->d_aac_api_provided_size=aac_ndb->d_aac_api_request_size;

    // Check whether DMA interrupt has occurred during DSP buffer fill and
    // if it has, check for DMA errors
    if(l1aac_apihisr_dma_error_handler()==FALSE)
    {
      // Set play and generate an MCU->DSP interrupt
      aac_ndb->d_aac_api_play=(API)1;

      // Generate API interrupt
      l1_trigger_api_interrupt();
    }
    else
    {
      // Set status to indicate DMA error
      status=AAC_FILL_DMA_ERROR;

      // Request stop to the DSP
      aac_ndb->d_aac_api_stop=(API)1;

      // Generate API interrupt
      l1_trigger_api_interrupt();
    }

  }

  return status;
}


/*---------------------------------------------------------------*/
/* l1aac_apihisr_memcpy()                                        */
/*---------------------------------------------------------------*/
/*                                                               */
/* Parameters : dst: destination pointer                         */
/*              src: source pointer                              */
/*              size: size (in bytes) of the buffer to be copied */
/*                                                               */
/* Return     : n/a                                              */
/*                                                               */
/* Description : Perform a memory copy by applying on the fly a  */
/*               big endian/little endian conversion             */
/*                                                               */
/*---------------------------------------------------------------*/
void l1aac_apihisr_memcpy(UWORD16 *dst, const UWORD16 *src, UWORD16 size)
{
  while(size>1)
  {
    UWORD16 a;

    a=*src++;
    *dst++=((a<<8)|(a>>8));
    size-=2;
  }

  // if number of byte is odd, pad with a zero byte
  if(size==1)
   *dst=(*src)<<8;
}

/*---------------------------------------------------------------*/
/* l1aac_apihisr_api_buffer_copy()                               */
/*---------------------------------------------------------------*/
/*                                                               */
/* Parameters : dst: destination pointer                         */
/*              src: source pointer                              */
/*              size: size (in words) of the buffer to be copied */
/*                                                               */
/* Return     : n/a                                              */
/*                                                               */
/* Description : Perform a copy of the buffer from API memory to */
/*               ARM internal memory. No endianness conversion   */
/*               is performed                                    */
/*                                                               */
/*---------------------------------------------------------------*/
void l1aac_apihisr_api_buffer_copy(UWORD16 *dst, const API *src, UWORD16 size)
{
  while(size>0)
  {
    *dst++=(API)*src++;
    size--;
  }

}

/*---------------------------------------------------------------*/
/* l1aac_apihisr_reset_buffer()                                  */
/*---------------------------------------------------------------*/
/*                                                               */
/* Parameters : dst: destination pointer                         */
/*              size: size (in words) of the buffer to be reset  */
/*                                                               */
/* Return     : n/a                                              */
/*                                                               */
/* Description : Reset 16 bits at a time                         */
/*                                                               */
/*---------------------------------------------------------------*/
void l1aac_apihisr_reset_buffer(UWORD16 *dst, UWORD16 size)
{
  while(size>0)
  {
    *dst++=0;
    size--;
  }

}

/*---------------------------------------------------------------*/
/* l1aac_apihisr_dma_error_handler()                             */
/*---------------------------------------------------------------*/
/*                                                               */
/* Parameters : n/a                                              */
/*                                                               */
/* Return     : TRUE if errors are critical, FALSE otherwise     */
/*                                                               */
/* Description : This checks whether DMA interrupt has occured   */
/*               during state machine processing. If the DMA IT  */
/*               has occured, it checks for critical DMA errors. */
/*               If a critical DMA error has occured, it resets  */
/*               the DMA ping-pong buffers and returns a FALSE   */
/*               value. Else, it updates the buffer index based  */
/*               on DMA half-block or block interrupt and resets */
/*               the DMA buffer corresponding to the buffer index*/
/*                                                               */
/*---------------------------------------------------------------*/
BOOL l1aac_apihisr_dma_error_handler()
{
  BOOL critical=FALSE;
  UWORD32 error_code=C_AAC_ERR_NONE;
  UWORD16 dma_csr=0;
#if(L1_BT_AUDIO ==1)
if(bt_audio.connected_status == FALSE)
{
#endif	
  // Check whether DMA interrupt has occured
  if(aac_ndb->d_aac_api_dma_it_occured)
  {
    // Reset d_aac_api_dma_it_occured variable
    aac_ndb->d_aac_api_dma_it_occured=(API)0;

    // Read the DMA_CSR value saved by the DMA ISR
    dma_csr=l1_apihisr.aac.dma_csr;

    // Check for DMA errors
    if(dma_csr&C_AAC_DMA_CSR_ERROR_CHECK_MASK)
    {
      if((dma_csr&C_AAC_DMA_CSR_ERROR_CHECK_MASK)==C_AAC_DMA_CSR_DROP)
      {
        error_code=C_AAC_ERR_DMA_DROP;
      }
      else if((dma_csr&C_AAC_DMA_CSR_ERROR_CHECK_MASK)==C_AAC_DMA_CSR_TOUT_SRC)
      {
        error_code=C_AAC_ERR_DMA_TOUT_SRC;
      }
      else
      {
        error_code=C_AAC_ERR_DMA_TOUT_DST;
      }

      // Critical error occured
      l1_apihisr.aac.error_code=(UWORD32)error_code;

      // Reset the ping-pong DMA input buffers
      l1aac_apihisr_reset_buffer((UWORD16 *)&aac_dma->a_aac_dma_input_buffer[0][0], 2*C_AAC_OUTPUT_BUFFER_SIZE);

        #if(L1_BT_AUDIO ==1) 
          if(bt_audio.connected_status == TRUE)
           bt_audio.pcm_data_failed =1;
         #endif
      // Set the return value
      critical=TRUE;
    }
    else // no critical DMA error
    {
      // Update the DMA input buffer index
      l1aac_apihisr_update_dma_buffer_index(dma_csr);

      // Reset the DMA input buffer
      l1aac_apihisr_reset_buffer((UWORD16 *)&aac_dma->a_aac_dma_input_buffer[d_aac_dma_current_buffer_id][0], C_AAC_OUTPUT_BUFFER_SIZE);

    } // DMA critical error check

  } // DMA interrupt occured check
#if(L1_BT_AUDIO==1)
}
#endif
  return critical;

} // End of l1aac_apihisr_dma_error_check()

/*---------------------------------------------------------------*/
/* l1aac_apihisr_update_dma_buffer_index()                       */
/*---------------------------------------------------------------*/
/*                                                               */
/* Parameters : dma_csr: DMA CSR register value saved by DMA ISR */
/*                                                               */
/* Return     : n/a                                              */
/*                                                               */
/* Description : Updates the DMA input buffer index based on the */
/*               DMA CSR value                                   */
/*                                                               */
/*---------------------------------------------------------------*/
void l1aac_apihisr_update_dma_buffer_index(UWORD16 dma_csr)
{
UWORD16 d_buffer_id;
#if(L1_BT_AUDIO ==1)
if(bt_audio.connected_status == FALSE)
{
#endif	
  if ( ( (dma_csr & C_AAC_DMA_CSR_HALF_BLOCK) && (d_aac_dma_current_buffer_id == 0) )  ||
       ( (dma_csr & C_AAC_DMA_CSR_BLOCK) && (d_aac_dma_current_buffer_id == 1) )
     )
  {
    d_buffer_id = d_aac_dma_current_buffer_id ^ 1;
       l1aac_apihisr_reset_buffer((UWORD16 *)&aac_dma->a_aac_dma_input_buffer[d_buffer_id][0], C_AAC_OUTPUT_BUFFER_SIZE);

    // As this happens to be a DMA error, we need to reset the API interrupt flag
    aac_ndb->d_aac_api_dma_it_occured=(API)0;
  }
  else
  {
    d_aac_dma_current_buffer_id ^= 1;
    // p_aac_dma_currentBuffer   = &a_aac_dma_input_buffer[d_aac_dma_current_buffer_id][0];
  }
#if(L1_BT_AUDIO ==1)
}
else
	d_aac_dma_current_buffer_id ^= 1;
 #endif
} // End of l1aac_apihisr_update_dma_buffer_index()


#endif  // L1_AAC

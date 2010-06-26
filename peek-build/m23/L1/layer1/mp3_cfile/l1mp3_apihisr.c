/************* Revision Controle System Header *************
 *                  GSM Layer 1 software
 * L1MP3_APIHISR.C
 *
 *        Filename l1mp3_apihisr.c
 *  Copyright 2004 (C) Texas Instruments
 *
 ************* Revision Controle System Header *************/

#if (L1_MP3_SIX_BUFFER == 1)
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
#if (L1_MP3 == 1)
  #include "l1mp3_const.h"
  #include "l1mp3_signa.h"
  #include "l1mp3_defty.h"
  #include "l1mp3_msgty.h"
  #include "l1mp3_error.h"
  #include "l1mp3_proto.h"
#endif
#if (L1_MIDI == 1)
  #include "l1midi_defty.h"
#endif
#include "l1_defty.h"
#include "cust_os.h"
#include "l1_varex.h"
#include "l1_macro.h"
#include "l1_api_hisr.h"
#include "l1_trace.h"

#if (L1_MP3 == 1)

// Return value of l1mp3_apihisr_fill_dsp_buffer() function
#define MP3_FILL_OK       0
#define MP3_FILL_END      1
#define MP3_FILL_LOOPBACK 2
#define MP3_FILL_DMA_ERROR 3  // if a critical DMA error has occured during DSP fill buffer
extern UWORD16 dma_it_occurred;

#ifdef BTS
#ifdef BTS_AV
#define MP3_API_OUTPUT_BUFFER_SIZE  1152

// Value to buffer -1, 0 or 1
#define MP3_OUT_BUFFER_NONE 	  2
#define MP3_OUT_BUFFER_0   		  0
#define MP3_OUT_BUFFER_1   		  1
char  av_buffer_to_read = MP3_OUT_BUFFER_NONE;
unsigned char local_mp3_api_output_buffer0[MP3_API_OUTPUT_BUFFER_SIZE];
unsigned char local_mp3_api_output_buffer1[MP3_API_OUTPUT_BUFFER_SIZE];
extern void PCMSIM_NotifyTxComplete(void);
extern void AVDRV_SendData(char * pcmData, long len);
extern void AVDRV_RequestMoreData(void);

UWORD8  l1_audio_used_buffer = MP3_OUT_BUFFER_0;
#endif
#endif
// MP3 NDB API
extern T_MP3_MCU_DSP *mp3_ndb;
        extern T_MP3_DMA_PARAM *mp3_dma;

// External prototypes
extern UWORD8 Cust_get_pointer_next_buffer(UWORD16 **ptr,UWORD16 *buffer_size,UWORD8 session_id);
extern void Cust_get_pointer_notify(UWORD8 session_id);
extern UWORD8 Cust_get_next_buffer_status(void);
extern UWORD32 mp3_forwardrewind_reset;//MP3-FR- to reset DSP for forward/rewind
UWORD8   d_mp3_dma_current_buffer_id;

extern UWORD16 mp3_dma_csr;
extern UWORD16  a_mp3_dma_input_buffer[MP3_BUFFER_COUNT][C_MP3_OUTPUT_BUFFER_SIZE];
#if(L1_BT_AUDIO ==1)
         extern  void l1_audio_manager(UWORD8 *src, UWORD16 size);
#endif
#if(L1_BT_AUDIO ==1)
      extern T_L1_BT_AUDIO bt_audio;
	UWORD16 pending_dec_req=0;
 #endif

/*-------------------------------------------------------------*/
/* l1mp3_apihisr()                                             */
/*-------------------------------------------------------------*/
/*                                                             */
/* Parameters : none                                           */
/*                                                             */
/* Return     : n/a                                            */
/*                                                             */
/* Description : implements MP3 HISR                           */
/*                                                             */
/*-------------------------------------------------------------*/
void l1mp3_apihisr()
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

  UWORD8 *state=&l1_apihisr.mp3.state;
  static UWORD8 Cust_get_pointer_status;
  static UWORD16 *mp3_buffer_ptr,*mp3_buffer_end;
  xSignalHeaderRec *conf_msg;
  UWORD16 error_code;


  // Update MP3 frame header for L1A
  l1a_apihisr_com.mp3.header=(mp3_ndb->d_mp3_api_header[0]<<16)+mp3_ndb->d_mp3_api_header[1];
  l1_apihisr.mp3.dma_it_occurred=dma_it_occurred;
  l1_apihisr.mp3.mp3_dma_csr=mp3_dma_csr;
  dma_it_occurred=0;

   // Check error_code from DSP and DMA
  // Do not check for an error in WAIT_STOP_ACK state
  if((*state>RESET) && (*state!=WAIT_STOP_ACK))
  {
    if(l1mp3_apihisr_error_handler(&error_code)==TRUE)
    {
      // Critical error occured
      l1_apihisr.mp3.error_code=(UWORD32)error_code;
// Reset the ping-pong DMA input buffers
	      l1mp3_apihisr_reset_buffer(&mp3_dma->a_mp3_dma_input_buffer[0][0], MP3_BUFFER_COUNT*C_MP3_OUTPUT_BUFFER_SIZE);
      // Request stop to the DSP
      mp3_ndb->d_mp3_api_stop=(API)1;
      d_mp3_dma_current_buffer_id=MP3_BUFFER_COUNT-1;
      // Change state
      *state=WAIT_STOP_ACK;

      // Generate API interrupt
      l1_trigger_api_interrupt();

      return;
    }
    else
    {
      if(error_code!=C_MP3_ERR_NONE)
      {
        // Trace non-critical errors
#if (TRACE_TYPE == 1) || (TRACE_TYPE == 4)
        if((trace_info.current_config->l1_dyn_trace) & (1<<L1_DYN_TRACE_MP3))
        {
          char str[25];
          sprintf(str,"MP3 warning: 0x%04X\n\r",error_code);
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
#if 0
#ifdef BTS
#ifdef BTS_AV
	/* Reset the audio buffer used to mp3 out buffer 0 */
	l1_audio_used_buffer 	= MP3_OUT_BUFFER_0;
	av_buffer_to_read         	= MP3_OUT_BUFFER_NONE;
#endif
#endif
#endif
      // *----------------------------*
      // * Start command from the L1A *
      // *----------------------------*
      if(l1a_apihisr_com.mp3.command.start==TRUE)
      {
        UWORD16 size;

        // Reset the command
        l1a_apihisr_com.mp3.command.start=FALSE;
        l1_apihisr.mp3.wait_data = FALSE;
        // Reset the ping-pong DMA input buffers
	      l1mp3_apihisr_reset_buffer((UWORD16 *)&(mp3_dma->a_mp3_dma_input_buffer[0][0]), MP3_BUFFER_COUNT*C_MP3_OUTPUT_BUFFER_SIZE);
        // Set the ping-pong buffer index to 5
        d_mp3_dma_current_buffer_id=MP3_BUFFER_COUNT-1;
	        if(bt_audio.connected_status == TRUE)
	        d_mp3_dma_current_buffer_id=0;

        // Store the DMA channel in the MP3 NDB
        mp3_ndb->d_mp3_api_channel=l1a_apihisr_com.mp3.dma_channel_number;

        // Set DSP init command & generate API interrupt
        mp3_ndb->d_mp3_api_init=(API)1;
        l1_trigger_api_interrupt();

        // Fill initial MP3 buffers
        mp3_buffer_ptr=NULL;
        Cust_get_pointer_status=Cust_get_pointer_next_buffer(&mp3_buffer_ptr,&size,l1a_l1s_com.mp3_task.parameters.session_id);
        mp3_buffer_end=mp3_buffer_ptr+size/2;   // size is in bytes, but we use 16-bit pointer

#if (TRACE_TYPE == 1) || (TRACE_TYPE == 4)
        if(size<1600)
        {
          if((trace_info.current_config->l1_dyn_trace) & (1<<L1_DYN_TRACE_MP3))
          {
            char str[]="MP3 warning: input buffer may be too small\n\r";
            rvt_send_trace_cpy((T_RVT_BUFFER)str,trace_info.l1_trace_user_id,strlen(str),RVT_ASCII_FORMAT);
          }
        }
#endif

        // Change state
        *state=WAIT_DSP_INIT;
      }

      return;
    }   // end case RESET
//omaps00090550    break;

    /*****************/
    /* WAIT_DSP_INIT */
    /*****************/
    case WAIT_DSP_INIT:
    {
      // *---------------------------*
      // * Data request from the DSP *
      // *---------------------------*
      if((mp3_ndb->d_mp3_api_play==0)&&(mp3_ndb->d_mp3_api_request_size>0) &&  (mp3_ndb->d_mp3_api_init==1) )
      {
        UWORD8 status=l1mp3_apihisr_fill_dsp_buffer(&mp3_buffer_ptr,&mp3_buffer_end,&Cust_get_pointer_status,TRUE);

         error_code = C_MP3_CHECK_BUFFER_KO;

#if (TRACE_TYPE == 1) || (TRACE_TYPE == 4)
		if((trace_info.current_config->l1_dyn_trace) & (1<<L1_DYN_TRACE_MP3))
		{
		   char str[25];
		   sprintf(str,"MP3 warning: 0x%04X\n\r",error_code);
		   rvt_send_trace_cpy((T_RVT_BUFFER)str,trace_info.l1_trace_user_id,strlen(str),RVT_ASCII_FORMAT);
		}
#endif    // (TRACE_TYPE == 1) || (TRACE_TYPE == 4)

        switch(status)
        {
          case MP3_FILL_END:
            // No more data, wait for stop acknowledgement from DSP
            *state=WAIT_STOP_ACK;
          break;
          case MP3_FILL_LOOPBACK:
            *state=WAIT_LOOPBACK_ACK;
          break;
        }
        return;
      }

      /*------------------------------------*/
      /* Start acknowledgement from the DSP */
      /*------------------------------------*/
      if(mp3_ndb->d_mp3_api_init==0)
      {
        // Send notification to L1A
        conf_msg=os_alloc_sig(0);
        DEBUGMSG(status,NU_ALLOC_ERR)
        conf_msg->SignalCode=API_MP3_START_CON;
        os_send_sig(conf_msg,L1C1_QUEUE);
        DEBUGMSG(status,NU_SEND_QUEUE_ERR)

        // Change state
        *state=WAIT_DATA_REQUEST;
      }

      return;
    }   // end case WAIT_DSP_INIT
//omaps00090550    break;

    /*********************/
    /* WAIT_DATA_REQUEST */
    /*********************/
    case WAIT_DATA_REQUEST:
    {
#if 0
#ifdef BTS
#ifdef BTS_AV		
	/* ORDINA : Copy the out buffer to local buffer */
	/* Is this the correct place to do this??? */
	if (l1_audio_used_buffer == MP3_OUT_BUFFER_0)
	{
		/* copy the buffer a_mp3_api_output_buffer0 to local buffer */
//        memcpy(local_mp3_api_output_buffer0, (void *)mp3_ndb->a_mp3_api_output_buffer0, 1152);

		AVDRV_SendData((char*)mp3_ndb->a_mp3_api_output_buffer0, 1152);

		l1_audio_used_buffer = MP3_OUT_BUFFER_1;
	}
	else if (l1_audio_used_buffer == MP3_OUT_BUFFER_1)
	{
		/* copy the buffer a_mp3_api_output_buffer1 to local buffer */
//        memcpy(local_mp3_api_output_buffer1, (void *)mp3_ndb->a_mp3_api_output_buffer1, 1152);

		AVDRV_SendData((char*)mp3_ndb->a_mp3_api_output_buffer1, 1152);

		l1_audio_used_buffer = MP3_OUT_BUFFER_0;	
	}
	/* notify PCMSIM that there is data waiting to be sent */
//	PCMSIM_NotifyTxComplete();
	//AVDRV_RequestMoreData();

//ORDINA toggle buffer
      //l1_audio_used_buffer = ~l1_audio_used_buffer;
#endif
#endif
#endif		
      // *---------------------------*
      // * Stop request from the L1A *
      // *---------------------------*

//Nina added for testing Huawei camera mp3 issue
//if(!(( mp3_ndb->d_mp3_ver_id==C_MP3_MPEG1)&&( mp3_ndb->d_mp3_frame_decoded==1)))
{
      if(l1a_apihisr_com.mp3.command.stop==TRUE)
      {

        UWORD8 mp3_buf_to_reset;
        // Reset the command
        l1a_apihisr_com.mp3.command.stop=FALSE;
	        l1mp3_apihisr_reset_buffer((UWORD16 *)&mp3_dma->a_mp3_dma_input_buffer[0][0], MP3_BUFFER_COUNT*C_MP3_OUTPUT_BUFFER_SIZE);
        d_mp3_dma_current_buffer_id=MP3_BUFFER_COUNT-1;
        // Request stop to the DSP
        mp3_ndb->d_mp3_api_stop=(API)1;

        // Change state
        *state=WAIT_STOP_ACK;

        // Generate API interrupt
        l1_trigger_api_interrupt();
        mp3_ndb->d_mp3_ver_id = (API)0;

        return;
      }

      // *----------------------------*
      // * Pause request from the L1A *
      // *----------------------------*
      if(l1a_apihisr_com.mp3.command.pause==TRUE)
      {
        // Reset the command
        l1a_apihisr_com.mp3.command.pause=FALSE;

            l1mp3_apihisr_update_dma_buffer_index(l1_apihisr.mp3.mp3_dma_csr);
	     l1_apihisr.mp3.dma_it_occurred=0;
#if (OP_L1_STANDALONE == 0)
            // Copy the decoded output from API buffer to DMA input buffer
	            l1mp3_apihisr_api_buffer_copy(&mp3_dma->a_mp3_dma_input_buffer[d_mp3_dma_current_buffer_id][0],
	                                          (API *)mp3_ndb->a_mp3_api_output_buffer0,
	                                          C_MP3_OUTPUT_BUFFER_SIZE);
#else
// Copy the decoded output from API buffer to DMA input buffer
	            l1mp3_apihisr_api_buffer_copy(&mp3_dma->a_mp3_dma_input_buffer[d_mp3_dma_current_buffer_id][0],
	                                          (API *)mp3_ndb->a_mp3_api_output_buffer0,
	                                          C_MP3_OUTPUT_BUFFER_SIZE);
            
#endif
	            
	             
	            
               #if(L1_BT_AUDIO ==1)||(L1_WCM ==1)
	        l1_audio_manager((UWORD8 *)&mp3_dma->a_mp3_dma_input_buffer[d_mp3_dma_current_buffer_id][0],C_MP3_OUTPUT_BUFFER_SIZE);
               #endif      
            // Proceed further only if there are no critical DMA errors
        if(l1mp3_apihisr_dma_error_handler()==FALSE)
        {
          // Request pause to the DSP
        mp3_ndb->d_mp3_api_pause=(API)1;

        // Change state
        *state=WAIT_PAUSE_ACK;

        // Generate API interrupt
        l1_trigger_api_interrupt();
        }
        else
        {
             // Request stop to the DSP
             mp3_ndb->d_mp3_api_stop=(API)1;

        // Change state
          *state=WAIT_STOP_ACK;

        // Generate API interrupt
        l1_trigger_api_interrupt();
                }
        return;
      }
}

      // *---------------------------*
      // * Data request from the DSP *
      // *---------------------------*
      if((mp3_ndb->d_mp3_api_play==0))
      {

        if((l1_apihisr.mp3.dma_it_occurred==0 ))
		l1_apihisr.mp3.dsp_hint_occurred=1;
			
     #if(L1_BT_AUDIO==1)
	if(bt_audio.connected_status == FALSE)
        {
	#endif		
	if(( l1_apihisr.mp3.dsp_hint_occurred==1) && ((l1_apihisr.mp3.dma_it_occurred==1)|| ((d_mp3_dma_current_buffer_id+1)%(MP3_BUFFER_COUNT/2)!=0)))
       {               
                    

		UWORD8 status;
		
		l1mp3_apihisr_update_dma_buffer_index(l1_apihisr.mp3.mp3_dma_csr);
#if (OP_L1_STANDALONE == 0) 	   
	       // Copy the decoded output from API buffer to DMA input buffer
	        l1mp3_apihisr_api_buffer_copy((UWORD16 *)&mp3_dma->a_mp3_dma_input_buffer[d_mp3_dma_current_buffer_id][0],
	                                          (API *)mp3_ndb->a_mp3_api_output_buffer0,
	                                         C_MP3_OUTPUT_BUFFER_SIZE);
		
	     
#else

		       // Copy the decoded output from API buffer to DMA input buffer
	            l1mp3_apihisr_api_buffer_copy(&mp3_dma->a_mp3_dma_input_buffer[d_mp3_dma_current_buffer_id][0],
	                                          (API *)mp3_ndb->a_mp3_api_output_buffer0,
	                                          C_MP3_OUTPUT_BUFFER_SIZE);

#endif	
       l1_apihisr.mp3.dma_it_occurred=0;
       l1_apihisr.mp3.dsp_hint_occurred=0;
#if(L1_BT_AUDIO ==1)
	l1_audio_manager((UWORD8 *)&mp3_dma->a_mp3_dma_input_buffer[d_mp3_dma_current_buffer_id][0],C_MP3_OUTPUT_BUFFER_SIZE);
#endif
	if(mp3_forwardrewind_reset==1)
	{
	/*Reset the DSP CODEC if FORWARD/REWIND REQUEST has come
	mp3_forwardrewind_reset =0;//global var to check the forward rewind params
	// Request restart to the DSP
        mp3_ndb->d_mp3_api_restart=(API)1;

        // Generate API interrupt
        l1_trigger_api_interrupt();
	*/
	}
// Proceed further only if there are no critical DMA errors
if(l1mp3_apihisr_dma_error_handler()==FALSE)
   {
      if( mp3_ndb->d_mp3_api_request_size!=0)
        {
          status=l1mp3_apihisr_fill_dsp_buffer(&mp3_buffer_ptr,&mp3_buffer_end,&Cust_get_pointer_status,FALSE);
                    if( Cust_get_pointer_status==WAIT_FOR_DATA)
			  l1_apihisr.mp3.dsp_hint_occurred=1;
        }
      else  
       {	
          mp3_ndb->d_mp3_api_play=(API)1;
         // Generate API interrupt
         l1_trigger_api_interrupt();
        }
		
switch(status)
	        {
	          case MP3_FILL_END:
	            // No more data, wait for end acknowledgement from DSP
	            *state=WAIT_END_ACK;
	          break;
	          case MP3_FILL_LOOPBACK:
	            *state=WAIT_LOOPBACK_ACK;
	          break;
	          case MP3_FILL_DMA_ERROR:
	              // Critical DMA error. Stop request has been sent to DSP
	              *state=WAIT_STOP_ACK;
	            break;
	        }
	}
	else
	        {
	          // Request stop to the DSP
	          mp3_ndb->d_mp3_api_stop=(API)1;

	          // Change state
	          *state=WAIT_STOP_ACK;

	          // Generate API interrupt
	          l1_trigger_api_interrupt();
	        }			
	        }
#if(L1_BT_AUDIO==1)
	}
	else
	{
			  
	   if((l1_apihisr.mp3.dsp_hint_occurred==1)&&( Cust_get_pointer_status!=WAIT_FOR_DATA))
               {
				if((bt_audio.pcm_data_ready==0))
               {

			l1mp3_apihisr_update_dma_buffer_index(l1_apihisr.mp3.mp3_dma_csr);
		       // Copy the decoded output from API buffer to DMA input buffer


			//l1mp3_apihisr_reset_buffer(&mp3_dma->a_mp3_dma_input_buffer[d_mp3_dma_current_buffer_id][0],C_MP3_OUTPUT_BUFFER_SIZE);
		     //else
	            l1mp3_apihisr_api_buffer_copy((UWORD16 *)mp3_dma->a_mp3_dma_input_buffer[d_mp3_dma_current_buffer_id],
	                                          (API *)mp3_ndb->a_mp3_api_output_buffer0,
	                                          C_MP3_OUTPUT_BUFFER_SIZE);

			#if(L1_BT_AUDIO ==1)||(L1_WCM ==1)
	              l1_audio_manager((UWORD8 *)&mp3_dma->a_mp3_dma_input_buffer[d_mp3_dma_current_buffer_id][0],C_MP3_OUTPUT_BUFFER_SIZE);
			#endif
}


	   }
	
		if((l1_apihisr.mp3.dma_it_occurred==1))
               {
			UWORD8 status;
			//pending_dec_req--;
		if(mp3_forwardrewind_reset==1)
		{
		//Reset the DSP CODEC if FORWARD/REWIND REQUEST has come
		//mp3_forwardrewind_reset =0;//global var to check the forward rewind params
		// Request restart to the DSP
	      //  mp3_ndb->d_mp3_api_restart=(API)1;

	        // Generate API interrupt
	      //  l1_trigger_api_interrupt();
		}
	// Proceed further only if there are no critical DMA errors
	if(l1mp3_apihisr_dma_error_handler()==FALSE)
	   {
if ( l1_apihisr.mp3.dsp_hint_occurred==1)
	{
	      if( mp3_ndb->d_mp3_api_request_size!=0)
        {
          status=l1mp3_apihisr_fill_dsp_buffer(&mp3_buffer_ptr,&mp3_buffer_end,&Cust_get_pointer_status,FALSE);
          if( Cust_get_pointer_status==WAIT_FOR_DATA)
			      l1_apihisr.mp3.dsp_hint_occurred=1;
        }
	      else  
	       {	
	          mp3_ndb->d_mp3_api_play=(API)1;
	         // Generate API interrupt
	         l1_trigger_api_interrupt();
	        }
		   if( Cust_get_pointer_status==WAIT_FOR_DATA)
			      l1_apihisr.mp3.dsp_hint_occurred=1;
		   else
   l1_apihisr.mp3.dsp_hint_occurred=0;
	}
        switch(status)
        {
          case MP3_FILL_END:
            // No more data, wait for end acknowledgement from DSP
            *state=WAIT_END_ACK;
          break;
          case MP3_FILL_LOOPBACK:
            *state=WAIT_LOOPBACK_ACK;
          break;
          case MP3_FILL_DMA_ERROR:
              // Critical DMA error. Stop request has been sent to DSP
              *state=WAIT_STOP_ACK;
            break;
        }
}
else
        {
          // Request stop to the DSP
          mp3_ndb->d_mp3_api_stop=(API)1;

          // Change state
          *state=WAIT_STOP_ACK;

          // Generate API interrupt
          l1_trigger_api_interrupt();
        }			
        }
 l1_apihisr.mp3.dma_it_occurred=0;
	}
#endif
	if(( l1_apihisr.mp3.dsp_hint_occurred==0) && (l1_apihisr.mp3.dma_it_occurred==1))
	{
	  dma_it_occurred=1;
          error_code = C_MP3_DECODING_DELAY;
           #if (TRACE_TYPE == 1) || (TRACE_TYPE == 4)
		if((trace_info.current_config->l1_dyn_trace) & (1<<L1_DYN_TRACE_MP3))
		{
		   char str[25];
		   sprintf(str,"MP3 warning: 0x%04X\n\r",error_code);
		   rvt_send_trace_cpy((T_RVT_BUFFER)str,trace_info.l1_trace_user_id,strlen(str),RVT_ASCII_FORMAT);
		}
           #endif    // (TRACE_TYPE == 1) || (TRACE_TYPE == 4)
        }
      }

      return;
    }   // end case WAIT_DATA_REQUEST
//omaps00090550    break;

    /***************/
    /* WAIT_RESUME */
    /***************/
    case WAIT_RESUME:
    {
      // *---------------------------*
      // * Stop request from the L1A *
      // *---------------------------*
      if(l1a_apihisr_com.mp3.command.stop==TRUE)
      {
        // Reset the command
        l1a_apihisr_com.mp3.command.stop=FALSE;

         // Reset the ping-pong DMA input buffers
	        l1mp3_apihisr_reset_buffer(&mp3_dma->a_mp3_dma_input_buffer[0][0], MP3_BUFFER_COUNT*C_MP3_OUTPUT_BUFFER_SIZE);

        // reset the ping-pong buffer index
        d_mp3_dma_current_buffer_id=MP3_BUFFER_COUNT-1;
       
        // Request stop to the DSP
        mp3_ndb->d_mp3_api_stop=(API)1;

        // Change state
        *state=WAIT_STOP_ACK;

        // Generate API interrupt
        l1_trigger_api_interrupt();

        return;
      }

      // *-----------------------------*
      // * Resume request from the L1A *
      // *-----------------------------*
      if(l1a_apihisr_com.mp3.command.resume==TRUE)
      {
        UWORD8 status;

        // Reset the command
        l1a_apihisr_com.mp3.command.resume=FALSE;

   // Update the DMA input buffer index
        l1mp3_apihisr_update_dma_buffer_index(l1_apihisr.mp3.mp3_dma_csr);
        l1_apihisr.mp3.dma_it_occurred=0;
        // Reset the DMA input buffer
	       // l1mp3_apihisr_reset_buffer(&mp3_dma->a_mp3_dma_input_buffer[d_mp3_dma_current_buffer_id][0],(MP3_BUFFER_COUNT/2)*C_MP3_OUTPUT_BUFFER_SIZE);
		#if(L1_BT_AUDIO ==1)||(L1_WCM ==1)
	        l1_audio_manager((UWORD8 *)&mp3_dma->a_mp3_dma_input_buffer[d_mp3_dma_current_buffer_id][0],C_MP3_OUTPUT_BUFFER_SIZE);
               #endif      
       d_mp3_dma_current_buffer_id +=((MP3_BUFFER_COUNT/2)-1);
        // Fill input buffer for DSP
        status=l1mp3_apihisr_fill_dsp_buffer(&mp3_buffer_ptr,&mp3_buffer_end,&Cust_get_pointer_status,FALSE);

        // Change state
        switch(status)
        {
          case MP3_FILL_END:
            // No more data, wait for end acknowledgement from DSP
            *state=WAIT_END_ACK;
          break;
          case MP3_FILL_LOOPBACK:
            *state=WAIT_LOOPBACK_ACK;
          break;
          case MP3_FILL_DMA_ERROR:
            *state=WAIT_STOP_ACK;
          break;
          default:
            *state=WAIT_DATA_REQUEST;
          break;
        }

        // Send notification to L1A, in case of no critical DMA errors
        // In case of critical DMA error, stop request would have been sent to DSP
        if(status!=MP3_FILL_DMA_ERROR)
        {
        conf_msg=os_alloc_sig(0);
        DEBUGMSG(status,NU_ALLOC_ERR)
        conf_msg->SignalCode=API_MP3_RESUME_CON;
        os_send_sig(conf_msg,L1C1_QUEUE);
        DEBUGMSG(status,NU_SEND_QUEUE_ERR)
        }

        return;
      }

      // *------------------------------*
      // * Restart request from the L1A *
      // *------------------------------*
      if(l1a_apihisr_com.mp3.command.restart==TRUE)
      {
        UWORD16 buffer_size;

        // Reset the command
        l1a_apihisr_com.mp3.command.restart=FALSE;
        // Update the DMA input buffer index
        l1mp3_apihisr_update_dma_buffer_index(l1_apihisr.mp3.mp3_dma_csr);
        l1_apihisr.mp3.dma_it_occurred=0;

	 // Reset the DMA input buffer
		if (d_mp3_dma_current_buffer_id < (MP3_BUFFER_COUNT/2))
	        l1mp3_apihisr_reset_buffer(&mp3_dma->a_mp3_dma_input_buffer[d_mp3_dma_current_buffer_id][0],(MP3_BUFFER_COUNT/2)*C_MP3_OUTPUT_BUFFER_SIZE);
        d_mp3_dma_current_buffer_id += ((MP3_BUFFER_COUNT/2)-1);
        // Note: We are not checking for DMA interrupt, as we will be playing out zeroes
        // in WAIT_RESUME state anyway. Also, the DSP will raise an HINT for every API-D
        // in this state

        // Change state
        *state=WAIT_DATA_REQUEST;

        // Request restart to the DSP
        mp3_ndb->d_mp3_api_restart=(API)1;

        // Reset MP3 file
        mp3_buffer_ptr=NULL;
        Cust_get_pointer_status=Cust_get_pointer_next_buffer(&mp3_buffer_ptr,&buffer_size,l1a_l1s_com.mp3_task.parameters.session_id);
        mp3_buffer_end=mp3_buffer_ptr+buffer_size/2;   // size is in bytes, but we use 16-bit pointer

        // Generate API interrupt
        l1_trigger_api_interrupt();

        // Send notification to L1A
        conf_msg=os_alloc_sig(0);
        DEBUGMSG(status,NU_ALLOC_ERR)
        conf_msg->SignalCode=API_MP3_RESTART_CON;
        os_send_sig(conf_msg,L1C1_QUEUE);
        DEBUGMSG(status,NU_SEND_QUEUE_ERR)

        return;
      }
      // Processing the dummy HINT received from DSP during Pause
      l1_apihisr.mp3.dma_it_occurred=0;
      // Update the DMA input buffer index
      l1mp3_apihisr_update_dma_buffer_index(l1_apihisr.mp3.mp3_dma_csr);
      // Reset the DMA input buffer
	      if (d_mp3_dma_current_buffer_id < (MP3_BUFFER_COUNT/2))
	      l1mp3_apihisr_reset_buffer(&mp3_dma->a_mp3_dma_input_buffer[0][0],(MP3_BUFFER_COUNT/2)*C_MP3_OUTPUT_BUFFER_SIZE);
	      else
	      l1mp3_apihisr_reset_buffer(&mp3_dma->a_mp3_dma_input_buffer[(MP3_BUFFER_COUNT/2)][0],(MP3_BUFFER_COUNT/2)*C_MP3_OUTPUT_BUFFER_SIZE);


	       #if(L1_BT_AUDIO ==1)||(L1_WCM ==1)
	        l1_audio_manager((UWORD8 *)&mp3_dma->a_mp3_dma_input_buffer[d_mp3_dma_current_buffer_id][0],C_MP3_OUTPUT_BUFFER_SIZE);
              #endif      
      d_mp3_dma_current_buffer_id += ((MP3_BUFFER_COUNT/2)-1);
      return;
    }   // end case WAIT_RESUME
//omaps00090550    break;

    // ******************
    // * WAIT_PAUSE_ACK *
    // ******************
    case WAIT_PAUSE_ACK:
    {
      // *------------------------------------*
      // * Pause acknowledgement from the DSP *
      // *------------------------------------*
      if(mp3_ndb->d_mp3_api_pause==0)
      {
	UWORD8 mp3_buf_to_reset;
        // Send notification to L1A
        conf_msg=os_alloc_sig(0);
        DEBUGMSG(status,NU_ALLOC_ERR)
        conf_msg->SignalCode=API_MP3_PAUSE_CON;
        os_send_sig(conf_msg,L1C1_QUEUE);
        DEBUGMSG(status,NU_SEND_QUEUE_ERR)

	if(d_mp3_dma_current_buffer_id>=(MP3_BUFFER_COUNT/2))
	mp3_buf_to_reset = (MP3_BUFFER_COUNT-1)-d_mp3_dma_current_buffer_id;
	else
	 mp3_buf_to_reset=((MP3_BUFFER_COUNT/2)-1)-d_mp3_dma_current_buffer_id;

		l1mp3_apihisr_reset_buffer(&mp3_dma->a_mp3_dma_input_buffer[d_mp3_dma_current_buffer_id+1][0],mp3_buf_to_reset*C_MP3_OUTPUT_BUFFER_SIZE);
               #if(L1_BT_AUDIO ==1)||(L1_WCM ==1)
	        l1_audio_manager((UWORD8 *)&mp3_dma->a_mp3_dma_input_buffer[d_mp3_dma_current_buffer_id][0],C_MP3_OUTPUT_BUFFER_SIZE);
               #endif      
       d_mp3_dma_current_buffer_id+=mp3_buf_to_reset;
       l1mp3_apihisr_reset_outbuffer((API *)mp3_ndb->a_mp3_api_output_buffer0, C_MP3_OUTPUT_BUFFER_SIZE);
        // Change state
        *state=WAIT_RESUME;
      }

      return;
    }   // end case WAIT_PAUSE_ACK
//omaps00090550    break;

    // *****************
    // * WAIT_STOP_ACK *
    // *****************
    case WAIT_STOP_ACK:
    {
      // *-----------------------------------*
      // * Stop acknowledgement from the DSP *
      // *-----------------------------------*
      if(mp3_ndb->d_mp3_api_stop==0)
      {
        // Send notification to L1A
        conf_msg=os_alloc_sig(sizeof(T_L1_API_MP3_STOP_CON));
        DEBUGMSG(status,NU_ALLOC_ERR)
        conf_msg->SignalCode=API_MP3_STOP_CON;
        ((T_L1_API_MP3_STOP_CON *)(conf_msg->SigP))->error_code=l1_apihisr.mp3.error_code;
        os_send_sig(conf_msg,L1C1_QUEUE);
        DEBUGMSG(status,NU_SEND_QUEUE_ERR)

        // Change state
        *state=RESET;
      }
      return;
    }   // end case WAIT_STOP_ACK
//omaps00090550    break;

    // ****************
    // * WAIT_END_ACK *
    // ****************
    case WAIT_END_ACK:
    {
      // *----------------------------------*
      // * End acknowledgement from the DSP *
      // *----------------------------------*
      if(mp3_ndb->d_mp3_api_end==0)
      {
        // Reset the ping-pong DMA input buffers
	        l1mp3_apihisr_reset_buffer(&mp3_dma->a_mp3_dma_input_buffer[0][0], MP3_BUFFER_COUNT*C_MP3_OUTPUT_BUFFER_SIZE);

        // Set the DMA ping-pong buffer index to 5
        d_mp3_dma_current_buffer_id=MP3_BUFFER_COUNT-1;
		 	if((bt_audio.connected_status==TRUE))
		 	{
				bt_audio.pcm_data_end=1;
				l1_audio_manager((UWORD8 *)&mp3_dma->a_mp3_dma_input_buffer[d_mp3_dma_current_buffer_id][0],C_MP3_OUTPUT_BUFFER_SIZE);

			}
        // Send notification to L1A
        conf_msg=os_alloc_sig(0);
        DEBUGMSG(status,NU_ALLOC_ERR)
        conf_msg->SignalCode=API_MP3_STOP_CON;
        ((T_L1_API_MP3_STOP_CON *)(conf_msg->SigP))->error_code=C_MP3_ERR_NONE;
        os_send_sig(conf_msg,L1C1_QUEUE);
        DEBUGMSG(status,NU_SEND_QUEUE_ERR)

              #if(L1_BT_AUDIO ==1)
		  if(bt_audio.connected_status == TRUE)
		  	{
	            bt_audio.pcm_data_end=1;
				bt_audio.pcm_data_ready = 0;
		  	}
		#endif
        // Change state
        *state=RESET;
      }
else
      {
	if(bt_audio.connected_status == FALSE)
	{
if((l1_apihisr.mp3.dma_it_occurred==1 )|| ((d_mp3_dma_current_buffer_id+1)%(MP3_BUFFER_COUNT/2)!=0))
 {
            l1mp3_apihisr_update_dma_buffer_index(l1_apihisr.mp3.mp3_dma_csr);
	     l1_apihisr.mp3.dma_it_occurred=0;
#if (OP_L1_STANDALONE == 0) 
           // Copy the decoded output from API buffer to DMA input buffer
            l1mp3_apihisr_api_buffer_copy(&mp3_dma->a_mp3_dma_input_buffer[d_mp3_dma_current_buffer_id][0],
                                          (API *)mp3_ndb->a_mp3_api_output_buffer0,
                                          C_MP3_OUTPUT_BUFFER_SIZE);
	l1_audio_manager((UWORD8 *)&mp3_dma->a_mp3_dma_input_buffer[d_mp3_dma_current_buffer_id][0],C_MP3_OUTPUT_BUFFER_SIZE);
	       // Generate API interrupt only if there are no critical DMA errors
	        if(l1mp3_apihisr_dma_error_handler()==FALSE)
	        {
	// Generate API interrupt
	       l1_trigger_api_interrupt();
	        }
	        else
	        {
	          // Request stop to the DSP
	          mp3_ndb->d_mp3_api_stop=(API)1;

	          // Change state
	          *state=WAIT_STOP_ACK;

	          // Generate API interrupt
	          l1_trigger_api_interrupt();
	        }
	}
	}
else
{
	if(l1_apihisr.mp3.dma_it_occurred==0 )
	{

            l1mp3_apihisr_update_dma_buffer_index(l1_apihisr.mp3.mp3_dma_csr);
		     l1_apihisr.mp3.dma_it_occurred=0;
#else
	    // Copy the decoded output from API buffer to DMA input buffer
	            l1mp3_apihisr_api_buffer_copy(&mp3_dma->a_mp3_dma_input_buffer[d_mp3_dma_current_buffer_id][0],
	                                          (API *)mp3_ndb->a_mp3_api_output_buffer0,
	                                          C_MP3_OUTPUT_BUFFER_SIZE);
#endif


       #if(L1_BT_AUDIO ==1)||(L1_WCM ==1)
	l1_audio_manager((UWORD8 *)&mp3_dma->a_mp3_dma_input_buffer[d_mp3_dma_current_buffer_id][0],C_MP3_OUTPUT_BUFFER_SIZE);
       #endif
       // Generate API interrupt only if there are no critical DMA errors
        if(l1mp3_apihisr_dma_error_handler()==FALSE)
        {
// Generate API interrupt
       l1_trigger_api_interrupt();
        }
        else
        {
          // Request stop to the DSP
          mp3_ndb->d_mp3_api_stop=(API)1;

          // Change state
          *state=WAIT_STOP_ACK;

          // Generate API interrupt
          l1_trigger_api_interrupt();
        }
	  }
   }

}
      return;
    }   // end case WAIT_END_ACK
//omaps00090550    break;

    case WAIT_LOOPBACK_ACK:
    {
      // *----------------------------------*
      // * End acknowledgement from the DSP *
      // *----------------------------------*
      if(mp3_ndb->d_mp3_api_end==0)
      {
        UWORD16 size;

        // Reset the ping-pong DMA input buffers
	        l1mp3_apihisr_reset_buffer(&mp3_dma->a_mp3_dma_input_buffer[0][0], MP3_BUFFER_COUNT*C_MP3_OUTPUT_BUFFER_SIZE);
              d_mp3_dma_current_buffer_id=MP3_BUFFER_COUNT-1;
    
        // Set DSP init command & generate API interrupt
        mp3_ndb->d_mp3_api_init=(API)1;
        l1_trigger_api_interrupt();

        // Fill initial MP3 buffers
        Cust_get_pointer_status=Cust_get_pointer_next_buffer(&mp3_buffer_ptr,&size,l1a_l1s_com.mp3_task.parameters.session_id);
        Cust_get_pointer_notify(l1a_l1s_com.mp3_task.parameters.session_id);
        mp3_buffer_end=mp3_buffer_ptr+size/2;   // size is in bytes, but we use 16-bit pointer

        // Change state
        *state=WAIT_DSP_INIT;
      }
else
      {
if((l1_apihisr.mp3.dma_it_occurred==1 )|| ((d_mp3_dma_current_buffer_id+1)%(MP3_BUFFER_COUNT/2)!=0))
 {
		// Update the DMA input buffer index
        l1mp3_apihisr_update_dma_buffer_index(l1_apihisr.mp3.mp3_dma_csr);
        l1_apihisr.mp3.dma_it_occurred=0;
#if (OP_L1_STANDALONE == 0)
       // Copy the decoded output from API buffer to DMA input buffer
        l1mp3_apihisr_api_buffer_copy(&mp3_dma->a_mp3_dma_input_buffer[d_mp3_dma_current_buffer_id][0],
                                          (API *)mp3_ndb->a_mp3_api_output_buffer0,
                                         C_MP3_OUTPUT_BUFFER_SIZE);
     
#else
 // Copy the decoded output from API buffer to DMA input buffer
	        l1mp3_apihisr_api_buffer_copy(&mp3_dma->a_mp3_dma_input_buffer[d_mp3_dma_current_buffer_id][0],
	                                          (API *)mp3_ndb->a_mp3_api_output_buffer0,
	                                         C_MP3_OUTPUT_BUFFER_SIZE);

#endif
	     

       #if(L1_BT_AUDIO ==1)||(L1_WCM ==1)
	l1_audio_manager((UWORD8 *)&mp3_dma->a_mp3_dma_input_buffer[d_mp3_dma_current_buffer_id][0],C_MP3_OUTPUT_BUFFER_SIZE);
       #endif     
  // Generate API interrupt only if there are no critical DMA errors
        if(l1mp3_apihisr_dma_error_handler()==FALSE)
        {
          // Generate dummy API interrupt           
    l1_trigger_api_interrupt();
 }
        else
        {
          // Request stop to the DSP
          mp3_ndb->d_mp3_api_stop=(API)1;

          // Change state
          *state=WAIT_STOP_ACK;

          // Generate API interrupt
          l1_trigger_api_interrupt();
        }
}
}
      return;
    }   // end case WAIT_LOOPBACK_ACK
//omaps00090550    break;
  }   // switch(*state)
}
/*---------------------------------------------------------------*/
/* l1mp3_apihisr_api_buffer_copy()                               */
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
void l1mp3_apihisr_api_buffer_copy(UWORD16 *dst, const API *src, UWORD16 size)
{
  while(size>0)
  {
    *dst++=((API)*src++);
    size--;
  }

}

/*---------------------------------------------------------------*/
/* l1mp3_apihisr_reset_buffer()                                  */
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
void l1mp3_apihisr_reset_buffer(UWORD16 *dst, UWORD16 size)
{
  while(size>0)
  {
    *dst++=0;
    size--;
  }
}
void l1mp3_apihisr_reset_outbuffer(API *dst, UWORD16 size)
{
  while(size>0)
  {
    *dst++=0;
    size--;
  }
}
void l1mp3_apihisr_update_dma_buffer_index(UWORD16 dma_csr)
{
  UWORD16 d_buffer_id,error_code;
  UWORD8 mp3_buf_ind;

#if(L1_BT_AUDIO ==1)
if(bt_audio.connected_status == FALSE)
{
#endif	
if( (l1_apihisr.mp3.dma_it_occurred==1 )&&(((dma_csr & C_MP3_DMA_CSR_HALF_BLOCK)&& d_mp3_dma_current_buffer_id!=(MP3_BUFFER_COUNT-1) )||
     ((dma_csr & C_MP3_DMA_CSR_BLOCK)&& d_mp3_dma_current_buffer_id !=(MP3_BUFFER_COUNT/2)-1) ))  
         {
     d_buffer_id = d_mp3_dma_current_buffer_id + 1;
     if(d_buffer_id > MP3_BUFFER_COUNT-1)
      d_buffer_id=0;

    if(d_buffer_id >= MP3_BUFFER_COUNT/2)
     mp3_buf_ind=(MP3_BUFFER_COUNT)-d_buffer_id;
    else
     mp3_buf_ind=((MP3_BUFFER_COUNT/2))-d_buffer_id;
		
     	
	 
	     l1mp3_apihisr_reset_buffer(&mp3_dma->a_mp3_dma_input_buffer[d_buffer_id][0],mp3_buf_ind* C_MP3_OUTPUT_BUFFER_SIZE);
    
     // As this happens to be a DMA error, we need to reset the API interrupt flag
  // mp3_ndb->d_mp3_out_dma_it_occured =(API)0;
 
    error_code = C_MP3_DMA_IT_MASKED;

     #if (TRACE_TYPE == 1) || (TRACE_TYPE == 4)
		if((trace_info.current_config->l1_dyn_trace) & (1<<L1_DYN_TRACE_MP3))
		{
		   char str[25];
		   sprintf(str,"MP3 warning: 0x%04X\n\r",error_code);
		   rvt_send_trace_cpy((T_RVT_BUFFER)str,trace_info.l1_trace_user_id,strlen(str),RVT_ASCII_FORMAT);
		}
     #endif    // (TRACE_TYPE == 1) || (TRACE_TYPE == 4)
    }
  else
  {

      d_mp3_dma_current_buffer_id += 1;
      if(d_mp3_dma_current_buffer_id>(MP3_BUFFER_COUNT-1))
          d_mp3_dma_current_buffer_id=0;    

  }
#if(L1_BT_AUDIO ==1)
 }
else
{
	d_mp3_dma_current_buffer_id += 1;
	if(d_mp3_dma_current_buffer_id>(MP3_BUFFER_COUNT-1))
	          d_mp3_dma_current_buffer_id=0;    
}
 #endif

} // End of l1mp3_apihisr_update_dma_buffer_index()


/*-------------------------------------------------------------*/
/* l1mp3_apihisr_error_handler()                               */
/*-------------------------------------------------------------*/
/*                                                             */
/* Parameters : error_code (OUT) error_code received from DSP  */
/*                                                             */
/* Return     : TRUE if errors are critical, FALSE otherwise   */
/*                                                             */
/* Description : fill DSP buf                                  */
/*                                                             */
/*-------------------------------------------------------------*/
BOOL l1mp3_apihisr_error_handler(UWORD16 *error_code)
{
  BOOL critical=FALSE;
UWORD16 dma_csr=0;

#if(L1_BT_AUDIO ==1)
if(bt_audio.connected_status == FALSE)
{
#endif	
  // Read the DMA_CSR value saved by the DMA ISR
  //here we dont check cached value of dma_it_occurred as it wont reflect the error case of DMA interrupt
if(dma_it_occurred )
{
  dma_csr=mp3_dma_csr;
  // Check for DMA errors, if any
  if(dma_csr & C_MP3_DMA_CSR_ERROR_CHECK_MASK)
  {
    if((dma_csr & C_MP3_DMA_CSR_ERROR_CHECK_MASK)==C_MP3_DMA_CSR_DROP)
    {
      *error_code=C_MP3_DMA_DROP;
    }
    else if((dma_csr & C_MP3_DMA_CSR_ERROR_CHECK_MASK)==C_MP3_DMA_CSR_TOUT_SRC)
    {
      *error_code=C_MP3_DMA_TOUT_SRC;
    }
    else
    {
      *error_code=C_MP3_DMA_TOUT_DST;
    }
           #if(L1_BT_AUDIO ==1)
           if(bt_audio.connected_status == TRUE)
              bt_audio.pcm_data_failed =1;
           #endif
    critical=TRUE;
    return critical;
  }
	}
#if(L1_BT_AUDIO ==1)
}
#endif	
  // Returns DSP error_code
  *error_code=mp3_ndb->d_mp3_api_error_code;

  // Acknowledge possible error codes
  if(mp3_ndb->d_mp3_api_error_code&C_MP3_ERR_NONE)
  {
    mp3_ndb->d_mp3_api_error_code&=~C_MP3_ERR_NONE;
  }
  if(mp3_ndb->d_mp3_api_error_code&C_MP3_SYNC_NOT_FOUND)
  {
    mp3_ndb->d_mp3_api_error_code&=~C_MP3_SYNC_NOT_FOUND;
  }
  if(mp3_ndb->d_mp3_api_error_code&C_MP3_NOT_LAYER3)
  {
    mp3_ndb->d_mp3_api_error_code&=~C_MP3_NOT_LAYER3;
    critical=TRUE;
  }
  if(mp3_ndb->d_mp3_api_error_code&C_MP3_FREE_FORMAT)
  {
    mp3_ndb->d_mp3_api_error_code&=~C_MP3_FREE_FORMAT;
    critical=TRUE;
  }
  if(mp3_ndb->d_mp3_api_error_code&C_MP3_ALG_ERROR)
  {
    mp3_ndb->d_mp3_api_error_code&=~C_MP3_ALG_ERROR;
    critical=TRUE;
  }
  if(mp3_ndb->d_mp3_api_error_code&C_MP3_DECODING_DELAY)
  {
    mp3_ndb->d_mp3_api_error_code&=~C_MP3_DECODING_DELAY;
  }

  if(mp3_ndb->d_mp3_api_error_code&C_MP3_DMA_DROP)
  {
    mp3_ndb->d_mp3_api_error_code&=~C_MP3_DMA_DROP;
    critical=TRUE;
  }
  if(mp3_ndb->d_mp3_api_error_code&C_MP3_DMA_TOUT_SRC)
  {
    mp3_ndb->d_mp3_api_error_code&=~C_MP3_DMA_TOUT_SRC;
    critical=TRUE;
  }
  if(mp3_ndb->d_mp3_api_error_code&C_MP3_DMA_TOUT_DST)
  {
    mp3_ndb->d_mp3_api_error_code&=~C_MP3_DMA_TOUT_DST;
    critical=TRUE;
  }
  if(mp3_ndb->d_mp3_api_error_code&C_MP3_DMA_IT_MASKED)
  {
    mp3_ndb->d_mp3_api_error_code&=~C_MP3_DMA_IT_MASKED;
  }
  if(mp3_ndb->d_mp3_api_error_code&C_MP3_CHECK_BUFFER_KO)
  {
    mp3_ndb->d_mp3_api_error_code&=~C_MP3_CHECK_BUFFER_KO;
  }
  if(mp3_ndb->d_mp3_api_error_code&C_MP3_CHECK_BUFFER_DELAY)
  {
    mp3_ndb->d_mp3_api_error_code&=~C_MP3_CHECK_BUFFER_DELAY;
  }
if((*error_code==0))
{
*error_code=C_MP3_ERR_NONE;
}
      #if(L1_BT_AUDIO ==1)
         //if(critical ==TRUE && bt_audio.connected_status == TRUE)
            //bt_audio.pcm_data_failed =1;
      #endif
  return critical;
}


/*---------------------------------------------------------------*/
/* l1mp3_apihisr_fill_dsp_buffer()                               */
/*---------------------------------------------------------------*/
/*                                                               */
/* Parameters : mp3_buffer_ptr (IN/OUT): pointer to current pos  */
/*                                       in current MP3 buffer   */
/*              mp3_buffer_end (IN/OUT): pointer to end of       */
/*                                       current MP3 buffer      */
/*              Cust_get_pointer_status (IN/OUT): status of data */
/*                        manager (DATA_AVAILABLE or DATA_LAST)  */
/*                                                               */
/*              init:  is decoder in init phase?                 */
/*                                                               */
/* Return     : MP3_FILL_OK                                      */
/*              MP3_FILL_END                                     */
/*              MP3_FILL_LOOPBACK                                */
/*                                                               */
/* Description :                                                 */
/*                                                               */
/*---------------------------------------------------------------*/
UWORD8 l1mp3_apihisr_fill_dsp_buffer(UWORD16 **mp3_buffer_ptr, UWORD16 **mp3_buffer_end, UWORD8 *Cust_get_pointer_status, BOOL init)
{
  UWORD16 buffer_size;
  UWORD8  status=MP3_FILL_OK;
  BOOL    loopback_reinit;
  UWORD16 size2;
  UWORD16 size1;
  UWORD8 buf_status;

  if(*mp3_buffer_ptr+mp3_ndb->d_mp3_api_request_size > *mp3_buffer_end)
  {
     size1=2*(*mp3_buffer_end-*mp3_buffer_ptr);

    // Flush current buffer
    if((*Cust_get_pointer_status==DATA_AVAILABLE) || (*Cust_get_pointer_status==WAIT_FOR_DATA))
    {



      if((*Cust_get_pointer_status==DATA_AVAILABLE))
      {
	    l1mp3_apihisr_memcpy((UWORD16 *)mp3_ndb->a_mp3_api_input_buffer+mp3_ndb->d_mp3_api_request_index,
	  	                         (UWORD16 *)*mp3_buffer_ptr,
	  	                         size1);
      }

      // Get pointer to next buffer
      //*Cust_get_pointer_status=Cust_get_pointer_next_buffer(mp3_buffer_ptr,&buffer_size,l1a_l1s_com.mp3_task.parameters.session_id);

      buf_status = Cust_get_next_buffer_status();

      if(buf_status == TRUE)
      {
        l1_apihisr.mp3.wait_data = TRUE;
        *Cust_get_pointer_status =WAIT_FOR_DATA;//omaps00090550
		  return status;
      }

      *Cust_get_pointer_status=Cust_get_pointer_next_buffer(mp3_buffer_ptr,&buffer_size,l1a_l1s_com.mp3_task.parameters.session_id);
      size2=mp3_ndb->d_mp3_api_request_size*2-size1;
      *mp3_buffer_end=*mp3_buffer_ptr+buffer_size/2;   // size is in bytes, but we use 16-bit pointer

      loopback_reinit=FALSE;

      if(size2>buffer_size)
      {
        // Last buffer to be played
        size2=buffer_size;
        if(init==TRUE)
          mp3_ndb->d_mp3_api_stop=(API)1;
        else
          mp3_ndb->d_mp3_api_end=(API)1;
        if(l1a_l1s_com.mp3_task.parameters.loopback==FALSE)
          status=MP3_FILL_END;
        else
        {
          status=MP3_FILL_LOOPBACK;
          loopback_reinit=TRUE;
        }
      }

      // Copy remaining requested data to DSP
      l1mp3_apihisr_memcpy((UWORD16 *)mp3_ndb->a_mp3_api_input_buffer+mp3_ndb->d_mp3_api_request_index+size1/2,
                           (UWORD16 *)*mp3_buffer_ptr,
                           size2);

      *mp3_buffer_ptr+=size2/2;   // size is in bytes, but we use 16-bit pointer

      mp3_ndb->d_mp3_api_provided_size=(size1+size2)/2;   // size is in bytes but DSP expect size in words

      
// Check whether DMA interrupt has occurred during DSP buffer fill and
      // if it has, check for DMA errors
      if(l1mp3_apihisr_dma_error_handler()==FALSE)
      {
      // Set play and generate an MCU->DSP interrupt
      mp3_ndb->d_mp3_api_play=(API)1;
      l1_trigger_api_interrupt();
}
      else
      {
        // Set status to indicate DMA error
        status=MP3_FILL_DMA_ERROR;

        // Request stop to the DSP
        mp3_ndb->d_mp3_api_stop=(API)1;

        // Generate API interrupt
        l1_trigger_api_interrupt();
      }

      // Send notification to upper layers
      if((loopback_reinit==TRUE) || (*Cust_get_pointer_status==DATA_AVAILABLE))
        Cust_get_pointer_notify(l1a_l1s_com.mp3_task.parameters.session_id);
    }
    else
    {

	l1mp3_apihisr_memcpy((UWORD16 *)mp3_ndb->a_mp3_api_input_buffer+mp3_ndb->d_mp3_api_request_index,(UWORD16 *)*mp3_buffer_ptr,size1);

      // Last buffer to be played
      mp3_ndb->d_mp3_api_provided_size=(size1)/2;   // size is in bytes, but DSP expects size in words
      if(init==TRUE)
        mp3_ndb->d_mp3_api_stop=(API)1;
      else
        mp3_ndb->d_mp3_api_end=(API)1;

      // Check whether DMA interrupt has occurred during DSP buffer fill and
      // if it has, check for DMA errors
      if(l1mp3_apihisr_dma_error_handler()==FALSE)
      {
      // Set play and generate an MCU->DSP interrupt
      mp3_ndb->d_mp3_api_play=(API)1;
      l1_trigger_api_interrupt();

      if(l1a_l1s_com.mp3_task.parameters.loopback==TRUE)
      {
        status=MP3_FILL_LOOPBACK;
        Cust_get_pointer_notify(l1a_l1s_com.mp3_task.parameters.session_id);
      }
      else
        status=MP3_FILL_END;
    }
      else
      {
        // Set status to indicate DMA error
        status=MP3_FILL_DMA_ERROR;

        // Request stop to the DSP
        mp3_ndb->d_mp3_api_stop=(API)1;

        // Generate API interrupt
        l1_trigger_api_interrupt();

        // Send notification to upper layers
        if(l1a_l1s_com.mp3_task.parameters.loopback==TRUE)
        {
          Cust_get_pointer_notify(l1a_l1s_com.mp3_task.parameters.session_id);
        }
      }
   }
  }
  else
  {
    // Current MP3 buffer is enough to fill DSP request
    l1mp3_apihisr_memcpy((UWORD16 *)mp3_ndb->a_mp3_api_input_buffer+mp3_ndb->d_mp3_api_request_index,
                         (UWORD16 *)*mp3_buffer_ptr,
                         2*mp3_ndb->d_mp3_api_request_size);

    *mp3_buffer_ptr+=mp3_ndb->d_mp3_api_request_size;

    mp3_ndb->d_mp3_api_provided_size=mp3_ndb->d_mp3_api_request_size;

     // Check whether DMA interrupt has occurred during DSP buffer fill and
    // if it has, check for DMA errors
    if(l1mp3_apihisr_dma_error_handler()==FALSE)
    {
    // Set play and generate an MCU->DSP interrupt
    mp3_ndb->d_mp3_api_play=(API)1;
    l1_trigger_api_interrupt();
  }
    else
    {
      // Set status to indicate DMA error
      status=MP3_FILL_DMA_ERROR;

      // Request stop to the DSP
      mp3_ndb->d_mp3_api_stop=(API)1;

      // Generate API interrupt
      l1_trigger_api_interrupt();
    }
  
//wait_ARM_cycles(30000);
//sim_intrupt(d_mp3_dma_current_buffer_id);
  }

  return status;
}


/*---------------------------------------------------------------*/
/* l1mp3_apihisr_dma_error_handler()                             */
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

BOOL l1mp3_apihisr_dma_error_handler()
{
  BOOL critical=FALSE;
  UWORD32 error_code=C_MP3_ERR_NONE;
  UWORD16 dma_csr=0;
#if(L1_BT_AUDIO ==1)
if(bt_audio.connected_status == FALSE)
{
#endif	  
  // Check whether DMA interrupt has occured
  //here we dont check cached value of dma_it_occurred as it wont reflect the error case of DMA interrupt
  if(dma_it_occurred)
  {
    // Reset d_mp3_api_dma_it_occured variable
  //  mp3_ndb->d_mp3_api_dma_it_occured=(API)0;

    // Read the DMA_CSR value saved by the DMA ISR
    dma_csr=mp3_dma_csr;

    // Check for DMA errors
    if(dma_csr&C_MP3_DMA_CSR_ERROR_CHECK_MASK)
    {
      if((dma_csr&C_MP3_DMA_CSR_ERROR_CHECK_MASK)==C_MP3_DMA_CSR_DROP)
      {
        error_code=C_MP3_DMA_DROP;
      }
      else if((dma_csr&C_MP3_DMA_CSR_ERROR_CHECK_MASK)==C_MP3_DMA_CSR_TOUT_SRC)
      {
        error_code=C_MP3_DMA_TOUT_SRC;
      }
      else
      {
        error_code=C_MP3_DMA_TOUT_DST;
      }

      // Critical error occured
      l1_apihisr.mp3.error_code=(UWORD32)error_code;

      // Reset the ping-pong DMA input buffers
	      l1mp3_apihisr_reset_buffer(&mp3_dma->a_mp3_dma_input_buffer[0][0],  MP3_BUFFER_COUNT *C_MP3_OUTPUT_BUFFER_SIZE);

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
     // l1mp3_apihisr_update_dma_buffer_index(dma_csr);

      // Reset the DMA input buffer
      //l1mp3_apihisr_reset_buffer(&a_mp3_dma_input_buffer[d_mp3_dma_current_buffer_id][0], C_MP3_OUTPUT_BUFFER_SIZE);

    } // DMA critical error check

  } // DMA interrupt occured check
#if(L1_BT_AUDIO==1)
}
#endif
  return critical;

} // End of l1mp3_apihisr_dma_error_check()


/*---------------------------------------------------------------*/
/* l1mp3_apihisr_memcpy()                                        */
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
void l1mp3_apihisr_memcpy(UWORD16 *dst, const UWORD16 *src, UWORD16 size)
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
   *dst=(*src)>>8;
}

#endif  // L1_MP3

#else

#include <stdio.h>
#include <string.h>
#include "l1_confg.h"



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



#if (OP_RIV_AUDIO == 1)
  #include "rv/rv_general.h"
#endif
#if (L1_MP3 == 1)
  #include "l1mp3_const.h"
  #include "l1mp3_signa.h"
  #include "l1mp3_defty.h"
  #include "l1mp3_msgty.h"
  #include "l1mp3_error.h"
  #include "l1mp3_proto.h"
#endif
#if (L1_MIDI == 1)
  #include "l1midi_defty.h"
#endif
#include "l1_defty.h"
#include "cust_os.h"
#include "l1_varex.h"
#include "l1_macro.h"
#include "l1_api_hisr.h"
#include "l1_trace.h"

#if (L1_MP3 == 1)

// Return value of l1mp3_apihisr_fill_dsp_buffer() function
#define MP3_FILL_OK       0
#define MP3_FILL_END      1
#define MP3_FILL_LOOPBACK 2

#if 0
#ifdef BTS
#ifdef BTS_AV
#define MP3_API_OUTPUT_BUFFER_SIZE  1152

// Value to buffer -1, 0 or 1
#define MP3_OUT_BUFFER_NONE 	  2
#define MP3_OUT_BUFFER_0   		  0
#define MP3_OUT_BUFFER_1   		  1
char  av_buffer_to_read = MP3_OUT_BUFFER_NONE;
unsigned char local_mp3_api_output_buffer0[MP3_API_OUTPUT_BUFFER_SIZE];
unsigned char local_mp3_api_output_buffer1[MP3_API_OUTPUT_BUFFER_SIZE];
extern void PCMSIM_NotifyTxComplete(void);
extern void AVDRV_SendData(char * pcmData, long len);
extern void AVDRV_RequestMoreData(void);

UWORD8  l1_audio_used_buffer = MP3_OUT_BUFFER_0;
#endif
#endif
#endif
// MP3 NDB API
extern T_MP3_MCU_DSP *mp3_ndb;

// External prototypes
extern UWORD8 Cust_get_pointer_next_buffer(UWORD16 **ptr,UWORD16 *buffer_size,UWORD8 session_id);
extern void Cust_get_pointer_notify(UWORD8 session_id);
extern UWORD8 Cust_get_next_buffer_status(void);
extern UWORD32 mp3_forwardrewind_reset;//MP3-FR- to reset DSP for forward/rewind
/*-------------------------------------------------------------*/
/* l1mp3_apihisr()                                             */
/*-------------------------------------------------------------*/
/*                                                             */
/* Parameters : none                                           */
/*                                                             */
/* Return     : n/a                                            */
/*                                                             */
/* Description : implements MP3 HISR                           */
/*                                                             */
/*-------------------------------------------------------------*/
void l1mp3_apihisr()
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

  UWORD8 *state=&l1_apihisr.mp3.state;
  static UWORD8 Cust_get_pointer_status;
  static UWORD16 *mp3_buffer_ptr,*mp3_buffer_end;
  xSignalHeaderRec *conf_msg;
  UWORD16 error_code;


  // Update MP3 frame header for L1A
  l1a_apihisr_com.mp3.header=(mp3_ndb->d_mp3_api_header[0]<<16)+mp3_ndb->d_mp3_api_header[1];

  // Check error_code from DSP
  if(*state>RESET)
  {
    if(l1mp3_apihisr_error_handler(&error_code)==TRUE)
    {
      // Critical error occured
      l1_apihisr.mp3.error_code=(UWORD32)error_code;

      // Request stop to the DSP
      mp3_ndb->d_mp3_api_stop=(API)1;

      // Change state
      *state=WAIT_STOP_ACK;

      // Generate API interrupt
      l1_trigger_api_interrupt();

      return;
    }
    else
    {
      if(error_code!=C_MP3_ERR_NONE)
      {
        // Trace non-critical errors
#if (TRACE_TYPE == 1) || (TRACE_TYPE == 4)
        if((trace_info.current_config->l1_dyn_trace) & (1<<L1_DYN_TRACE_MP3))
        {
          char str[25];
          sprintf(str,"MP3 warning: 0x%04X\n\r",error_code);
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
#ifdef BTS
#ifdef BTS_AV
	/* Reset the audio buffer used to mp3 out buffer 0 */
	l1_audio_used_buffer 	= MP3_OUT_BUFFER_0;
	av_buffer_to_read         	= MP3_OUT_BUFFER_NONE;
#endif
#endif
	
      // *----------------------------*
      // * Start command from the L1A *
      // *----------------------------*
      if(l1a_apihisr_com.mp3.command.start==TRUE)
      {
        UWORD16 size;

        // Reset the command
        l1a_apihisr_com.mp3.command.start=FALSE;
        l1_apihisr.mp3.wait_data = FALSE;

        // Store the DMA channel in the MP3 NDB
        mp3_ndb->d_mp3_api_channel=l1a_apihisr_com.mp3.dma_channel_number;

        // Set DSP init command & generate API interrupt
        mp3_ndb->d_mp3_api_init=(API)1;
        l1_trigger_api_interrupt();

        // Fill initial MP3 buffers
        mp3_buffer_ptr=NULL;
        Cust_get_pointer_status=Cust_get_pointer_next_buffer(&mp3_buffer_ptr,&size,l1a_l1s_com.mp3_task.parameters.session_id);
        mp3_buffer_end=mp3_buffer_ptr+size/2;   // size is in bytes, but we use 16-bit pointer

#if (TRACE_TYPE == 1) || (TRACE_TYPE == 4)
        if(size<1600)
        {
          if((trace_info.current_config->l1_dyn_trace) & (1<<L1_DYN_TRACE_MP3))
          {
            char str[]="MP3 warning: input buffer may be too small\n\r";
            rvt_send_trace_cpy((T_RVT_BUFFER)str,trace_info.l1_trace_user_id,strlen(str),RVT_ASCII_FORMAT);
          }
        }
#endif

        // Change state
        *state=WAIT_DSP_INIT;
      }

      return;
    }   // end case RESET
//omaps00090550    break;

    /*****************/
    /* WAIT_DSP_INIT */
    /*****************/
    case WAIT_DSP_INIT:
    {
      // *---------------------------*
      // * Data request from the DSP *
      // *---------------------------*
      if((mp3_ndb->d_mp3_api_play==0) && (mp3_ndb->d_mp3_api_init==1))
      {
        UWORD8 status=l1mp3_apihisr_fill_dsp_buffer(&mp3_buffer_ptr,&mp3_buffer_end,&Cust_get_pointer_status,TRUE);

         error_code = C_MP3_CHECK_BUFFER_KO;

#if (TRACE_TYPE == 1) || (TRACE_TYPE == 4)
		if((trace_info.current_config->l1_dyn_trace) & (1<<L1_DYN_TRACE_MP3))
		{
		   char str[25];
		   sprintf(str,"MP3 warning: 0x%04X\n\r",error_code);
		   rvt_send_trace_cpy((T_RVT_BUFFER)str,trace_info.l1_trace_user_id,strlen(str),RVT_ASCII_FORMAT);
		}
#endif    // (TRACE_TYPE == 1) || (TRACE_TYPE == 4)

        switch(status)
        {
          case MP3_FILL_END:
            // No more data, wait for stop acknowledgement from DSP
            *state=WAIT_STOP_ACK;
          break;
          case MP3_FILL_LOOPBACK:
            *state=WAIT_LOOPBACK_ACK;
          break;
        }
        return;
      }

      /*------------------------------------*/
      /* Start acknowledgement from the DSP */
      /*------------------------------------*/
      if(mp3_ndb->d_mp3_api_init==0)
      {
        // Send notification to L1A
        conf_msg=os_alloc_sig(0);
        DEBUGMSG(status,NU_ALLOC_ERR)
        conf_msg->SignalCode=API_MP3_START_CON;
        os_send_sig(conf_msg,L1C1_QUEUE);
        DEBUGMSG(status,NU_SEND_QUEUE_ERR)

        // Change state
        *state=WAIT_DATA_REQUEST;
      }

      return;
    }   // end case WAIT_DSP_INIT
//omaps00090550    break;

    /*********************/
    /* WAIT_DATA_REQUEST */
    /*********************/
    case WAIT_DATA_REQUEST:
    {
#ifdef BTS
#ifdef BTS_AV		
	/* ORDINA : Copy the out buffer to local buffer */
	/* Is this the correct place to do this??? */
	if (l1_audio_used_buffer == MP3_OUT_BUFFER_0)
	{
		/* copy the buffer a_mp3_api_output_buffer0 to local buffer */
//        memcpy(local_mp3_api_output_buffer0, (void *)mp3_ndb->a_mp3_api_output_buffer0, 1152);

		AVDRV_SendData((char*)mp3_ndb->a_mp3_api_output_buffer0, 1152);

		l1_audio_used_buffer = MP3_OUT_BUFFER_1;
	}
	else if (l1_audio_used_buffer == MP3_OUT_BUFFER_1)
	{
		/* copy the buffer a_mp3_api_output_buffer1 to local buffer */
//        memcpy(local_mp3_api_output_buffer1, (void *)mp3_ndb->a_mp3_api_output_buffer1, 1152);

		AVDRV_SendData((char*)mp3_ndb->a_mp3_api_output_buffer1, 1152);

		l1_audio_used_buffer = MP3_OUT_BUFFER_0;	
	}
	/* notify PCMSIM that there is data waiting to be sent */
//	PCMSIM_NotifyTxComplete();
	//AVDRV_RequestMoreData();

//ORDINA toggle buffer
      //l1_audio_used_buffer = ~l1_audio_used_buffer;
#endif
#endif
	
      // *---------------------------*
      // * Stop request from the L1A *
      // *---------------------------*
      if(l1a_apihisr_com.mp3.command.stop==TRUE)
      {
        // Reset the command
        l1a_apihisr_com.mp3.command.stop=FALSE;

        // Request stop to the DSP
        mp3_ndb->d_mp3_api_stop=(API)1;

        // Change state
        *state=WAIT_STOP_ACK;

        // Generate API interrupt
        l1_trigger_api_interrupt();

        return;
      }

      // *----------------------------*
      // * Pause request from the L1A *
      // *----------------------------*
      if(l1a_apihisr_com.mp3.command.pause==TRUE)
      {
        // Reset the command
        l1a_apihisr_com.mp3.command.pause=FALSE;

        // Request stop to the DSP
        mp3_ndb->d_mp3_api_pause=(API)1;

        // Change state
        *state=WAIT_PAUSE_ACK;

        // Generate API interrupt
        l1_trigger_api_interrupt();

        return;
      }

      // *---------------------------*
      // * Data request from the DSP *
      // *---------------------------*
      if((mp3_ndb->d_mp3_api_play==0))
      {
        UWORD8 status=l1mp3_apihisr_fill_dsp_buffer(&mp3_buffer_ptr,&mp3_buffer_end,&Cust_get_pointer_status,FALSE);
	if(mp3_forwardrewind_reset==1)
	{
	/*Reset the DSP CODEC if FORWARD/REWIND REQUEST has come
	mp3_forwardrewind_reset =0;//global var to check the forward rewind params
	// Request restart to the DSP
        mp3_ndb->d_mp3_api_restart=(API)1;

        // Generate API interrupt
        l1_trigger_api_interrupt();
	*/
	}
        switch(status)
        {
          case MP3_FILL_END:
            // No more data, wait for end acknowledgement from DSP
            *state=WAIT_END_ACK;
          break;
          case MP3_FILL_LOOPBACK:
            *state=WAIT_LOOPBACK_ACK;
          break;
        }
      }

      return;
    }   // end case WAIT_DATA_REQUEST
//omaps00090550    break;

    /***************/
    /* WAIT_RESUME */
    /***************/
    case WAIT_RESUME:
    {
      // *---------------------------*
      // * Stop request from the L1A *
      // *---------------------------*
      if(l1a_apihisr_com.mp3.command.stop==TRUE)
      {
        // Reset the command
        l1a_apihisr_com.mp3.command.stop=FALSE;

        // Request stop to the DSP
        mp3_ndb->d_mp3_api_stop=(API)1;

        // Change state
        *state=WAIT_STOP_ACK;

        // Generate API interrupt
        l1_trigger_api_interrupt();

        return;
      }

      // *-----------------------------*
      // * Resume request from the L1A *
      // *-----------------------------*
      if(l1a_apihisr_com.mp3.command.resume==TRUE)
      {
        UWORD8 status;

        // Reset the command
        l1a_apihisr_com.mp3.command.resume=FALSE;

        status=l1mp3_apihisr_fill_dsp_buffer(&mp3_buffer_ptr,&mp3_buffer_end,&Cust_get_pointer_status,FALSE);

        // Change state
        switch(status)
        {
          case MP3_FILL_END:
            // No more data, wait for end acknowledgement from DSP
            *state=WAIT_END_ACK;
          break;
          case MP3_FILL_LOOPBACK:
            *state=WAIT_LOOPBACK_ACK;
          break;
          default:
            *state=WAIT_DATA_REQUEST;
          break;
        }

        // Send notification to L1A
        conf_msg=os_alloc_sig(0);
        DEBUGMSG(status,NU_ALLOC_ERR)
        conf_msg->SignalCode=API_MP3_RESUME_CON;
        os_send_sig(conf_msg,L1C1_QUEUE);
        DEBUGMSG(status,NU_SEND_QUEUE_ERR)

        return;
      }

      // *------------------------------*
      // * Restart request from the L1A *
      // *------------------------------*
      if(l1a_apihisr_com.mp3.command.restart==TRUE)
      {
        UWORD16 buffer_size;

        // Reset the command
        l1a_apihisr_com.mp3.command.restart=FALSE;

        // Change state
        *state=WAIT_DATA_REQUEST;

        // Request restart to the DSP
        mp3_ndb->d_mp3_api_restart=(API)1;

        // Reset MP3 file
        mp3_buffer_ptr=NULL;
        Cust_get_pointer_status=Cust_get_pointer_next_buffer(&mp3_buffer_ptr,&buffer_size,l1a_l1s_com.mp3_task.parameters.session_id);
        mp3_buffer_end=mp3_buffer_ptr+buffer_size/2;   // size is in bytes, but we use 16-bit pointer

        // Generate API interrupt
        l1_trigger_api_interrupt();

        // Send notification to L1A
        conf_msg=os_alloc_sig(0);
        DEBUGMSG(status,NU_ALLOC_ERR)
        conf_msg->SignalCode=API_MP3_RESTART_CON;
        os_send_sig(conf_msg,L1C1_QUEUE);
        DEBUGMSG(status,NU_SEND_QUEUE_ERR)

        return;
      }

      return;
    }   // end case WAIT_RESUME
//omaps00090550    break;

    // ******************
    // * WAIT_PAUSE_ACK *
    // ******************
    case WAIT_PAUSE_ACK:
    {
      // *------------------------------------*
      // * Pause acknowledgement from the DSP *
      // *------------------------------------*
      if(mp3_ndb->d_mp3_api_pause==0)
      {
        // Send notification to L1A
        conf_msg=os_alloc_sig(0);
        DEBUGMSG(status,NU_ALLOC_ERR)
        conf_msg->SignalCode=API_MP3_PAUSE_CON;
        os_send_sig(conf_msg,L1C1_QUEUE);
        DEBUGMSG(status,NU_SEND_QUEUE_ERR)

        // Change state
        *state=WAIT_RESUME;
      }

      return;
    }   // end case WAIT_PAUSE_ACK
//omaps00090550    break;

    // *****************
    // * WAIT_STOP_ACK *
    // *****************
    case WAIT_STOP_ACK:
    {
      // *-----------------------------------*
      // * Stop acknowledgement from the DSP *
      // *-----------------------------------*
      if(mp3_ndb->d_mp3_api_stop==0)
      {
        // Send notification to L1A
        conf_msg=os_alloc_sig(sizeof(T_L1_API_MP3_STOP_CON));
        DEBUGMSG(status,NU_ALLOC_ERR)
        conf_msg->SignalCode=API_MP3_STOP_CON;
        ((T_L1_API_MP3_STOP_CON *)(conf_msg->SigP))->error_code=l1_apihisr.mp3.error_code;
        os_send_sig(conf_msg,L1C1_QUEUE);
        DEBUGMSG(status,NU_SEND_QUEUE_ERR)

        // Change state
        *state=RESET;
      }
      return;
    }   // end case WAIT_STOP_ACK
//omaps00090550    break;

    // ****************
    // * WAIT_END_ACK *
    // ****************
    case WAIT_END_ACK:
    {
      // *----------------------------------*
      // * End acknowledgement from the DSP *
      // *----------------------------------*
      if(mp3_ndb->d_mp3_api_end==0)
      {
        // Send notification to L1A
        conf_msg=os_alloc_sig(0);
        DEBUGMSG(status,NU_ALLOC_ERR)
        conf_msg->SignalCode=API_MP3_STOP_CON;
        ((T_L1_API_MP3_STOP_CON *)(conf_msg->SigP))->error_code=C_MP3_ERR_NONE;
        os_send_sig(conf_msg,L1C1_QUEUE);
        DEBUGMSG(status,NU_SEND_QUEUE_ERR)

        // Change state
        *state=RESET;
      }
      return;
    }   // end case WAIT_END_ACK
//omaps00090550    break;

    case WAIT_LOOPBACK_ACK:
    {
      // *----------------------------------*
      // * End acknowledgement from the DSP *
      // *----------------------------------*
      if(mp3_ndb->d_mp3_api_end==0)
      {
        UWORD16 size;

        // Set DSP init command & generate API interrupt
        mp3_ndb->d_mp3_api_init=(API)1;
        l1_trigger_api_interrupt();

        // Fill initial MP3 buffers
        Cust_get_pointer_status=Cust_get_pointer_next_buffer(&mp3_buffer_ptr,&size,l1a_l1s_com.mp3_task.parameters.session_id);
        Cust_get_pointer_notify(l1a_l1s_com.mp3_task.parameters.session_id);
        mp3_buffer_end=mp3_buffer_ptr+size/2;   // size is in bytes, but we use 16-bit pointer

        // Change state
        *state=WAIT_DSP_INIT;
      }
      return;
    }   // end case WAIT_LOOPBACK_ACK
//omaps00090550    break;
  }   // switch(*state)
}


/*-------------------------------------------------------------*/
/* l1mp3_apihisr_error_handler()                               */
/*-------------------------------------------------------------*/
/*                                                             */
/* Parameters : error_code (OUT) error_code received from DSP  */
/*                                                             */
/* Return     : TRUE if errors are critical, FALSE otherwise   */
/*                                                             */
/* Description : fill DSP buf                                  */
/*                                                             */
/*-------------------------------------------------------------*/
BOOL l1mp3_apihisr_error_handler(UWORD16 *error_code)
{
  BOOL critical=FALSE;

  // Returns DSP error_code
  *error_code=mp3_ndb->d_mp3_api_error_code;

  // Acknowledge possible error codes
  if(mp3_ndb->d_mp3_api_error_code&C_MP3_ERR_NONE)
  {
    mp3_ndb->d_mp3_api_error_code&=~C_MP3_ERR_NONE;
  }
  if(mp3_ndb->d_mp3_api_error_code&C_MP3_SYNC_NOT_FOUND)
  {
    mp3_ndb->d_mp3_api_error_code&=~C_MP3_SYNC_NOT_FOUND;
  }
  if(mp3_ndb->d_mp3_api_error_code&C_MP3_NOT_LAYER3)
  {
    mp3_ndb->d_mp3_api_error_code&=~C_MP3_NOT_LAYER3;
    critical=TRUE;
  }
  if(mp3_ndb->d_mp3_api_error_code&C_MP3_FREE_FORMAT)
  {
    mp3_ndb->d_mp3_api_error_code&=~C_MP3_FREE_FORMAT;
    critical=TRUE;
  }
  if(mp3_ndb->d_mp3_api_error_code&C_MP3_ALG_ERROR)
  {
    mp3_ndb->d_mp3_api_error_code&=~C_MP3_ALG_ERROR;
    critical=TRUE;
  }
  if(mp3_ndb->d_mp3_api_error_code&C_MP3_DECODING_DELAY)
  {
    mp3_ndb->d_mp3_api_error_code&=~C_MP3_DECODING_DELAY;
  }

  if(mp3_ndb->d_mp3_api_error_code&C_MP3_DMA_DROP)
  {
    mp3_ndb->d_mp3_api_error_code&=~C_MP3_DMA_DROP;
    critical=TRUE;
  }
  if(mp3_ndb->d_mp3_api_error_code&C_MP3_DMA_TOUT_SRC)
  {
    mp3_ndb->d_mp3_api_error_code&=~C_MP3_DMA_TOUT_SRC;
    critical=TRUE;
  }
  if(mp3_ndb->d_mp3_api_error_code&C_MP3_DMA_TOUT_DST)
  {
    mp3_ndb->d_mp3_api_error_code&=~C_MP3_DMA_TOUT_DST;
    critical=TRUE;
  }
  if(mp3_ndb->d_mp3_api_error_code&C_MP3_DMA_IT_MASKED)
  {
    mp3_ndb->d_mp3_api_error_code&=~C_MP3_DMA_IT_MASKED;
  }
  if(mp3_ndb->d_mp3_api_error_code&C_MP3_CHECK_BUFFER_KO)
  {
    mp3_ndb->d_mp3_api_error_code&=~C_MP3_CHECK_BUFFER_KO;
  }
  if(mp3_ndb->d_mp3_api_error_code&C_MP3_CHECK_BUFFER_DELAY)
  {
    mp3_ndb->d_mp3_api_error_code&=~C_MP3_CHECK_BUFFER_DELAY;
  }

  return critical;
}


/*---------------------------------------------------------------*/
/* l1mp3_apihisr_fill_dsp_buffer()                               */
/*---------------------------------------------------------------*/
/*                                                               */
/* Parameters : mp3_buffer_ptr (IN/OUT): pointer to current pos  */
/*                                       in current MP3 buffer   */
/*              mp3_buffer_end (IN/OUT): pointer to end of       */
/*                                       current MP3 buffer      */
/*              Cust_get_pointer_status (IN/OUT): status of data */
/*                        manager (DATA_AVAILABLE or DATA_LAST)  */
/*                                                               */
/*              init:  is decoder in init phase?                 */
/*                                                               */
/* Return     : MP3_FILL_OK                                      */
/*              MP3_FILL_END                                     */
/*              MP3_FILL_LOOPBACK                                */
/*                                                               */
/* Description :                                                 */
/*                                                               */
/*---------------------------------------------------------------*/
UWORD8 l1mp3_apihisr_fill_dsp_buffer(UWORD16 **mp3_buffer_ptr, UWORD16 **mp3_buffer_end, UWORD8 *Cust_get_pointer_status, BOOL init)
{
  UWORD16 buffer_size;
  UWORD8  status=MP3_FILL_OK;
  BOOL    loopback_reinit;
  UWORD16 size2;
  UWORD16 size1;
  UWORD8 buf_status;

  if(*mp3_buffer_ptr+mp3_ndb->d_mp3_api_request_size > *mp3_buffer_end)
  {
     size1=2*(*mp3_buffer_end-*mp3_buffer_ptr);

    // Flush current buffer
    if((*Cust_get_pointer_status==DATA_AVAILABLE) || (*Cust_get_pointer_status==WAIT_FOR_DATA))
    {



      if((*Cust_get_pointer_status==DATA_AVAILABLE))
      {
	    l1mp3_apihisr_memcpy((UWORD16 *)mp3_ndb->a_mp3_api_input_buffer+mp3_ndb->d_mp3_api_request_index,
	  	                         (UWORD16 *)*mp3_buffer_ptr,
	  	                         size1);
      }

      // Get pointer to next buffer
      //*Cust_get_pointer_status=Cust_get_pointer_next_buffer(mp3_buffer_ptr,&buffer_size,l1a_l1s_com.mp3_task.parameters.session_id);

      buf_status = Cust_get_next_buffer_status();

      if(buf_status == TRUE)
      {
        l1_apihisr.mp3.wait_data = TRUE;
        *Cust_get_pointer_status =WAIT_FOR_DATA;//omaps00090550
		  return status;
      }

      *Cust_get_pointer_status=Cust_get_pointer_next_buffer(mp3_buffer_ptr,&buffer_size,l1a_l1s_com.mp3_task.parameters.session_id);

      size2=mp3_ndb->d_mp3_api_request_size*2-size1;
      *mp3_buffer_end=*mp3_buffer_ptr+buffer_size/2;   // size is in bytes, but we use 16-bit pointer

      loopback_reinit=FALSE;

      if(size2>buffer_size)
      {
        // Last buffer to be played
        size2=buffer_size;
        if(init==TRUE)
          mp3_ndb->d_mp3_api_stop=(API)1;
        else
          mp3_ndb->d_mp3_api_end=(API)1;
        if(l1a_l1s_com.mp3_task.parameters.loopback==FALSE)
          status=MP3_FILL_END;
        else
        {
          status=MP3_FILL_LOOPBACK;
          loopback_reinit=TRUE;
        }
      }

      // Copy remaining requested data to DSP
      l1mp3_apihisr_memcpy((UWORD16 *)mp3_ndb->a_mp3_api_input_buffer+mp3_ndb->d_mp3_api_request_index+size1/2,
                           (UWORD16 *)*mp3_buffer_ptr,
                           size2);

      *mp3_buffer_ptr+=size2/2;   // size is in bytes, but we use 16-bit pointer

      mp3_ndb->d_mp3_api_provided_size=(size1+size2)/2;   // size is in bytes but DSP expect size in words

      // Set play and generate an MCU->DSP interrupt
      mp3_ndb->d_mp3_api_play=(API)1;
      l1_trigger_api_interrupt();

      // Send notification to upper layers
      if((loopback_reinit==TRUE) || (*Cust_get_pointer_status==DATA_AVAILABLE))
        Cust_get_pointer_notify(l1a_l1s_com.mp3_task.parameters.session_id);
    }
    else
    {

	  l1mp3_apihisr_memcpy((UWORD16 *)mp3_ndb->a_mp3_api_input_buffer+mp3_ndb->d_mp3_api_request_index,
							 (UWORD16 *)*mp3_buffer_ptr,
							 size1);

      // Last buffer to be played
      mp3_ndb->d_mp3_api_provided_size=size1/2;   // size is in bytes, but DSP expects size in words
      if(init==TRUE)
        mp3_ndb->d_mp3_api_stop=(API)1;
      else
        mp3_ndb->d_mp3_api_end=(API)1;

      // Set play and generate an MCU->DSP interrupt
      mp3_ndb->d_mp3_api_play=(API)1;
      l1_trigger_api_interrupt();

      if(l1a_l1s_com.mp3_task.parameters.loopback==TRUE)
      {
        status=MP3_FILL_LOOPBACK;
        Cust_get_pointer_notify(l1a_l1s_com.mp3_task.parameters.session_id);
      }
      else
        status=MP3_FILL_END;
    }
  }
  else
  {
    // Current MP3 buffer is enough to fill DSP request
    l1mp3_apihisr_memcpy((UWORD16 *)mp3_ndb->a_mp3_api_input_buffer+mp3_ndb->d_mp3_api_request_index,
                         (UWORD16 *)*mp3_buffer_ptr,
                         2*mp3_ndb->d_mp3_api_request_size);

    *mp3_buffer_ptr+=mp3_ndb->d_mp3_api_request_size;

    mp3_ndb->d_mp3_api_provided_size=mp3_ndb->d_mp3_api_request_size;

    // Set play and generate an MCU->DSP interrupt
    mp3_ndb->d_mp3_api_play=(API)1;
    l1_trigger_api_interrupt();
  }

  return status;
}


/*---------------------------------------------------------------*/
/* l1mp3_apihisr_memcpy()                                        */
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
void l1mp3_apihisr_memcpy(UWORD16 *dst, const UWORD16 *src, UWORD16 size)
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
   *dst=(*src)>>8;
}

#endif  // L1_MP3
#endif // #if (L1_MP3_SIX_BUFFER == 1)

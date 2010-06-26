/************* Revision Controle System Header *************
 *                  GSM Layer 1 software
 * L1GTT_BACK.C
 *
 *        Filename l1gtt_back.c
 *  Copyright 2003 (C) Texas Instruments  
 *
 ************* Revision Controle System Header *************/


#include "l1_macro.h"
#include "l1_confg.h"

#if (L1_GTT == 1)

  #if (CODE_VERSION == SIMULATION)
    #include <string.h>
    #include <ctype.h>
    #include "l1_types.h"
    #include "sys_types.h"
    #include "l1_const.h"
    #include "l1_signa.h"

    #if TESTMODE
      #include "l1tm_defty.h"
    #endif

    #if (AUDIO_TASK == 1)
      #include "l1audio_const.h"
      #include "l1audio_cust.h"
      #include "l1audio_defty.h"
    #endif

    #include "fifo.h"
    #include "ctm_defines.h"
    #include "ctm_transmitter.h"
    #include "ctm_receiver.h"
    #include "l1gtt_baudot_functions.h"
    #include "ucs_functions.h"

    #include "l1gtt_const.h"
    #include "l1gtt_defty.h"
    #include "l1gtt_msgty.h"
    #include "l1gtt_signa.h"
    #include "l1gtt_varex.h"

    #if (L1_MP3 == 1)
      #include "l1mp3_defty.h"
    #endif

    #if (L1_MIDI == 1)
      #include "l1midi_defty.h"
    #endif

    #include "l1_defty.h"
    #include "cust_os.h"
    #include "l1_msgty.h"
    #include "l1_varex.h"
    #include "l1_proto.h"
    #include "l1_mftab.h"

    #include "l1_tabs.h"

    #if L2_L3_SIMUL
      #include "hw_debug.h"
    #endif
  #else
    #include <string.h>
    #include <ctype.h>
    #include "l1_types.h"
    #include "sys_types.h"
    #include "l1_const.h"
    #include "l1_signa.h"

    #if TESTMODE
      #include "l1tm_defty.h"
    #endif

    #if (AUDIO_TASK == 1)
      #include "l1audio_const.h"
      #include "l1audio_cust.h"
      #include "l1audio_defty.h"
    #endif

    #include "fifo.h"
    #include "ctm_defines.h"
    #include "ctm_transmitter.h"
    #include "ctm_receiver.h"
    #include "l1gtt_baudot_functions.h"
    #include "ucs_functions.h"

    #include "l1gtt_const.h"
    #include "l1gtt_defty.h"
    #include "l1gtt_msgty.h"
    #include "l1gtt_signa.h"
    #include "l1gtt_varex.h"

    #if (L1_MP3 == 1)
      #include "l1mp3_defty.h"
    #endif

    #if (L1_MIDI == 1)
      #include "l1midi_defty.h"
    #endif

    #include "l1_defty.h"
    #include "cust_os.h"
    #include "l1_msgty.h"
    #include "l1_varex.h"
    #include "l1_proto.h"
    #include "l1_mftab.h"

    #include "l1_tabs.h"

    #if L2_L3_SIMUL
      #include "hw_debug.h"
    #endif
  #endif

  #if (TRACE_TYPE == 1) || (TRACE_TYPE == 4) || (TRACE_TYPE == 5)
    #include "rvt_gen.h"
    #include "l1_trace.h"
  #endif

  /* Enabling the below flag, results in a new 'GTT' trace, which traces
   * the Softdecision data of CTM demodulator given by DSP to MCU.
   * The trace appears as DLD (N1 N2): <soft decision samples> ....
   *   where,
   *    N1- Is the number of Softdecision samples given by DSP to MCU
   *    N2- Is the number of Softdecision samples processed by MCU, as MCU
   *        can process a max of MAX_CTM_SAMPLES_IN samples
   *   <soft decision samples> - The actual softdecision samples given by
   *         DSP                           
   * */
  #define CTM_RX_DL_SD_TRACE  (0)

  /**************************************/
  /* Prototypes                         */
  /**************************************/

#if (OP_L1_STANDALONE == 1)
  void l1gtt_task(UWORD32 argc, void *argv);
#endif
  void l1gtt_main_processing(void);
  void l1gtt_dl_processing(void);
  void l1gtt_ul_processing(void);

  /**************************************/
  /* External prototypes                */
  /**************************************/
  extern void    TTY_fifo_initialize            (void);
  extern void    l1gtt_initialize               (void);
  extern void    l1gtt_exit                     (void);
  extern void    l1gtt_init_trace               (void);

#if (OP_L1_STANDALONE == 1)
  /*-------------------------------------------------------*/
  /* l1gtt_task()                                          */
  /*-------------------------------------------------------*/
  /*                                                       */
  /* Description:                                          */
  /* ------------                                          */
  /* L1 GTT background task function used to perform GTT   */
  /* processing (TTY encoding/decoding, CTM encoding/      */
  /* decoding). This task is an infinite loop waiting for  */
  /* an event set by L1S in order to start processing.     */
  /*-------------------------------------------------------*/
  void l1gtt_task(UWORD32 argc, void *argv)
  {
    UWORD8 timeout = 0; /* Indicates No Timeout. */
    int return_flags;

    l1gtt_init_trace();

    while(1)
    {
      return_flags = os_retrieve_events(L1GTTBACK_EVENTS, 0xFF);

      if(return_flags & INIT_EVENT)
      {
        l1gtt_initialize();
      }
      else if(return_flags & EXIT_EVENT)
      {
        l1gtt_exit();
      }
      else if(return_flags & DATA_AVAIL_EVENT)
      {
        l1gtt_main_processing();
      } /* End of else if(return_flags & DATA_AVAIL_EVENT) */
    }
  }
#endif // OP_L1_STANDALONE

   extern T_GTT_DEBUG l1_gtt_debug;

  /*-------------------------------------------------------*/
  /* l1gtt_main_processing()                               */
  /*-------------------------------------------------------*/
  /*                                                       */
  /* Description:                                          */
  /* ------------                                          */
  /* This function executes the processing to be done in   */
  /* the GTT background task:                              */
  /* - read the data coming from DSP CTM estimator         */
  /* - read the TTY Baudot samples coming from the DSP     */
  /*   Baudot estimator                                    */
  /* - process the UL and DL data                          */
  /*-------------------------------------------------------*/
 
  #if (CTM_RX_DL_SD_TRACE == 1) 
    WORD16  inputSampleBufferDL_log[MAX_CTM_SAMPLES_IN];   
    UWORD32 dl_data_count_log_g =0;
    UWORD32 num_entries_actual_log_g =0;
  #endif  

  void l1gtt_main_processing(void)
  {
      WORD16  inputSampleBufferDL[MAX_CTM_SAMPLES_IN];
      WORD16  i = 0;
      int dl_data_count = 0;
      char    str[128];

      l1_gtt_debug.fn_gtt_main_start = l1s.actual_time.fn_mod42432;

      /************    Read the incoming CTM code data from DSP    ***********/
      if(global_dl_ptr->num_entries_actual>=MAX_CTM_SAMPLES_IN)
      {
             dl_data_count = MAX_CTM_SAMPLES_IN;
      }
      else
             dl_data_count = global_dl_ptr->num_entries_actual;

      #if (CTM_RX_DL_SD_TRACE == 1) 
        num_entries_actual_log_g = global_dl_ptr->num_entries_actual;        
      #endif  

      /* Remove downlink data from global buffer for processing */
      Shortint_fifo_pop(global_dl_ptr,inputSampleBufferDL,dl_data_count);

      #if (CTM_RX_DL_SD_TRACE == 1)       
      {
        memcpy( inputSampleBufferDL_log, inputSampleBufferDL, MAX_CTM_SAMPLES_IN*sizeof(UWORD16));
        dl_data_count_log_g = dl_data_count;
      }
      #endif       

      /* Convert downlink data to odd valued for wait_for_sync purposes */
      for(i=0;i<dl_data_count;i++)
      {
           if( inputSampleBufferDL[i] != 0 )
             inputSampleBufferDL[i] |= 0x0001;
      }

      /************  Read the incoming Baudot sample data from DSP  ***********/
       Shortint_fifo_push(signalFifoState_ptr,
                          inputSampleBufferDL,
                          dl_data_count);

      /************    Start the data processing main loop    ***********/
      while(dl_data_count >= 2)
      {
          /* Increment processing counter by 1. */
          cyclesSinceLastEnquiryBurst++;

          /* There's CTM code data or TTY code data available to be processed */
          /* Run the CTM receiver */
          ctm_receiver(signalFifoState_ptr,
                       ctmOutTTYCodeFifoState_ptr,
                       &earlyMutingRequired,
                       rx_state_ptr);

          dl_data_count = dl_data_count-2;

          enquiryFromFarEndDetected = false;

          /* Check whether the far-end side is able to support CTM signals */
          if ((rx_state_ptr->wait_state.sync_found) && (!ctmFromFarEndDetected))
          {
              ctmFromFarEndDetected = true;
              if(transparentMode == true)
              {
                transparentMode = false;
                // Received CTM in the downlink; Enable bypass path mute
                l1s_dsp_com.dsp_ndb_ptr->d_tty2x_dl_bypass_mute = C_DL_BYPASS_MUTE_ENABLE;
                
                cntTransmittedEnquiries=0;
                cyclesSinceLastEnquiryBurst=0;
              }

              /* If we have not transmitted CTM tones so far, we should
               * treat the received burst as an enquiry burst.          */
              if (!ctmCharacterTransmitted)
              {
                  enquiryFromFarEndDetected=true;
                  cntSamplesSinceEnquiryDetected=0;
              }
          }
          l1gtt_dl_processing();

          /* If transparent mode, don't do uplink processing */
          if(!transparentMode)
          {
              l1gtt_ul_processing();
          }

          if(cntFramesSinceBurstInit<WORD16_MAX)
          {
              cntFramesSinceBurstInit++;  /* Increment 1 every 5ms */
          }
      } /* end of while() */

      #if (CTM_RX_DL_SD_TRACE == 1) 
      #if (TRACE_TYPE == 1) || (TRACE_TYPE == 4) || (TRACE_TYPE == 5)
        if (trace_info.current_config->l1_dyn_trace & 1<<L1_DYN_TRACE_GTT)
        { 
          char str_g[256];
          int i=0;
          sprintf(str_g,"DLD (%ld %ld): ", num_entries_actual_log_g, dl_data_count_log_g);
          for(i=0;((i<dl_data_count_log_g) && (i<40));i++)
          {
             char str[8];
             sprintf(str,"%4.4x ",((UWORD16)(inputSampleBufferDL_log[i])));
             strcat(str_g,str);
             if(((i+1)&0x7) == 0){
               strcat(str_g,"\n\r");
             }
          }
          strcat(str_g,"\n\r");
          GTT_send_trace_cpy(str_g);

          if(dl_data_count_log_g >= 40)
          {
            sprintf(str_g,"DLD_C: ");
            for(i=40;((i<dl_data_count_log_g));i++)
            {
              char str[8];
              sprintf(str,"%4.4x ",((UWORD16)(inputSampleBufferDL_log[i])));
              strcat(str_g,str);
            }
            strcat(str_g,"\n\r");
            GTT_send_trace_cpy(str_g);
          }
        }
      #endif  
      #endif  
      
      
      l1_gtt_debug.fn_gtt_main_end = l1s.actual_time.fn_mod42432;      
      if(l1_gtt_debug.fn_gtt_proc_flag>0)
        l1_gtt_debug.fn_gtt_proc_flag--;
      
      #if((TRACE_TYPE == 1) || (TRACE_TYPE == 4))      
        sprintf(str,"GTT MAIN %d %ld %ld %ld %ld %ld\n\r", 
            l1_gtt_debug.fn_gtt_proc_flag,
            l1_gtt_debug.fn_gtt_error_count,
            l1_gtt_debug.fn_gtt_sync,
            l1_gtt_debug.fn_gtt_main_start,
            l1_gtt_debug.fn_gtt_main_end,
            l1_gtt_debug.odd_fifo3_count); 
        rvt_send_trace_cpy ((T_RVT_BUFFER)str, trace_info.l1_trace_user_id, (T_RVT_MSG_LG)strlen(str), RVT_ASCII_FORMAT); 
      #endif        
      
  }

  /*-------------------------------------------------------*/
  /* l1gtt_dl_processing()                                 */
  /*-------------------------------------------------------*/
  /*                                                       */
  /* Description:                                          */
  /* ------------                                          */
  /* Execute the GTT DL data processing (processing from   */
  /* CTM estimator to TTY modulator)                       */
  /* - CTM receiver execution (except CTM estimator)       */
  /* - UTF-8 -> TTY text conversion                        */
  /* - Baudot encoding                                     */
  /*-------------------------------------------------------*/
  void l1gtt_dl_processing()
  {
      WORD16     ttyCode = 0;
      UWORD16    ucsCode = 0;
      char         character;

     /************ First, we process the transmission from the ***********/
     /************ CTM receiver to the Baudot encoder.         ***********/

     /* If there is a character from the CTM receiver available
      * push it into ctmToBaudotFifo. */

     if(ctmOutTTYCodeFifoState_ptr->num_entries_actual>0)
     {
          /* If we tried to begin the connection but there was a failure, then we came back to */
          /* transparent mode. But now we are still receiving CTM characters: hence, if we discover */
          /* a potential CTM burst, we re-set the non transparent mode (ETSI 26.226 p 18)*/


          Shortint_fifo_pop(ctmOutTTYCodeFifoState_ptr,
                            (WORD16 *)&ucsCode,
                            1);

          /* Check whether this was an enquiry burst from the other
           * side. Ignore this enquiry, if the last enquiry has
           * been detected less than 25 frames (500 ms) ago.        */
          /* CTM sample rate is 2bits/5ms */
          if ((ucsCode==ENQU_SYMB) &&
              (cntSamplesSinceEnquiryDetected > (500/5)))
          {
              enquiryFromFarEndDetected=true;
              cntSamplesSinceEnquiryDetected=0;

  #if (TRACE_TYPE == 1) || (TRACE_TYPE == 4) || (TRACE_TYPE == 5)
              if (trace_info.current_config->l1_dyn_trace & 1<<L1_DYN_TRACE_GTT)
              {
                  char str[40];

                  sprintf(str,"Enquiry from far end detected.\n\r");

                  GTT_send_trace_cpy(str);
              }
  #endif /* End trace */

          }
          else
          {
              /* Convert character from UCS to Baudot code, print
               * it on the screen and push it into ctmToBaudotFifo. */
              character = toupper(convertUCScode2char(ucsCode));
              ttyCode   = convertChar2ttyCode(character);

              if (ttyCode >= 0)
              {
  #if (TRACE_TYPE == 1) || (TRACE_TYPE == 4) || (TRACE_TYPE == 5)
                  if (trace_info.current_config->l1_dyn_trace & 1<<L1_DYN_TRACE_GTT)
                  {
                    char str[30];

                    switch(character)
                    {
                      case 10:
                        sprintf(str,"CTM RX OUT: <LF>\n\r");
                        break;
                      case 13:
                        sprintf(str,"CTM RX OUT: <CR>\n\r");
                        break;
                      case 32:
                        sprintf(str,"CTM RX OUT: <SPACE>\n\r");
                         break;
                      default:
                         sprintf(str,"CTM RX OUT: %c\n\r",character);
                         break;
                    }

                    GTT_send_trace_cpy(str);
                  }
  #endif/* End trace */

  #if L2_L3_SIMUL
         if (l1a_l1s_com.gtt_task.test.scen_enable != TTY_ENC_INPUT)
  #endif
                  Shortint_fifo_push(ctmToBaudotFifoState_ptr,
                                     &ttyCode,
                                     1);

              }
          } /* End of else of if ((ucsCode==ENQU_SYMB) */
     }

#if L2_L3_SIMUL
         // GTT test
         else
         if (l1a_l1s_com.gtt_task.test.scen_enable == TTY_ENC_INPUT)
         {
           // In case of test, the input scenario is injected at
           // the input of the Baudot encoder
           while(l1s.gtt_test.chars_to_push > 0)
           {
             // Read 'character' from scenario
             character = l1s.gtt_test.scen_ptr[l1s.gtt_test.scen_index++];
             if (l1s.gtt_test.scen_index >= l1s.gtt_test.scen_length)
               l1s.gtt_test.scen_index = 0;

             ttyCode   = convertChar2ttyCode(character);

             Shortint_fifo_push(ctmToBaudotFifoState_ptr,
                                &ttyCode,
                                1);

            #if (TRACE_TYPE == 1) || (TRACE_TYPE == 5)
              if (trace_info.current_config->l1_dyn_trace & 1<<L1_DYN_TRACE_GTT)
              {
                  char str[30];

                  switch(character)
                  {
                    case 10:
                      sprintf(str,"Baudot enc IN: <LF>\n\r");
                      break;
                    case 13:
                      sprintf(str,"Baudot enc IN: <CR>\n\r");
                      break;
                    case 32:
                      sprintf(str,"Baudot enc IN: <SPACE>\n\r");
                      break;
                    default:
                      sprintf(str,"Baudot enc IN: %c\n\r",character);
                      break;
                  }

                  GTT_send_trace_cpy(str);
             }
           #endif

             l1s.gtt_test.chars_to_push--;
           }
         }
#endif

     /* Count the CTM samples, to get an estimation of time passed
      * Assume that the CTM samples are continuesly coming         */
     if (cntSamplesSinceEnquiryDetected <WORD16_MAX)
     {
          cntSamplesSinceEnquiryDetected ++;
     }

     /* If there are characters from the CTM receiver, or if the CTM
      * receiver has detected a synchronisation preamble, or if the
      * Baudot Modulator is still busy (i.e. there are still bits to
      * modulate) --> run the Baudot Modulator (again). This branch is
      * also executed (but with the Baudot Modulator in idle mode), if
      * the ctm_receiver has indicated that early_muting is required.    */
     if((ctmToBaudotFifoState_ptr->num_entries_actual >0) || earlyMutingRequired)
     {
       cntSamplesSinceLastCTMFromBypass = 0;       
       l1s_dsp_com.dsp_ndb_ptr->d_tty2x_dl_bypass_mute = C_DL_BYPASS_MUTE_ENABLE;       
       /* Compared the available buffer space for baudot encoding with 14 words.
        * This is because some characters may require SHIFT (FIGS/LTRS)
        * to be encoded also. */
       if((MAX_BAUDOT_BITS_OUT - (baudotCodeFifoState_ptr->num_entries_actual)) >= 14)
       {
      /* If there is a character on the ctmToBaudotFifo available and
       * if the Baudot modulator is able to process a new character
       * --> pop the character from the fifo.                         */
         if((ctmToBaudotFifoState_ptr->num_entries_actual >0) &&
            (cntSamplesSinceLastBypassFromCTM>=200/5)) //10 frame: 200ms
         {
             Shortint_fifo_pop(ctmToBaudotFifoState_ptr, &ttyCode, 1);
         }
         else
         {
             ttyCode = -1;
         }

         baudot_encode(ttyCode,
                       baudotCodeFifoState_ptr,
                       baudot_encode_state_ptr);
       }/* End of if((MAX_BAUDOT_BITS_OUT ... ) >= 14) */

         if (cntSamplesSinceLastBypassFromCTM < WORD16_MAX)
         {
             cntSamplesSinceLastBypassFromCTM++;
         }

     }
     else
     {
       if(cntSamplesSinceLastCTMFromBypass >= (500/5)) // 500 msec
       {
         l1s_dsp_com.dsp_ndb_ptr->d_tty2x_dl_bypass_mute = C_DL_BYPASS_MUTE_DISABLE;                       
       }
       if(cntSamplesSinceLastCTMFromBypass < WORD16_MAX)
       {
         cntSamplesSinceLastCTMFromBypass++;
       }
      /* should go into bypass state, stop generating output now */
         cntSamplesSinceLastBypassFromCTM = 0;
     }
  }

  /*-------------------------------------------------------*/
  /* l1gtt_ul_processing()                                 */
  /*-------------------------------------------------------*/
  /*                                                       */
  /* Description:                                          */
  /* ------------                                          */
  /* Execute the GTT UL data processing (processing from   */
  /* TTY estimator to CTM modulator)                       */
  /* - Baudot decoding                                     */
  /* - TTY text -> UTF-8 conversion                        */
  /* - CTM transmitter execution (except CTM modulator)    */
  /*-------------------------------------------------------*/
  void l1gtt_ul_processing()
  {
    WORD16     ttyCode = 0;
    UWORD16    ucsCode = 0;
    char       character;

      /************ Now we process the transmission from the    *************/
      /************ Baudot demodulator to the CTM transmitter   *************/
      if(enquiryFromFarEndDetected)
      {
          /* Generate Acknowledgement burst, if Enquiry from the far side
           * has been detected. The code 0xFFFF has a special meaning,
           * see description of function ctm_transmitter()                */
          if(baudotToCtmFifoState_ptr->num_entries_actual==0)
          {
              ucsCode = 0xFFFF;
              Shortint_fifo_push(baudotToCtmFifoState_ptr,
                                 (WORD16 *)&ucsCode,
                                 1);

              ctmCharacterTransmitted   = true;
              enquiryFromFarEndDetected = false;

  #if (TRACE_TYPE == 1) || (TRACE_TYPE == 4) || (TRACE_TYPE == 5)
              if (trace_info.current_config->l1_dyn_trace & 1<<L1_DYN_TRACE_GTT)
              {
                char str[60];

                sprintf(str,"CTM negotiation: Acknowledgement burst generated.\n\r");

                GTT_send_trace_cpy(str);
              }
  #endif /* End trace */
          }

          transparentMode = false;
          // Received CTM in the downlink; Disable bypass path i.e. mute bypass path
          l1s_dsp_com.dsp_ndb_ptr->d_tty2x_dl_bypass_mute = C_DL_BYPASS_MUTE_ENABLE;

          cntTransmittedEnquiries = 0;
      }
      else if(!ctmFromFarEndDetected && ctmTransmitterIsIdle &&
              (global_ul_ptr->num_entries_actual>0) &&
              (cntTransmittedEnquiries<NUM_ENQUIRY_BURSTS) &&
              ((cyclesSinceLastEnquiryBurst>=ENQUIRY_BURST_TIMEOUT)||
              (cntTransmittedEnquiries==0)) || (!ctmFromFarEndDetected
               && (cntTransmittedEnquiries>0) &&
               (cyclesSinceLastEnquiryBurst>=ENQUIRY_BURST_TIMEOUT) &&
               (cntTransmittedEnquiries<NUM_ENQUIRY_BURSTS)))
      {
         /* Generate an Enquiry Burst, if one of the following
          * conditions is fulfilled:
          * - The first five bits of a character have been detected
          *   by the Baudot demodulator and no CTM frames have been
          *   transmitted so far.
          * - There is at least one character to transmit, but CTM
          *   receiver hasn't received an acknowledgement for the far-end
          *   so far. In this case an Enquiry Burst is initiated, if the
          *   last Burst is finished and if the number of enquiry bursts
          *   doesn't exceed NUM_ENQUIRY_BURSTS.
          * - The amount of time passed since last enquiry burst > 1320ms. */
          ucsCode = ENQU_SYMB;
          Shortint_fifo_push(baudotToCtmFifoState_ptr,
                             (WORD16 *)&ucsCode,
                             1);

          ctmCharacterTransmitted = true;
          if(cntTransmittedEnquiries<WORD16_MAX)
          {
              cntTransmittedEnquiries++;
          }

          /* Reset enquiry burst timer */
          cyclesSinceLastEnquiryBurst = 0;
      } /* End of else if() */

      /* The CTM transmitter is executed in the following cases:
       * - Character from Baudot demodulator available or start + info bits
       *   detected, provided that we know that the other side supports CTM
       *   or if we are still in the negotiation phase
       * - There is still a character in the baudotToCtmFifo
       * - The CTM Modulator is still running
       * Otherwise, the audio samples are bypassed!                         */

#if L2_L3_SIMUL
      // Note: CTM transmitter is also executed in case of GTT test loop
      //       the input scenario is injected to CTM trasnsmitter and the
      //       output is traced at the output of the Baudot decoder
      if (((global_ul_ptr->num_entries_actual>0) &&
           (ctmFromFarEndDetected || (cntFramesSinceBurstInit<ENQUIRY_TIMEOUT))) ||
          (baudotToCtmFifoState_ptr->num_entries_actual>0) ||
          tx_state_ptr->burstActive ||
          (cntTransmittedEnquiries<=NUM_ENQUIRY_BURSTS) ||
          (l1a_l1s_com.gtt_task.test.scen_enable == CTM_TX_INPUT))
#else
      if (((global_ul_ptr->num_entries_actual>0) &&
           (ctmFromFarEndDetected || (cntFramesSinceBurstInit<ENQUIRY_TIMEOUT))) ||
          (baudotToCtmFifoState_ptr->num_entries_actual>0) || 
          tx_state_ptr->burstActive ||
          (cntTransmittedEnquiries<=NUM_ENQUIRY_BURSTS))
#endif
      {
          /*  Transition from idle into active mode represents start of burst */
          if(ctmTransmitterIsIdle)
          {
              cntFramesSinceBurstInit = 0;
          }
         /* If there is a character from the Baudot receiver available
          * and if the CTM transmitter is able to process a new character:
          * pop the character from the fifo.                               */

         if((global_ul_ptr->num_entries_actual>0) &&
            (baudotToCtmFifoState_ptr->num_entries_actual==0) &&
            (ctmFromFarEndDetected || (cntFramesSinceBurstInit<ENQUIRY_TIMEOUT)))
         {
             Shortint_fifo_pop(global_ul_ptr,
                               &ttyCode,
                               1);

#if (TRACE_TYPE == 1) || (TRACE_TYPE == 4) || (TRACE_TYPE == 5)
            if (trace_info.current_config->l1_dyn_trace & 1<<L1_DYN_TRACE_GTT)
            {
               char str[30];

               switch(convertTTYcode2char(ttyCode))
               {
                 case 10:
                    sprintf(str,"Baudot dec OUT: <LF>\n\r");
                    break;
                 case 13:
                    sprintf(str,"Baudot dec OUT: <CR>\n\r");
                    break;
                 case 32:
                    sprintf(str,"Baudot dec OUT: <SPACE>\n\r");
                    break;
                 default:
                    sprintf(str,"Baudot dec OUT: %c\n\r",convertTTYcode2char(ttyCode));
                    break;
               }

               GTT_send_trace_cpy(str);
            }
#endif /* End trace */

             character = convertTTYcode2char(ttyCode);

           #if L2_L3_SIMUL
             // GTT test

             // In case of test, no character is given to the CTM transmitter since
             // it is done by the test input
             if (l1a_l1s_com.gtt_task.test.scen_enable != CTM_TX_INPUT)
           #endif

             {
               ucsCode = convertChar2UCScode(character);

               Shortint_fifo_push(baudotToCtmFifoState_ptr,
                                  (WORD16 *)&ucsCode,
                                  1);
             }
         }

#if L2_L3_SIMUL
         // GTT test
         else
         if (l1a_l1s_com.gtt_task.test.scen_enable == CTM_TX_INPUT)
         {
           // In case of test, the input scenario is injected at
           // the input of the CTM transmitter
           while(l1s.gtt_test.chars_to_push > 0)
           {
             // Read 'character' from scenario
             character = l1s.gtt_test.scen_ptr[l1s.gtt_test.scen_index++];
             if (l1s.gtt_test.scen_index >= l1s.gtt_test.scen_length)
               l1s.gtt_test.scen_index = 0;

             ucsCode = convertChar2UCScode(character);

             Shortint_fifo_push(baudotToCtmFifoState_ptr,
                                (WORD16 *)&ucsCode,
                                1);

            #if (TRACE_TYPE == 1) || (TRACE_TYPE == 5)
              if (trace_info.current_config->l1_dyn_trace & 1<<L1_DYN_TRACE_GTT)
              {
                  char str[30];

                  switch(character)
                  {
                    case 10:
                      sprintf(str,"CTM TX IN: <LF>\n\r");
                      break;
                    case 13:
                      sprintf(str,"CTM TX IN: <CR>\n\r");
                      break;
                    case 32:
                      sprintf(str,"CTM TX IN: <SPACE>\n\r");
                      break;
                    default:
                      sprintf(str,"CTM TX IN: %c\n\r",character);
                      break;
                  }

                  GTT_send_trace_cpy(str);
             }
           #endif

             l1s.gtt_test.chars_to_push--;
           }
         }
#endif

         if(baudotToCtmFifoState_ptr->num_entries_actual>0)
         {
             Shortint_fifo_pop(baudotToCtmFifoState_ptr,
                               (WORD16 *)&ucsCode,
                               1);
         }
         else
         {
             ucsCode = IDLE_SYMB;
         }

  #if (TRACE_TYPE == 1) || (TRACE_TYPE == 4) || (TRACE_TYPE == 5)
         if (trace_info.current_config->l1_dyn_trace & 1<<L1_DYN_TRACE_GTT)
         {
           if(ucsCode==ENQU_SYMB)
           {
             char str[50];

             sprintf(str,"CTM negotiation: Enquiry burst generated\n\r");

             GTT_send_trace_cpy(str);
           }
         }
  #endif /* End trace */

         ctm_transmitter(ucsCode,
                         ctmCodeULFifoState_ptr,
                         tx_state_ptr);

         ctmTransmitterIsIdle = !tx_state_ptr->burstActive;
         ctmCharacterTransmitted = true;
      }
      else
      {

  #if (TRACE_TYPE == 1) || (TRACE_TYPE == 4) || (TRACE_TYPE == 5)
         if (trace_info.current_config->l1_dyn_trace & 1<<L1_DYN_TRACE_GTT)
         {
           if(tx_state_ptr->fifo_state.num_entries_actual>0)
           {
                char str[30];

                sprintf(str,"Unprocessed UL data.\n\r");

                GTT_send_trace_cpy(str);
           }
         }
  #endif /* End trace */

         /* discard characters in oder to avoid FIFO buffer overflows */
         if(global_ul_ptr->num_entries_actual >= global_ul_fifo_length-1)
         {
             Shortint_fifo_pop(global_ul_ptr,
                               &ttyCode,
                               1);

  #if (TRACE_TYPE == 1) || (TRACE_TYPE == 4) || (TRACE_TYPE == 5)
         if (trace_info.current_config->l1_dyn_trace & 1<<L1_DYN_TRACE_GTT)
         {
             char str[60];

             sprintf(str,"Data in global_ul discarded.\n\r");

             GTT_send_trace_cpy(str);
         }
  #endif /* End trace */
         }
      }
     /* After three unanswered Enquiry bursts we get into transparent mode */
     if((cntTransmittedEnquiries==NUM_ENQUIRY_BURSTS) &&
        (cyclesSinceLastEnquiryBurst>=ENQUIRY_BURST_TIMEOUT)&&
        (!ctmFromFarEndDetected) &&
        (!transparentMode))
     {
      transparentMode = true;
      // Did not receive CTM in the downlink for a long time; Disable bypass path mute
      l1s_dsp_com.dsp_ndb_ptr->d_tty2x_dl_bypass_mute = C_DL_BYPASS_MUTE_DISABLE;      

      cntFramesSinceBurstInit          = 0;
      ctmCharacterTransmitted   = false;
      enquiryFromFarEndDetected = false;
      ctmTransmitterIsIdle      = true;
      cntSamplesSinceEnquiryDetected = 0;

      /* Reset the CTM transmitter state machine. */
      reset_ctm_transmitter(tx_state_ptr);

      /* Reset the fifo buffers used in uplink processing */
      Shortint_fifo_reset(global_ul_ptr);
      Shortint_fifo_reset(baudotToCtmFifoState_ptr);
      Shortint_fifo_reset(ctmCodeULFifoState_ptr);

  #if (TRACE_TYPE == 1) || (TRACE_TYPE == 4) || (TRACE_TYPE == 5)
       if (trace_info.current_config->l1_dyn_trace & 1<<L1_DYN_TRACE_GTT)
       {
         char str[40];

         sprintf(str,"Entering transparent mode\n\r");

         GTT_send_trace_cpy(str);
       }
  #endif /* End trace */
     }
  }

#endif // L1_GTT

/*
*******************************************************************************
*
*      COPYRIGHT (C) 2000 BY ERICSSON EUROLAB DEUTSCHLAND GmbH
*      90411 NUERNBERG, GERMANY, Tel Int + 49 911 5217 100
*
*      The program(s) may be used and/or copied only with the
*      written permission from Ericsson or in accordance
*      with the terms and conditions stipulated in the agreement or
*      contract under which the program(s) have been supplied.
*
*******************************************************************************
*
*      File             : ctm_receiver.c
*      Author           : EEDN/RV Matthias Doerbecker
*      Tested Platforms : Sun Solaris, MS Windows NT 4.0
*      Description      : Complete CTM Receiver including Demodulator, 
*                         Synchronisation, Deinterleaver, and Error Correction
*
*      Changes since October 13, 2000:
*      - added reset function reset_ctm_receiver()
*
*      Changes since December 07, 2000:
*      - Bug fix within the code for initial synchronization 
*        based on the detection of the resync sequence
*
*      $Log: $
*
*******************************************************************************
*/

#include "l1_macro.h"
#include "l1_confg.h"

#if (L1_GTT == 1)

  /*
  *******************************************************************************
  *                         INCLUDE FILES
  *******************************************************************************
  */
  #include "l1_types.h"
  #include "ctm_receiver.h"
  #include "ctm_defines.h"
  #include "init_interleaver.h"
  #include "diag_deinterleaver.h"
  #include "m_sequence.h"
  #include "wait_for_sync.h"
  #include "conv_poly.h"
  #include "viterbi.h"
  #include "ucs_functions.h"
  #include "l1gtt_const.h"
  #include <fifo.h>
  #include <stdio.h> 

  #if (TRACE_TYPE == 1) || (TRACE_TYPE == 4) || (TRACE_TYPE == 5)
    #include "rvt_gen.h"
    #include "l1_trace.h"
  #endif

  extern void *l1gtt_malloc(UWORD32 size);
  extern void l1gtt_free(void *ptr);

  /***********************************************************************/
  /* init_ctm_receiver()                                                 */
  /* *******************                                                 */
  /* Initialization of the CTM Receiver.                                 */
  /*                                                                     */
  /* output vaiables:                                                    */
  /* rx_state :   pointer to a variable of rx_state_t containing the     */
  /*              initialized states of the receiver                     */
  /***********************************************************************/

  void init_ctm_receiver(rx_state_t* rx_state)
  {
      rx_state->samplingCorrection        = 0;
      rx_state->cntIdleSymbols            = 0;
      rx_state->numDeintlBits             = 0;
      rx_state->cntRXBits                 = 0;
      rx_state->syncCorrect               = 0;
      rx_state->cntUnreliableGrossBits    = 0;
    
      /* set up fifo buffers */
#if(TTY_SYNC_MCU_2==0)
      Shortint_fifo_init(&(rx_state->rx_bits_fifo_state), 
                          intlvB*intlvB*intlvD+CHC_RATE);
      Shortint_fifo_init(&(rx_state->net_bits_fifo_state), 
                          (intlvB*intlvB*intlvD)/CHC_RATE+3+BITS_PER_SYMB);
    
      Shortint_fifo_init(&(rx_state->octet_fifo_state), 5);
#else
      Shortint_fifo_init(&(rx_state->rx_bits_fifo_state), 
                     intlvB*intlvB*intlvD+RESYNC_OFFSET+CHC_RATE);
      Shortint_fifo_init(&(rx_state->net_bits_fifo_state), 
                     (intlvB*intlvB*intlvD+RESYNC_OFFSET)/CHC_RATE+3+BITS_PER_SYMB);
    
      Shortint_fifo_init(&(rx_state->octet_fifo_state), 5+RESYNC_OFFSET/(CHC_RATE*BITS_PER_SYMB));
#endif
    
      /* Initialize the demodulator */
      /* ***************** To Uncomment @@@@@@@@@@@@@@@@@ **************
       init_tonedemod(&(rx_state->tonedemod_state));
      * ***************** To Uncomment @@@@@@@@@@@@@@@@@ **************/

      /* Initialize the viterbi decoder */
      viterbi_init(&(rx_state->viterbi_state));
    
      calc_mute_positions(rx_state->mutePositions, 
                          NUM_MUTE_ROWS, 
                          intlvB-1,
                          intlvB, 
                          intlvD);

      /* initialize interleaver/deinterleaver/wait_for_sync */
      /* and allocate memory for input/output vectors       */

      init_deinterleaver(&(rx_state->deintl_state), 
                         intlvB, 
                         intlvD);

      init_wait_for_sync(&(rx_state->wait_state), 
                         intlvB, 
                         intlvD, 
                         deintSyncLns);

      rx_state->waitSyncOut 
       = (WORD16*)l1gtt_malloc((2+rx_state->wait_state.length_shift_reg-1)
                             *sizeof(WORD16));

      if (rx_state->waitSyncOut==(WORD16*)NULL)
      {
  #if (TRACE_TYPE == 1) || (TRACE_TYPE == 4) || (TRACE_TYPE == 5)
        if (trace_info.current_config->l1_dyn_trace & 1<<L1_DYN_TRACE_GTT)
        {
          char str[50];
          sprintf(str, "Error while allocating memory for waitSyncOut\n\r");
        
          GTT_send_trace_cpy(str);
        }
  #endif    /* End trace  */
          //exit(1);  Should never come here !!
      }
#if(TTY_SYNC_MCU_2==1)
      rx_state->deintlOut 
        = (WORD16*)l1gtt_malloc((2+rx_state->wait_state.length_shift_reg-1+RESYNC_OFFSET)
                              *sizeof(WORD16));
#else
      rx_state->deintlOut 
        = (WORD16*)l1gtt_malloc((2+rx_state->wait_state.length_shift_reg-1)
                              *sizeof(WORD16));
#endif

      if (rx_state->deintlOut==(WORD16*)NULL)
      {
  #if (TRACE_TYPE == 1) || (TRACE_TYPE == 4) || (TRACE_TYPE == 5)
        if (trace_info.current_config->l1_dyn_trace & 1<<L1_DYN_TRACE_GTT)
        {
          char str[50];

          sprintf(str, "Error while allocating memory for deintlOut\n\r");

          GTT_send_trace_cpy(str);
        }
  #endif    /* End trace  */
          //exit(1);  Should never come here !!
      }

  }

  /*
   ***********************************************************************
   * Shutdown_ctm_receiver()                                             *
   * ***********************                                             *
   * Shutdown of the CTM Receiver.                                       *
   *                                                                     *
   * Input variables:                                                    *
   * rx_state :   pointer to a variable of rx_state_t containing the     *
   *              initialized states of the receiver                     *
   ***********************************************************************
  */
  void Shutdown_ctm_receiver(rx_state_t* rx_state)
  {
      /* Free fifo buffers. */
      Shortint_fifo_exit(&(rx_state->rx_bits_fifo_state));
      Shortint_fifo_exit(&(rx_state->net_bits_fifo_state));
      Shortint_fifo_exit(&(rx_state->octet_fifo_state));

      /* Calls the deinterleaver shutdown routine to execute buffer  
       * deallocation process. */
      Shutdown_deinterleaver(&(rx_state->deintl_state));

      /* Calls the wait_for_sync shutdown routine to execute buffer  
       * deallocation process. */
      Shutdown_wait_for_sync(&(rx_state->wait_state));

      /* Free dynamically allocated memory */
      l1gtt_free(rx_state->waitSyncOut);
      l1gtt_free(rx_state->deintlOut);
  }

  /*
   ***********************************************************************
   * reset_ctm_receiver()                                                *
   * ********************                                                *
   * Reset of the Cellular Text Telephone Modem receiver, state          *
   * machines and buffer pointers.                                       *
   *                                                                     *
   * Input variables:                                                    *
   * rx_state :   pointer to a variable of rx_state_t containing the     *
   *              initialized states of the receiver                     *
   ***********************************************************************
  */
  void reset_ctm_receiver(rx_state_t* rx_state)
  {
      rx_state->samplingCorrection        = 0;
      rx_state->cntIdleSymbols            = 0;
      rx_state->numDeintlBits             = 0;
      rx_state->cntRXBits                 = 0;
      rx_state->syncCorrect               = 0;
      rx_state->cntUnreliableGrossBits    = 0;
    
      /* reset fifo buffers */
      Shortint_fifo_reset(&(rx_state->rx_bits_fifo_state));
      Shortint_fifo_reset(&(rx_state->net_bits_fifo_state));
      Shortint_fifo_reset(&(rx_state->octet_fifo_state));
    
      /* reset the viterbi decoder */
      viterbi_reinit(&(rx_state->viterbi_state));
    
      reinit_deinterleaver(&(rx_state->deintl_state));
      reinit_wait_for_sync(&(rx_state->wait_state));
  }
#if(TTY_SYNC_MCU_2==1)
void decode_flushed_bits(WORD16 *deint_flush_vect,rx_state_t *rx_state,WORD16 num_flushed_bits,fifo_state_t*  ptr_output_char_fifo_state)
{
WORD16 cnt;
WORD16  fecGrossBits[CHC_RATE];
WORD16 fecNetBit;
WORD16 numViterbiOutBits;
BOOL      octetAvailable;
WORD16  ucsBitsIn[BITS_PER_SYMB];
UWORD16 utfOctet = 0;
UWORD16 ucsCode  = 0;


for (cnt=0; cnt<num_flushed_bits; cnt++)
        {
          rx_state->deintlOut[cnt]=deint_flush_vect[cnt];
          if (rx_state->numDeintlBits >= rx_state->intl_delay+NUM_BITS_BETWEEN_RESYNC+RESYNC_SEQ_LENGTH)
            {
              /* Ignore all bits that are for muting or resync */
             // if (rx_state->deintlOut[cnt]!=0 && (rx_state->cntRXBits<NUM_BITS_BETWEEN_RESYNC))
              if (!(mutingRequired((WORD16)(rx_state->cntRXBits), 
                                   rx_state->mutePositions, 
                                   NUM_MUTE_ROWS*intlvB))
                  && (rx_state->cntRXBits<NUM_BITS_BETWEEN_RESYNC))
                {
                  Shortint_fifo_push(&(rx_state->rx_bits_fifo_state), 
                                     &(rx_state->deintlOut[cnt]), 1);
                }
              
              rx_state->cntRXBits++;
              if(rx_state->cntRXBits
                 ==NUM_BITS_BETWEEN_RESYNC+RESYNC_SEQ_LENGTH)
                rx_state->cntRXBits = 0;
            }
          rx_state->numDeintlBits++;

          /* Avoid Overflows of numDeintlBits */
          if (rx_state->numDeintlBits > 10000)
            rx_state->numDeintlBits 
              -= (NUM_BITS_BETWEEN_RESYNC+RESYNC_SEQ_LENGTH);
        } 
  while (Shortint_fifo_check(&(rx_state->rx_bits_fifo_state)) > CHC_RATE)
    {
      Shortint_fifo_pop(&(rx_state->rx_bits_fifo_state), 
                        fecGrossBits, CHC_RATE);
      
      /* Count gross bits with low reliability (i.e. bits with too low */
      /* magnitute or with their LSB not set).                         */
      for (cnt=0;cnt<CHC_RATE; cnt++)
        if ((abs(fecGrossBits[cnt])<THRESHOLD_RELIABILITY_FOR_GOING_OFFLINE)
            || ((fecGrossBits[cnt] & 0x0001) == 0))
          {
            if (rx_state->cntUnreliableGrossBits < WORD16_MAX)
              rx_state->cntUnreliableGrossBits++;
          }
        else
          rx_state->cntUnreliableGrossBits=0;

      /* Channel decoder */
      viterbi_exec(fecGrossBits, 1*CHC_RATE, 
                   &fecNetBit, &numViterbiOutBits,
                   &(rx_state->viterbi_state));
      if (numViterbiOutBits > 0)
        {
          Shortint_fifo_push(&(rx_state->net_bits_fifo_state), &fecNetBit, 1);
        }
    }	
  octetAvailable = false;
  
  /* As long as there are bits on the fifo: pop them and decode them into */
  /* octets until a valid octet (i.e. not an idle symbol) is received     */
  while (Shortint_fifo_check(&(rx_state->net_bits_fifo_state))>=BITS_PER_SYMB)
    {
      Shortint_fifo_pop(&(rx_state->net_bits_fifo_state),
                        ucsBitsIn, BITS_PER_SYMB);
      
      utfOctet = 0;
      octetAvailable = true;
      for (cnt=0; cnt<BITS_PER_SYMB; cnt++)
        {
          if (ucsBitsIn[cnt]>0)
            utfOctet += (1<<cnt);
        }
      // fprintf(stderr, " ((%d)) ", utfOctet);

      /* Decide, whether received octet is an idle symbol */
      if (utfOctet==IDLE_SYMB)
        {
          octetAvailable = false;
          rx_state->cntIdleSymbols++;
        }
      
      /* If more than MAX_IDLE_SYMB have been received or if more than */
      /* MAX_NUM_UNRELIABLE_GROSS_BITS gross bits with low reliability */
      /* have been received, assume that synchronization is lost and   */
      /* reset the wait_for_sync function.                             */
      if ((rx_state->cntIdleSymbols>= MAX_IDLE_SYMB) ||
          (rx_state->cntUnreliableGrossBits>MAX_NUM_UNRELIABLE_GROSS_BITS))
        {
          reinit_deinterleaver(&(rx_state->deintl_state));
          viterbi_reinit(&(rx_state->viterbi_state));
          rx_state->cntIdleSymbols = 0;
          rx_state->numDeintlBits = 0;
          
          /* pop all remaining bits from fifos and forget the bits */
          Shortint_fifo_reset(&(rx_state->net_bits_fifo_state));
          Shortint_fifo_reset(&(rx_state->rx_bits_fifo_state));
          
          octetAvailable = false;
        }
      
      /* If octet available -> push it into octet fifo buffer */
      if (octetAvailable)
        {
          Shortint_fifo_push(&(rx_state->octet_fifo_state), (WORD16 *)&utfOctet, 1);
          rx_state->cntIdleSymbols=0; /* reset counter for idle symbols */
        }
      
      /* Try to convert octets from buffer into UCS code.        */
      /* If successful, push decoded UCS code into output buffer */
      if (transformUTF2UCS(&ucsCode, &(rx_state->octet_fifo_state)))
        Shortint_fifo_push(ptr_output_char_fifo_state,(WORD16 *) &ucsCode, 1);
    }


}
#endif
  /***************************************************************************/
  /* ctm_receiver()                                                          */
  /* **************                                                          */
  /* Runs the Cellular Text Telephone Modem Receiver for a block of          */
  /* (nominally) 160 samples. Due to the internal synchronization, the       */
  /* number of processed samples might vary between 156 and 164 samples.     */
  /* The input of the samples and the output of the decoded characters       */
  /* is handled via fifo buffers, which have to be initialized               */
  /* externally before using this function (see fifo.h for details).         */
  /*                                                                         */
  /* input/output variables:                                                 */
  /* *ptr_signal_fifo_state      fifo state for the input samples            */
  /* *ptr_output_char_fifo_state fifo state for the output characters        */
  /* *ptr_early_muting_required  returns whether the original audio signal   */
  /*                             must not be forwarded. This is to guarantee */
  /*                             that the preamble or resync sequence is     */
  /*                             detected only by the first CTM device, if   */
  /*                             several CTM devices are cascaded            */
  /*                             subsequently.                               */
  /* *rx_state                   pointer to the variable containing the      */
  /*                             receiver states                             */
  /***************************************************************************/

  void ctm_receiver(fifo_state_t*  ptr_input_fifo_state,
                    fifo_state_t*  ptr_output_char_fifo_state,
                    BOOL*          ptr_early_muting_required,
                    rx_state_t*    rx_state)
  {
    WORD16  toneVec[SYMB_LEN+1];
    WORD16  bitsDemod[2];
    WORD16  cnt;
    WORD16  numValidBits;
    BOOL    actual_sync_found;
    BOOL    octetAvailable;
#if(TTY_SYNC_MCU_2==1)
    BOOL    sync_after_resync;
    WORD16  num_flushed_bits;
#if(OPTIMISED==1)
  static WORD16 deint_flush_vect[520];
#else
    WORD16 deint_flush_vect[520];
#endif
#endif
    WORD16  fecNetBit;
    UWORD16 utfOctet = 0;
    WORD16  ShortintValueTmp;
    WORD16  syncOffset;
    WORD16  resyncDetected;
    WORD16  wait_interval;
    WORD16  numViterbiOutBits;
    #if(OPTIMISED==1)
    static WORD16 counter_bitween_bursts=-1;
    #endif
  #if(NEW_WKA_PATCH==1)
  WORD16 dim_2[2];
  #endif
    UWORD16 ucsCode  = 0;

    static  WORD16  ucsBits[BITS_PER_SYMB];
    static  WORD16  fecGrossBitsIn[CHC_RATE];
  #if(NEW_WKA_PATCH==1)
  static WORD16 zero_counter=WORD16_MAX;
  static UWORD16 THRESHOLD_FOR_NOISE =0;
  #endif

  /* Beginning of code being replaced. Laura Ning 02/21/2002 */
  //  while (Shortint_fifo_check(ptr_signal_fifo_state)>SYMB_LEN)
  //    {
        /* Pop SYMB_LEN-1, SYMB_LEN, or SYMB_LEN+1 samples from fifo, */
        /* depending on the state of samplingCorrection.              */
  //      Shortint_fifo_pop(ptr_signal_fifo_state, toneVec, 
  //                        SYMB_LEN+rx_state->samplingCorrection);
        
        /* Run the tone demodulator */
  //      tonedemod(bitsDemod, toneVec, 
  //                (WORD16)(SYMB_LEN+rx_state->samplingCorrection), 
  //                &(rx_state->samplingCorrection), 
  //                &(rx_state->tonedemod_state));
        
  //#ifdef DEBUG_OUTPUT
  //      if (fwrite(bitsDemod, sizeof(WORD16), 2, rx_bits_file) == 0)
  //        {
  //          fprintf(stderr, "Error while writing to file\n\n") ;
  //          exit(1);
  //        }
  //#endif

  /* End of code being replaced */

  /* Beginning of code to replace. Laura Ning 02/21/2002 */
      while (ptr_input_fifo_state->num_entries_actual >=2)  
      {
    
          Shortint_fifo_pop(ptr_input_fifo_state, bitsDemod, 2);
  /* End of code to replace */

          /* Find the synchronization sequence and run the */
          /* deinterleaver on the synchronized bitstream   */
          actual_sync_found = 
#if(TTY_SYNC_MCU_2==0)
            wait_for_sync(rx_state->waitSyncOut, 
                          bitsDemod, 2, 
                          rx_state->cntIdleSymbols, 
                          &numValidBits, 
                          &wait_interval, 
                          &resyncDetected, 
                          ptr_early_muting_required,
                          &(rx_state->wait_state));
#else
            wait_for_sync(rx_state->waitSyncOut, 
                          bitsDemod, 2, 
                          rx_state->cntIdleSymbols, 
                          &numValidBits, 
                          &wait_interval, 
                          &resyncDetected, 
                          ptr_early_muting_required,
                      &(rx_state->wait_state),&sync_after_resync);

#endif
#if(NEW_WKA_PATCH==1)    
if((zero_counter==EARLY_SILENCE_PERIOD && (abs(bitsDemod[0])>=THRESHOLD_FOR_NOISE || abs(bitsDemod[1])>=THRESHOLD_FOR_NOISE)) || (zero_counter==LATE_SILENCE_PERIOD && (abs(bitsDemod[0])>=THRESHOLD_FOR_NOISE || abs(bitsDemod[1])>=THRESHOLD_FOR_NOISE)))
{
	zero_counter=zero_counter-SILENCE_PERIOD;
	if(zero_counter>0)
		{
		numValidBits=0;
		just_shift(zero_counter,rx_state->waitSyncOut,&(rx_state->deintl_state));
              }
	else
		{
		dim_2[0]=0;
		dim_2[1]=0;
		just_shift(zero_counter,dim_2,&(rx_state->deintl_state));
		numValidBits+=-zero_counter;
	       }
	zero_counter=WORD16_MAX;
}
else if(zero_counter==SILENCE_PERIOD && (abs(bitsDemod[0])> 0 && abs(bitsDemod[1])> 0))
   {
      zero_counter=WORD16_MAX;
   }
else if(zero_counter!=WORD16_MAX)
{
   if(abs(bitsDemod[0])<THRESHOLD_FOR_NOISE || zero_counter<SILENCE_PERIOD/2)
        	zero_counter++;
   if(abs(bitsDemod[1])<THRESHOLD_FOR_NOISE || zero_counter<SILENCE_PERIOD/2)
	       zero_counter++;
}
#endif

 #if(OPTIMISED==1)
    if(counter_bitween_bursts>=RESYNC_OFFSET+intlvB)
    	{
    	   viterbi_reinit(&(rx_state->viterbi_state));
          rx_state->cntIdleSymbols = 0;
          rx_state->numDeintlBits = counter_bitween_bursts;
          rx_state->cntRXBits = 0;
          rx_state->syncCorrect = 0;
          counter_bitween_bursts=-1;
          /* Pop all remaining bits from fifos and forget the bits */
          Shortint_fifo_reset(&(rx_state->net_bits_fifo_state));
          Shortint_fifo_reset(&(rx_state->rx_bits_fifo_state));
    	}
 #endif
    
          if (actual_sync_found)
          {
              resyncDetected = -1;
#if(TTY_SYNC_MCU_2==1)
          if(sync_after_resync==true)
          	{
          	   num_flushed_bits=flush_diag_deint(deint_flush_vect,
                        rx_state->waitSyncOut,
                        &(rx_state->deintl_state));
          	   #if(OPTIMISED==1)
          	   counter_bitween_bursts=0;
          	   #else
          	   decode_flushed_bits(deint_flush_vect, rx_state,num_flushed_bits,ptr_output_char_fifo_state);
          	   #endif
          }
#endif
              reinit_deinterleaver(&(rx_state->deintl_state));
#if(TTY_SYNC_MCU_2==1)
          zero_pad_deint(&(rx_state->deintl_state));
#endif
          #if(OPTIMISED==1)
          if(counter_bitween_bursts==-1)
          {
              viterbi_reinit(&(rx_state->viterbi_state));
              rx_state->cntIdleSymbols = 0;
              rx_state->numDeintlBits = 0;
              rx_state->cntRXBits = 0;
              rx_state->syncCorrect = 0;
            
           /* Pop all remaining bits from fifos and forget the bits */
              Shortint_fifo_reset(&(rx_state->net_bits_fifo_state));
              Shortint_fifo_reset(&(rx_state->rx_bits_fifo_state));
      	   }
          #else
          viterbi_reinit(&(rx_state->viterbi_state));
          rx_state->cntIdleSymbols = 0;
          rx_state->numDeintlBits = 0;
          rx_state->cntRXBits = 0;
          rx_state->syncCorrect = 0;

          /* Pop all remaining bits from fifos and forget the bits */
          Shortint_fifo_reset(&(rx_state->net_bits_fifo_state));
          Shortint_fifo_reset(&(rx_state->rx_bits_fifo_state));
      	   #endif
           /* Calculate the deinterleaver's delay of                      */
           /* intlvB*deintSyncLns+intlvB*(intlvB-1)*intlvD elements       */
           /* plus an additional delay of RESYMC_SEQ_LENGTH, if the       */
           /* synchronization was triggered by detecting resync sequence  */
           /* and not by detecting the preamble.                          */
              rx_state->intl_delay 
            = intlvB*deintSyncLns+intlvB*(intlvB-1)*intlvD+wait_interval;
  #if(NEW_WKA_PATCH==1)
      zero_counter=0;
      THRESHOLD_FOR_NOISE=((abs(bitsDemod[0])+abs(bitsDemod[1]))/2)/5;
  #endif
          }

          /**************************************************************/
          /*       The following lines are for resynchronisation        */
          /*         (also the deinterleaver is executed here)          */
          /**************************************************************/
        
          if (resyncDetected>=0)
          {
    #if(OPTIMISED==1)
      if(counter_bitween_bursts>-1 && resyncDetected>=0)
      {
          rx_state->numDeintlBits = counter_bitween_bursts;
          counter_bitween_bursts=-1;
          viterbi_reinit(&(rx_state->viterbi_state));
          rx_state->cntIdleSymbols = 0;
          rx_state->cntRXBits = 0;
          rx_state->syncCorrect = 0;
          /* Pop all remaining bits from fifos and forget the bits */
          Shortint_fifo_reset(&(rx_state->net_bits_fifo_state));
          Shortint_fifo_reset(&(rx_state->rx_bits_fifo_state));
      }
    #endif
          #if(NEW_WKA_PATCH==1)
          zero_counter=0;
          THRESHOLD_FOR_NOISE=((abs(bitsDemod[0])+abs(bitsDemod[1]))/2)/5;
          #endif
              syncOffset 
                = (rx_state->numDeintlBits + resyncDetected + 1
                 - rx_state->intl_delay)
                 % (NUM_BITS_BETWEEN_RESYNC+RESYNC_SEQ_LENGTH);
            
            if (syncOffset > 
               (NUM_BITS_BETWEEN_RESYNC+RESYNC_SEQ_LENGTH)/2)
                syncOffset = syncOffset - 
                             (NUM_BITS_BETWEEN_RESYNC+RESYNC_SEQ_LENGTH);
            
            if ((syncOffset>-16) && (syncOffset<16))
                 rx_state->syncCorrect = syncOffset;
            else 
                 rx_state->syncCorrect = 0;
            
          }
   #if(OPTIMISED==1)
          if (rx_state->syncCorrect==0)
          {
           if(counter_bitween_bursts>=0)
        	{
        	  new_diag_deinterleaver(rx_state->deintlOut,counter_bitween_bursts,deint_flush_vect, rx_state->waitSyncOut,numValidBits,&(rx_state->deintl_state));
                counter_bitween_bursts+=numValidBits;
        	}
            else
              /* The synchronisation is still ok. --> no adaptation */
              diag_deinterleaver(rx_state->deintlOut, 
                                 rx_state->waitSyncOut,
                                 numValidBits, 
                                 &(rx_state->deintl_state));
          }
          else if (rx_state->syncCorrect>0)
          {
              /* The ResyncSecquence was too late */
              /* --> some bits have to be dropped */
              if (rx_state->syncCorrect>=numValidBits)
              {
                /* The incoming bits are only inserted into the deinterleaver, */
                /* no bits are handled to the following functions              */
#if (TTY_SYNC_MCU == 0)
                  shift_deinterleaver((WORD16)(-numValidBits), 
                                      rx_state->waitSyncOut,
                                      &(rx_state->deintl_state));
#else
                 flush_and_shift_deinterleaver((WORD16)(-numValidBits), 
                                      rx_state->waitSyncOut,
                                      &(rx_state->deintl_state));
#endif
                  rx_state->syncCorrect -= numValidBits;
                  numValidBits=0;
              }
              else
              {
#if (TTY_SYNC_MCU == 0)
                  shift_deinterleaver((WORD16)(-rx_state->syncCorrect),
                                      rx_state->waitSyncOut,
                                      &(rx_state->deintl_state));
#else
                  flush_and_shift_deinterleaver((WORD16)(-rx_state->syncCorrect),
                                      rx_state->waitSyncOut,
                                      &(rx_state->deintl_state));
#endif                
                
                  numValidBits -= rx_state->syncCorrect;
              if(counter_bitween_bursts>=0)
        	{
        	  new_diag_deinterleaver(rx_state->deintlOut,counter_bitween_bursts,deint_flush_vect, rx_state->waitSyncOut,numValidBits,&(rx_state->deintl_state));
                counter_bitween_bursts+=numValidBits;
        	}
              else
                  diag_deinterleaver(rx_state->deintlOut, 
                                     &(rx_state->waitSyncOut[rx_state->syncCorrect]),
                                     numValidBits, 
                                     &(rx_state->deintl_state));
                  rx_state->syncCorrect=0;
              }
          }
          else
          {
            /* The ResyncSequence was too early        */
            /* --> additional Bits have to be inserted */
              ShortintValueTmp=0;
              for (cnt=0; cnt<-(rx_state->syncCorrect); cnt++)
              {
              if(counter_bitween_bursts>=0)
            	{
        	  new_diag_deinterleaver(rx_state->deintlOut,counter_bitween_bursts,deint_flush_vect, &ShortintValueTmp,1,&(rx_state->deintl_state));
            	  counter_bitween_bursts++;
            	}
              else
                  diag_deinterleaver(&(rx_state->deintlOut[cnt]), 
                                     &ShortintValueTmp, 1,
                                     &(rx_state->deintl_state));
                 #if(TTY_SYNC_MCU == 0)
                  shift_deinterleaver(1, &ShortintValueTmp,
                                      &(rx_state->deintl_state));
                 #else
                 flush_and_shift_deinterleaver(1, &ShortintValueTmp,
                                      &(rx_state->deintl_state));
                 #endif

              }
              if(counter_bitween_bursts>=0)
        	{
        	  new_diag_deinterleaver(rx_state->deintlOut,counter_bitween_bursts,deint_flush_vect, rx_state->waitSyncOut,numValidBits,&(rx_state->deintl_state));
                counter_bitween_bursts+=numValidBits;
              }
              else
              diag_deinterleaver(&(rx_state->deintlOut[-(rx_state->syncCorrect)]), 
                                 rx_state->waitSyncOut,
                                 numValidBits, 
                                 &(rx_state->deintl_state));
              numValidBits += -(rx_state->syncCorrect);
              rx_state->syncCorrect=0;
          }
#else
          if (rx_state->syncCorrect==0)
          {
              /* The synchronisation is still ok. --> no adaptation */
              diag_deinterleaver(rx_state->deintlOut, 
                                 rx_state->waitSyncOut,
                                 numValidBits, 
                                 &(rx_state->deintl_state));
          }
          else if (rx_state->syncCorrect>0)
          {
              /* The ResyncSecquence was too late */
              /* --> some bits have to be dropped */
              if (rx_state->syncCorrect>=numValidBits)
              {
                /* The incoming bits are only inserted into the deinterleaver, */
                /* no bits are handled to the following functions              */
#if (TTY_SYNC_MCU == 0)
                  shift_deinterleaver((WORD16)(-numValidBits), 
                                      rx_state->waitSyncOut,
                                      &(rx_state->deintl_state));
#else
                 flush_and_shift_deinterleaver((WORD16)(-numValidBits), 
                                      rx_state->waitSyncOut,
                                      &(rx_state->deintl_state));
#endif
                  rx_state->syncCorrect -= numValidBits;
                  numValidBits=0;
              }
              else
              {
#if (TTY_SYNC_MCU == 0)
 shift_deinterleaver((WORD16)(-rx_state->syncCorrect),
                                      rx_state->waitSyncOut,
                                      &(rx_state->deintl_state));
#else
                  flush_and_shift_deinterleaver((WORD16)(-rx_state->syncCorrect),
                                      rx_state->waitSyncOut,
                                      &(rx_state->deintl_state));
#endif                
        
                  numValidBits -= rx_state->syncCorrect;
                  diag_deinterleaver(rx_state->deintlOut, 
                                     &(rx_state->waitSyncOut[rx_state->syncCorrect]),
                                     numValidBits, 
                                     &(rx_state->deintl_state));
                  rx_state->syncCorrect=0;
              }
          }
          else
          {
            /* The ResyncSequence was too early        */
            /* --> additional Bits have to be inserted */
              ShortintValueTmp=0;
              for (cnt=0; cnt<-(rx_state->syncCorrect); cnt++)
              {
                  diag_deinterleaver(&(rx_state->deintlOut[cnt]), 
                                     &ShortintValueTmp, 1,
                                     &(rx_state->deintl_state));

#if(TTY_SYNC_MCU == 0)
                  shift_deinterleaver(1, &ShortintValueTmp,
                                      &(rx_state->deintl_state));

#else
                  flush_and_shift_deinterleaver(1, &ShortintValueTmp,

                                      &(rx_state->deintl_state));

#endif

              }
              diag_deinterleaver(&(rx_state->deintlOut[-(rx_state->syncCorrect)]), 
                                 rx_state->waitSyncOut,
                                 numValidBits, 
                                 &(rx_state->deintl_state));
              numValidBits += -(rx_state->syncCorrect);
              rx_state->syncCorrect=0;
          }
    #endif    
          /**************************************************************/
          /*               End of resynchronisation                     */
          /**************************************************************/
        
          /* Consider the deinterleaver's delay                         */
          /* and push the demodulated bits into the fifo buffer         */
          for (cnt=0; cnt<numValidBits; cnt++)
          {
#if (TTY_SYNC_MCU == 0)
              if (rx_state->numDeintlBits >= rx_state->intl_delay)
#else
              if (rx_state->numDeintlBits >= rx_state->intl_delay+NUM_BITS_BETWEEN_RESYNC+RESYNC_SEQ_LENGTH)
#endif
              {
                  /* Ignore all bits that are for muting or resync */
                  if (!(mutingRequired((WORD16)(rx_state->cntRXBits), 
                                       rx_state->mutePositions, 
                                       NUM_MUTE_ROWS*intlvB))
                                    && (rx_state->cntRXBits<NUM_BITS_BETWEEN_RESYNC))
                  {
                      Shortint_fifo_push(&(rx_state->rx_bits_fifo_state), 
                                         &(rx_state->deintlOut[cnt]), 1);
                  }
                
                  rx_state->cntRXBits++;
                  if(rx_state->cntRXBits
                     ==NUM_BITS_BETWEEN_RESYNC+RESYNC_SEQ_LENGTH)
                      rx_state->cntRXBits = 0;
              }
              rx_state->numDeintlBits++;

            /* Avoid Overflows of numDeintlBits */
              if (rx_state->numDeintlBits > 10000)
                  rx_state->numDeintlBits 
                      -= (NUM_BITS_BETWEEN_RESYNC+RESYNC_SEQ_LENGTH);
          } 
      }
    
    /* As long as there are gross bits in the fifo: pop them, run  */
    /* the channel decoder and pop the net bits into the next fifo */
      while ((rx_state->rx_bits_fifo_state).num_entries_actual > CHC_RATE)
      {
          Shortint_fifo_pop(&(rx_state->rx_bits_fifo_state), 
                            fecGrossBitsIn, 
                            CHC_RATE);
        
          /* Count gross bits with low reliability (i.e. bits with too low */
          /* magnitute or with their LSB not set).                         */
          for (cnt=0;cnt<CHC_RATE; cnt++)
              if ((abs(fecGrossBitsIn[cnt])<THRESHOLD_RELIABILITY_FOR_GOING_OFFLINE)
                   || ((fecGrossBitsIn[cnt] & 0x0001) == 0))
              {
                  if (rx_state->cntUnreliableGrossBits < WORD16_MAX)
                      rx_state->cntUnreliableGrossBits++;
              }
              else
                  rx_state->cntUnreliableGrossBits=0;

              /* Channel decoder */
              viterbi_exec(fecGrossBitsIn, 
                           1*CHC_RATE, 
                           &fecNetBit, 
                           &numViterbiOutBits,
                           &(rx_state->viterbi_state));
              if (numViterbiOutBits > 0)
              {
                  Shortint_fifo_push(&(rx_state->net_bits_fifo_state), 
                                     &fecNetBit, 1);
              }
      }
    
      octetAvailable = false;
    
      /* As long as there are bits on the fifo: pop them and decode them into */
      /* octets until a valid octet (i.e. not an idle symbol) is received     */
      while ((rx_state->net_bits_fifo_state).num_entries_actual>=BITS_PER_SYMB)
      {
          Shortint_fifo_pop(&(rx_state->net_bits_fifo_state),
                            ucsBits, 
                            BITS_PER_SYMB);
        
          utfOctet = 0;
          octetAvailable = true;
          for (cnt=0; cnt<BITS_PER_SYMB; cnt++)
          {
              if (ucsBits[cnt]>0)
                  utfOctet += (1<<cnt);
          }


          /* Decide, whether received octet is an idle symbol */
          if (utfOctet==IDLE_SYMB)
          {
              octetAvailable = false;
              rx_state->cntIdleSymbols++;
          }
        
          /* If more than MAX_IDLE_SYMB have been received or if more than */
          /* MAX_NUM_UNRELIABLE_GROSS_BITS gross bits with low reliability */
          /* have been received, assume that synchronization is lost and   */
          /* reset the wait_for_sync function.                             */
          if ((rx_state->cntIdleSymbols>= MAX_IDLE_SYMB) ||
              (rx_state->cntUnreliableGrossBits>MAX_NUM_UNRELIABLE_GROSS_BITS))
          {
        #if(OPTIMISED==1)
        if(counter_bitween_bursts==-1)
        	{
              reinit_wait_for_sync(&(rx_state->wait_state));
              reinit_deinterleaver(&(rx_state->deintl_state));
        	}
        #else
          reinit_wait_for_sync(&(rx_state->wait_state));
        #endif
              viterbi_reinit(&(rx_state->viterbi_state));
              rx_state->cntIdleSymbols = 0;
              rx_state->numDeintlBits  = 0;
            
              /* pop all remaining bits from fifos and forget the bits */
              Shortint_fifo_reset(&(rx_state->net_bits_fifo_state));
              Shortint_fifo_reset(&(rx_state->rx_bits_fifo_state));
            
              octetAvailable = false;
          }
        
          /* If octet available -> push it into octet fifo buffer */
          if (octetAvailable)
          {
              Shortint_fifo_push(&(rx_state->octet_fifo_state), 
                                  (WORD16 *)&utfOctet, 1);
          rx_state->cntIdleSymbols=0; 
              /* reset counter for idle symbols */
          }
        
          /* Try to convert octets from buffer into UCS code.        */
          /* If successful, push decoded UCS code into output buffer */
          if (transformUTF2UCS(&ucsCode, &(rx_state->octet_fifo_state)))
          {
              Shortint_fifo_push(ptr_output_char_fifo_state, 
                                (WORD16 *)&ucsCode, 1);
          }
      }
  }

#endif // L1_GTT

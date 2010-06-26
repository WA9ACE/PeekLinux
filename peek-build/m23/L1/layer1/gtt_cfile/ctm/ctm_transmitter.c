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
*      File             : ctm_transmitter.c
*      Author           : EEDN/RV Matthias Doerbecker
*      Tested Platforms : Sun Solaris, MS Windows NT 4.0
*      Description      : Complete CTM Transmitter including Modulator, 
*                         Synchronisation, Interleaver, and Error Protetion
*
*      Changes since October 13, 2000:
*      - added reset function reset_ctm_transmitter()
*      - the peripheral functions for calling tonemod() have been adjusted
*        in order to support the new release of tonemod. The transmitter
*        can now be executed sample-by-sample or in frames of 160 samples.
*
*      $Log: $
*
*******************************************************************************
*/

/*
*******************************************************************************
*                         MODULE INCLUDE FILE AND VERSION ID
*******************************************************************************
*/

#include "l1_macro.h"
#include "l1_confg.h"

#if (L1_GTT == 1)

  #include "l1_types.h"
  #include "ctm_transmitter.h"

  /*
  ******************************************************************************
  *                         INCLUDE FILES
  *******************************************************************************
  */
  #include <stdlib.h> /* For exit() function */
  #include "ctm_defines.h"
  #include "init_interleaver.h"
  #include "diag_interleaver.h"
  #include "tonemod.h"
  #include "m_sequence.h"
  #include "wait_for_sync.h"     /* generate_resync_sequence() */
  #include "conv_poly.h"
  #include "conv_encoder.h"
  #include "ucs_functions.h"
  #include "fifo.h"
  #include  <stdio.h>
  #include "ctm_typedefs.h"

  #if (TRACE_TYPE == 1) || (TRACE_TYPE == 4) || (TRACE_TYPE == 5)
    #include "rvt_gen.h"
    #include "l1_trace.h"
  #endif

  /* Calculate the required size of the the interleaver's output buffer. */
  /* The maximum number of output elements might occur either during     */
  /* the flushing of the interleaver or during regular operation.        */
  #define INTL_FLUSH_LEN    intlvB*((intlvB-1)*intlvD+demodSyncLns+deintSyncLns)
  #define INTL_REGULAR_LEN  BITS_PER_SYMB*CHC_RATE+(CHC_K-1)*CHC_RATE+NUM_MUTE_ROWS*intlvB

  #if INTL_FLUSH_LEN > INTL_REGULAR_LEN
  #define INTL_OUT_BUF_LEN  INTL_FLUSH_LEN
  #else
  #define INTL_OUT_BUF_LEN  INTL_REGULAR_LEN
  #endif
  BOOL ctm_fifo2_flag = FALSE;

  /***********************************************************************/
  /* init_ctm_transmitter()                                              */
  /* **********************                                              */
  /* Initialization of the Cellular Text Telephone Modem transmitter     */
  /*                                                                     */
  /* output vaiables:                                                    */
  /* tx_state :   pointer to a variable of tx_state_t containing the     */
  /*              initialized states of the receiver                     */
  /***********************************************************************/

  void init_ctm_transmitter(tx_state_t* tx_state)
  {
    tx_state->burstActive              = false;
    tx_state->cntIdleSymbols           = 0;
    tx_state->cntEncBitsInCurrentBlock = 0;
    tx_state->cntTXBitsInCurrentBlock  = 0;


    Shortint_fifo_init(&(tx_state->fifo_state), 400);
    Shortint_fifo_init(&(tx_state->octet_fifo_state), 5);



    /* check whether values for resync are multiples of intlvB */


    if (((NUM_BITS_BETWEEN_RESYNC%intlvB)!=0) &&
        ((RESYNC_SEQ_LENGTH%intlvB)!=0))
      {
  #if (TRACE_TYPE == 1) || (TRACE_TYPE == 4) || (TRACE_TYPE == 5)
        if (trace_info.current_config->l1_dyn_trace & 1<<L1_DYN_TRACE_GTT)
        {
          char str[200];

          sprintf(str,
                  "Error: (NUM_BITS_BETWEEN_RESYNC=%d) and (RESYNC_SEQ_LENGTH=%d) must be a multiple of (intlvB=%d)\n\r",
                  NUM_BITS_BETWEEN_RESYNC, 
                  RESYNC_SEQ_LENGTH,
                  intlvB);

          GTT_send_trace_cpy(str);
        }
  #endif    /* End trace  */
        //exit(1);  Should never come here !!
      }


    /* Initialization of the interleaver */

     init_interleaver(&(tx_state->diag_int_state),
                     intlvB, intlvD, demodSyncLns, deintSyncLns);

    /* Initialization of the convolutional encoder */
    conv_encoder_init(&(tx_state->conv_enc_state));

    /* Calculation of the resynchronization sequence */
    generate_resync_sequence(tx_state->resync_sequence);
    
    calc_mute_positions(tx_state->mutePositions, NUM_MUTE_ROWS, intlvB-1,
                        intlvB, intlvD);

  }

  /*
   ***********************************************************************
   * Shutdown_ctm_transmitter()                                          *
   * **********************                                              *
   * Shutdown of the Cellular Text Telephone Modem transmitter and       *
   * deallocate memory used by the CTM Tx buffers.                       *
   *                                                                     *
   * Input variables:                                                    *
   * tx_state :   pointer to a variable of tx_state_t containing the     *
   *              initialized states of the transmitter.                 *
   ***********************************************************************
  */
  void Shutdown_ctm_transmitter(tx_state_t* tx_state)
  {
      Shortint_fifo_exit(&(tx_state->fifo_state));
      Shortint_fifo_exit(&(tx_state->octet_fifo_state));
      Shutdown_interleaver(&(tx_state->diag_int_state));
  }

  /*
   ***********************************************************************
   * reset_ctm_transmitter()                                             *
   * **********************                                              *
   * Reset of the Cellular Text Telephone Modem transmitter, state       *
   * machines and buffer pointers.                                       *
   *                                                                     *
   * Input variables:                                                    *
   * tx_state :   pointer to a variable of tx_state_t containing the     *
   *              initialized states of the transmitter.                 *
   ***********************************************************************
  */
  void reset_ctm_transmitter(tx_state_t* tx_state)
  {
    tx_state->burstActive              = false;
    tx_state->cntIdleSymbols           = 0;
    tx_state->cntEncBitsInCurrentBlock = 0;
    tx_state->cntTXBitsInCurrentBlock  = 0;
    
    Shortint_fifo_reset(&(tx_state->fifo_state));
    Shortint_fifo_reset(&(tx_state->octet_fifo_state));

    /* Initialization of the interleaver */
    reinit_interleaver(&(tx_state->diag_int_state));

    /* Initialization of the convolutional encoder */
    conv_encoder_reset(&(tx_state->conv_enc_state));
  }

  /***********************************************************************/
  /* ctm_transmitter()                                                   */
  /* *****************                                                   */
  /* Runs the Cellular Text Telephone Modem (CTM) Transmitter for a      */
  /* block of 160 output samples, representing 8 gross bits.             */
  /* The bits, which are modulated into tones, are taken from an         */
  /* internal fifo buffer. If the fifo buffer is empty, zero-valued      */
  /* samples are generated. The fifo buffer is filled with               */
  /* channel-encoded and interleaved bits, which are generated           */
  /* internally by coding the actual input character.  With each call of */
  /* this function one or less input characters can be coded. If there   */
  /* is no character to for transmission, one of the following code has  */
  /* be used:                                                            */
  /* - 0x0016:  <IDLE>, indicates that there is no character to transmit */
  /*            and that the transmitter should stay in idle mode, if it */
  /*            is currently already in idle mode. If the transmitter is */
  /*            NOT in idle mode, it might generate <IDLE> symbols in    */
  /*            to keep an active burst running. The CTM burst is        */
  /*            terminated if five <IDLE> symbols have been generated    */
  /*            consecutively.                                           */
  /*                                                                     */
  /* - 0xFFFF:  although there is no character to transmit, a CTM burst  */
  /*            is initiated in order to signalize to the far-end side   */
  /*            that CTM is supported. The burst starts with the <IDLE>  */
  /*            symbol and will be contiued with <IDLE> symbols if there */
  /*            are no regular characters handed over during the next    */
  /*            calls of this function. The CTM burst is terminated if   */
  /*            five <IDLE> symbols have been transmitted consecutively. */
  /*                                                                     */
  /* In order to avoid an overflow of the internal fifo buffer, the      */
  /* variable *ptrNumBitsStillToModulate should be checked before        */
  /* calling this function.                                              */
  /*                                                                     */
  /* input variables:                                                    */
  /* ucsCode                    UCS code of the character                */
  /*                            or one of the codes 0x0016 or 0xFFFF     */
  /* sineOutput                 must be false in regular mode; if true,  */
  /*                            a pure sine output signal is generated   */
  /* output variables:                                                   */
  /* txToneVec                  output signal (vector  of 160 samples)   */
  /* input/output variables:                                             */
  /* tx_state                   pointer to the variable containing the   */
  /*                            transmitter states                       */
  /***********************************************************************/

  void ctm_transmitter(UWORD16  ucsCode, 
                       fifo_state_t*  ptrctmCodeFifoState,
                       tx_state_t* tx_state) 
  {
    WORD16  cnt;
    WORD16  numValidBits;
    WORD16  numInterleavedBits;
    WORD16  numBitsEnc;
    WORD16  cntBitsEnc;
    WORD16  numBitsToModulate;
    
    WORD16  indexOffset;
    WORD16  cntBitsEncMuted;
    WORD16  cntInterleavedBits;
    WORD16  utfOctet;
    WORD16  guardBit = GUARD_BIT_SYMBOL;

    WORD16 netBits[8];
    WORD16 bitsEnc[8*CHC_RATE+(CHC_K-1)*CHC_RATE];
    WORD16 bitsEncMuted[8*CHC_RATE+(CHC_K-1)*CHC_RATE+NUM_MUTE_ROWS*intlvB];
    WORD16 bitsEncIntBuf[INTL_OUT_BUF_LEN];
    WORD16 txBits[LENGTH_TX_BITS];
    
    WORD16 zero_vec[] = {0,0,0,0,0,0,0,0,0,0};

    WORD16 ctmCode;
    /* The folowing vectors are static in order to avoid time-consuming */
    /* reallocation for each function call. The contents of these       */
    /* vectors is not of any importance for the next function call,     */
    /* therefore the use of static variables does not violate the use   */
    /* of this function in multiple instances.                          */
    
    /* Transform ucs Code into a sequence of UTF-8 octets, */
    /* if it is NOT one of the codes 0x0016 or 0XFFFF      */
    if ((ucsCode!=0x0016) && (ucsCode!=0xFFFF))
    {
      transformUCS2UTF(ucsCode, &(tx_state->octet_fifo_state));
    }
    
    /* Convert octet into bit pattern */
    if ((tx_state->octet_fifo_state).num_entries_actual > 0)
    {
        Shortint_fifo_pop(&(tx_state->octet_fifo_state), &utfOctet, 1);
        for (cnt=0; cnt<8; cnt++)
          netBits[cnt] = (utfOctet & (1<<cnt)) ? 1 : 0;
        numValidBits = 8;
    }
    else
      numValidBits = 0;
    
    /* If there are no valid net bits and the fifo buffer is          */
    /* running out, idle symbols have to be generated (the fifo       */
    /* buffer must contain at least LENGTH_TX_BITS bits, because the  */
    /* CTM transmitter is able to transmit LENGTH_TX_BITS gross bits  */
    /* within a frame of LENGTH_TONE_VEC samples)                     */
    
    if (numValidBits>0)
    {
        /* if valid bits available -> reset idle symbol counter */
        tx_state->cntIdleSymbols = 0;
    }
    /* Generate <IDLE> symbols in any of the following cases:     */
    /* - if transmitter is inactive and input UCS code was 0xFFFF */
    /* - if transmitter is active and fifo buffer is running out  */
    else if ( ((ucsCode==0xFFFF) && !(tx_state->burstActive)) || 
              (((tx_state->fifo_state).num_entries_actual<LENGTH_TX_BITS) 
               && tx_state->burstActive))
    {
        for(cnt=0;cnt<BITS_PER_SYMB;cnt++)
          netBits[cnt] = (IDLE_SYMB & (1<<cnt)) ? 1 : 0;
        numValidBits=BITS_PER_SYMB;
        tx_state->cntIdleSymbols++;

  #if (TRACE_TYPE == 1) || (TRACE_TYPE == 4) || (TRACE_TYPE == 5)
        if (trace_info.current_config->l1_dyn_trace & 1<<L1_DYN_TRACE_GTT)
        {
           char str[40];
                  
           sprintf(str,"CTM TX: Idle symbol transmitted.\n\r");
      
           GTT_send_trace_cpy(str);
        }
  #endif /* End trace */
    }
    
    /* If there are valid net bits (either real bits or idle bits),  */
    /* do the channel encoding and the interleaving                  */
    if (numValidBits>0)
    {
        /* If interleaver is in idle mode -> initialize interleaver */
        if (!(tx_state->burstActive))
        {
            reinit_interleaver(&(tx_state->diag_int_state));
            
            /* set flag indicating that the burst is active  */
            /* and that the interleaver memory contains bits */
            tx_state->burstActive = true;
            tx_state->cntEncBitsInCurrentBlock = 0;
            tx_state->cntTXBitsInCurrentBlock  = 0;
        }
        
        /* Execute the convolutional encoder */
        conv_encoder_exec(&(tx_state->conv_enc_state), 
                          netBits, numValidBits, bitsEnc);
        numBitsEnc = numValidBits*CHC_RATE;
        
        /* If all of the last MAX_IDLE_SYMB symbols were idle symbols */
        /* --> flush the convolutional encoder                        */
        if (tx_state->cntIdleSymbols>=MAX_IDLE_SYMB)
        {
            conv_encoder_exec(&(tx_state->conv_enc_state), 
                              zero_vec, CHC_K-1 , &(bitsEnc[numBitsEnc]));
            numBitsEnc += (CHC_K-1)*CHC_RATE;
        }
        
        tx_state->cntEncBitsInCurrentBlock += numBitsEnc;
        
        /* Convert from 1/0 to +1/-1 */
        for (cnt=0; cnt<numBitsEnc; cnt++)
          bitsEnc[cnt] = 2*bitsEnc[cnt]-1;
        
        /* Insertion of "mute-bits" */
        cntBitsEnc = 0;
        cntBitsEncMuted = 0;
        while (cntBitsEnc<numBitsEnc)
        {
            /* Calculate the offset, i.e. the index within the actual */
            /* block or within the next block.                        */
            indexOffset = ((cntBitsEncMuted+tx_state->cntTXBitsInCurrentBlock) 
                           % (NUM_BITS_BETWEEN_RESYNC));
            
            /* If the actual position within the bitstream towards  */
            /* the interleaver refers to a bit that has to be muted */
            /* --> insert a "mute-bit"                              */
            if (mutingRequired(indexOffset, tx_state->mutePositions, 
                               NUM_MUTE_ROWS*intlvB))
            {
                bitsEncMuted[cntBitsEncMuted] = GUARD_BIT_SYMBOL;
                cntBitsEncMuted++;
            }
            else
            {
                bitsEncMuted[cntBitsEncMuted] = bitsEnc[cntBitsEnc];
                cntBitsEnc++;
                cntBitsEncMuted++;
            }
        }

        /* Now the variable cntBitsEncMuted specifies how many bits have to */
        /* be interleaved. This interleaving is made in one or more passes  */
        /* depending on how the number of bits to be interleaved fits in    */
        /* remaining space of the actual block.                             */
        
        cntInterleavedBits = 0;
        while (cntInterleavedBits<cntBitsEncMuted)
        {
            /* Decide whether the number of bits to be interleaved is greater */
            /* or smaller than the remaining space of the actual block.       */
            /* The variable numInterleavedBits is assigned to the number of   */
            /* bits that have to be interleaved during this pass.             */
            if (tx_state->cntTXBitsInCurrentBlock + 
                cntBitsEncMuted - cntInterleavedBits > NUM_BITS_BETWEEN_RESYNC)
              numInterleavedBits 
                = NUM_BITS_BETWEEN_RESYNC - tx_state->cntTXBitsInCurrentBlock;
            else
              numInterleavedBits = cntBitsEncMuted - cntInterleavedBits;
            
            /* Run the interleaver */
            diag_interleaver(bitsEncIntBuf, 
                             &(bitsEncMuted[cntInterleavedBits]), 
                             numInterleavedBits,
                             &(tx_state->diag_int_state));
            
            tx_state->cntTXBitsInCurrentBlock += numInterleavedBits;
            cntInterleavedBits += numInterleavedBits;
            
            /* Push the interleaved bits into the fifo buffer */
            Shortint_fifo_push(&(tx_state->fifo_state), bitsEncIntBuf, 
                               numInterleavedBits);
            
            /* Check whether the actual block is filled completely */
            if (tx_state->cntTXBitsInCurrentBlock == NUM_BITS_BETWEEN_RESYNC)
            {
                /* Block is filled completely -> reset counters */
                tx_state->cntEncBitsInCurrentBlock =0;
                tx_state->cntTXBitsInCurrentBlock =0;
                
                /* Send the resync sequence to the interleaver */
                diag_interleaver(bitsEncIntBuf, tx_state->resync_sequence, 
                                 RESYNC_SEQ_LENGTH, &(tx_state->diag_int_state));
                
                Shortint_fifo_push(&(tx_state->fifo_state), bitsEncIntBuf, 
                                   RESYNC_SEQ_LENGTH);

            }
        }
    }
    
    /* If all of the last MAX_IDLE_SYMB symbols were idle symbols,    */
    /* initiate the end of the transmission and flush the interleaver */
    if (tx_state->cntIdleSymbols>=MAX_IDLE_SYMB)
    {
        diag_interleaver_flush(bitsEncIntBuf, &numInterleavedBits,
                               &(tx_state->diag_int_state));
        tx_state->burstActive = false;  
        
        /* Push the interleaved bits into the fifo buffer */
        Shortint_fifo_push(&(tx_state->fifo_state), bitsEncIntBuf,
                           numInterleavedBits);
        
        /* Append guard bits in order to generate a guard interval */
        /* with silence between two bursts                         */
        for (cnt=0; cnt<NUM_BITS_GUARD_INTERVAL; cnt++)
          Shortint_fifo_push(&(tx_state->fifo_state), &guardBit, 1);
        
        /* Reset the Idle Symbol Counter */
        tx_state->cntIdleSymbols=0;
    }

    /* Check whether the modulator is able to accept new bits */
    
       numBitsToModulate = (tx_state->fifo_state).num_entries_actual;
       if (numBitsToModulate>=LENGTH_TX_BITS)
       {
        /* Copies interleaved bits into FIFO.
         * Checks to make sure there's enough FIFO space. */
         ctm_fifo2_flag = TRUE;
         if(numBitsToModulate> (ptrctmCodeFifoState->length_buffer - 
            ptrctmCodeFifoState->num_entries_actual))
         {
            numBitsToModulate = ptrctmCodeFifoState->length_buffer - 
                                ptrctmCodeFifoState->num_entries_actual;
         }

         for(cnt = 0; cnt < numBitsToModulate; cnt ++)
         {
                Shortint_fifo_pop(&(tx_state->fifo_state), &ctmCode, 1);
                /* Stop if there's error putting data into FIFO */
                if(Shortint_fifo_push(ptrctmCodeFifoState, &ctmCode, 1)!=0)
                {
                   break;
                }
         }
         ctm_fifo2_flag = FALSE;
       }
  }

#endif






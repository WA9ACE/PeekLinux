/*
*****************************************************************************
*                                                                           *
* ========================================================================= *
*                                                                           *
* Copyright (c) 2002                                                        *
* Matsushita Communication Industrial, MMCD USA                             *
*                                                                           *
* ========================================================================= *
*                                                                           *
* File Name:                                                                *
*     l1gtt_baudot_functions.c                                              *
*                                                                           *
* Purpose: It contains the functions used to process the baudot code.       *
*****************************************************************************
*/

/*
*** Maintenance *********************************************************
*                                                                       *
*                                                                       *
*                 02/11/2002   initial version created       Laura Ning *
*                                                                       *
*************************************************************************
*/

#include "l1_confg.h"

#if (L1_GTT == 1)

  #include "l1_types.h"
  #include "fifo.h"
  #include "l1gtt_baudot_functions.h"
  #include <string.h> /* For memcpy calls */
  #include <stdio.h>
  #include "ctm_typedefs.h"
  #include "ctm_transmitter.h"

  #if (TRACE_TYPE == 1) || (TRACE_TYPE == 4) || (TRACE_TYPE == 5)
    #include "rvt_gen.h"
    #include "l1_trace.h"
  #endif

  /*
  *****************************************************************************
  * baudot_encode()                                                           *
  * ****************                                                          *
  * Purpose:                                                                  *
  *     The purpose of this function is to generate the baudot code based on  *
  *     the input extended TTY code information. The baudot code generated    *
  *     contains a START bit and a STOP bit, as well as the 5 data bits. It   *
  *     is ready to be modulated to get the baudot signal, which goes into    *
  *     the TTY device.                                                       *
  *                                                                           *
  * Inputs:                                                                   *
  *     inputTTYcode    -- variable containing the input extended TTY code.   *
  *                                                                           *
  * input/output:                                                             *
  *     ptrOutFifoState   -- Pointer to the state of the output shift         *
  *                          register containing the to-be-modulated baudot   *
  *                          code.                                            *
  *     state             -- Pointer to the state variable of baudot_encode() *
  *                                                                           *
  * Warnings:                                                                 *
  *                                                                           *
  *                                                                           *
  *****************************************************************************
  */
  void baudot_encode(WORD16 inputTTYcode,
                     fifo_state_t* ptrOutFifoState,
                     baudot_encode_state_t* state)
  {
      WORD16   cnt;
      WORD16   cntTxBits=0;
      extern baudot_encode_state_t baudot_encode_state;
    
    /* The following vector is static in order to prevent a reallocation   
     * with each call of this function. The contents of the vector is not  
     * required after leaving this function, therefore the use of static   
     * variables does not prevent multiple instances of this function.     */
  
    static WORD16  TxBitsBuffer[2*(1+BAUDOT_NUM_INFO_BITS+NUM_STOP_BITS_TX)];
  
    /* Check, whether actual character is valid */
      if ((inputTTYcode>=0) && (inputTTYcode<64))
      {
          /* ShiftToLetters/SiftToFigures have to be generated, if the     
           * actual character and the current transmitter mode do not fit. 
           * Additionally, an appropriate Shift symbol is sent at least    
           * once for each interval of 72 characters.                      */
      
          if (((inputTTYcode>=32) && !(state->inFigureMode)) ||
              ((state->cntCharsSinceLastShift>=72) && (state->inFigureMode)))
          {

    #if (TRACE_TYPE == 1) || (TRACE_TYPE == 4) || (TRACE_TYPE == 5)
             if (trace_info.current_config->l1_dyn_trace & 1<<L1_DYN_TRACE_GTT)
             {
               char str[30];
                  
               sprintf(str,"Baudot enc IN: SHIFT:FIGS\n\r");   

               GTT_send_trace_cpy(str);
             }
    #endif /* End trace */

              /* send BAUDOT_SHIFT_FIGURES */
              TxBitsBuffer[cntTxBits++] = BAUDOT_START_BIT; /* start bit */

              TxBitsBuffer[cntTxBits++] =  1;
              TxBitsBuffer[cntTxBits++] =  1;
              TxBitsBuffer[cntTxBits++] = -1;
              TxBitsBuffer[cntTxBits++] =  1;
              TxBitsBuffer[cntTxBits++] =  1;

              TxBitsBuffer[cntTxBits++] = BAUDOT_STOP_OUTPUT; /* stop bit */
              state->cntCharsSinceLastShift = 0;
              state->inFigureMode           = true;
          }
      
          if (((inputTTYcode<32) && (state->inFigureMode)) ||
              ((state->cntCharsSinceLastShift>=72) && !(state->inFigureMode)))
          {

    #if (TRACE_TYPE == 1) || (TRACE_TYPE == 4) || (TRACE_TYPE == 5)
              if (trace_info.current_config->l1_dyn_trace & 1<<L1_DYN_TRACE_GTT)
              {
                char str[30];
                  
                sprintf(str,"Baudot enc IN: SHIFT:LTRS\n\r");   

                GTT_send_trace_cpy(str);
              }
    #endif /* End trace  */

              /* send BAUDOT_SHIFT_LETTERS */
              TxBitsBuffer[cntTxBits++] = BAUDOT_START_BIT; /* start bit */
              for (cnt=0; cnt<BAUDOT_NUM_INFO_BITS; cnt++)
              {
                  if(((BAUDOT_SHIFT_LETTERS >> cnt) & 1) == 1)
                  {
                      TxBitsBuffer[cntTxBits++] = BAUDOT_LOGICAL_1;
                  }
                  else
                  {
                      TxBitsBuffer[cntTxBits++] = BAUDOT_LOGICAL_0;
                  }
              }
 
              TxBitsBuffer[cntTxBits++] = BAUDOT_STOP_OUTPUT; /* stop bit */
              state->cntCharsSinceLastShift = 0;
              state->inFigureMode           = false;
          }
      
          /* send inputTTYcode */
          TxBitsBuffer[cntTxBits++] = BAUDOT_START_BIT; /* start bit */
          for (cnt=0; cnt<BAUDOT_NUM_INFO_BITS; cnt++)
          {
              if(((inputTTYcode >> cnt) & 1) == 1)
              {
                  TxBitsBuffer[cntTxBits++] = BAUDOT_LOGICAL_1;
              }
              else
              {
                  TxBitsBuffer[cntTxBits++] = BAUDOT_LOGICAL_0;
              }
          }

          TxBitsBuffer[cntTxBits++] = BAUDOT_STOP_OUTPUT; /* stop bit */

          (state->cntCharsSinceLastShift)++;
      
          /* push all TxBits into the fifo buffer */
          Shortint_fifo_push(ptrOutFifoState, 
                             TxBitsBuffer, 
                             cntTxBits);
          state->tailBitsGenerated = false;
      }
      else
      {
          /*
           * This code writes extra bits to DSP task 4.It is removed because 
           * it caused additional (176 ms of 1400 Hz signal) to be generated.
           * This additional signal served no purpose.
          if ((Shortint_fifo_check(ptrOutFifoState)<=1) && 
              !(state->tailBitsGenerated))
          {
              for (cnt=0; cnt<8; cnt++)   
              {
                  TxBitsBuffer[cnt] = 1;
              }
              Shortint_fifo_push(ptrOutFifoState, 
                                 TxBitsBuffer, 
                                 8);
              state->tailBitsGenerated = true;
          }
          */
      }
  }

  /*
  ****************************************************************************
  * init_baudot_encode()                                                     *
  * ****************                                                         *
  * Purpose:                                                                 *
  *     The purpose of this function is to initialize the state variables    *
  *     used in the function baudot_encode(). It should be called at the     *
  *     beginning when baudot encoder is to be executed.                     *
  *                                                                          *
  * Inputs:                                                                  *
  *     N/A                                                                  *
  *                                                                          *
  * input/output:                                                            *
  *     baudot_encode_state     -- Pointer to the state variable of          *
  *                                baudot_encode(), the structure is         *
  *                                defined as baudot_encode_state_t.         *
  *                                                                          *
  * Warnings:                                                                *
  *                                                                          *
  *                                                                          *
  ****************************************************************************
  */
  void init_baudot_encode(baudot_encode_state_t*  baudot_encode_state)
  {
      /* this generates an initial SHIFT */
      baudot_encode_state->cntCharsSinceLastShift = 72;  
      baudot_encode_state->inFigureMode           = false;
      baudot_encode_state->tailBitsGenerated      = true;

  }

  /*
  *****************************************************************************
  * convertChar2ttyCode()                                                     *
  * *********************                                                     *
  * Conversion from character into tty code.                                  *
  *                                                                           *
  * TTY code is similar to Baudot Code, with the exception that bit5 is used  *
  * for signalling whether the actual character is out of the Letters or the  *
  * Figures character set. The remaining bits (bit0...bit4) are the same      *
  * than in Baudot Code.                                                      *
  *                                                                           *
  * input variables:                                                          *
  * - inChar       character that shall be converted                          *
  *                                                                           *
  * return value:  Baudot Code (0..63) of the input character                 *
  *                or -1 in case that inChar is not valid (e.g. inChar=='\0') *
  *                                                                           *
  * Matthias Doerbecker, Ericsson Eurolab Deutschland (EED/N/RV), 2000/02/17  *
  *****************************************************************************
  */
  WORD16 convertChar2ttyCode(char inChar)
  {
    const char ttyCharTab[] = 
     "\bE\nA SIU\rDRJNFCKTZLWHYPQOBG\0MXV\0\b3\n- " 
     "\087\r$4',!:(5\")2=6019\?+\0./;\0";
  
    WORD16  ttyCharCode=-1; 

      if (inChar != '\0')
      {
          /* determine the character's TTY code index */
          ttyCharCode=0; 
          while((inChar!=ttyCharTab[ttyCharCode]) && (ttyCharCode<64))
              ttyCharCode++;
      }
      if (ttyCharCode==64)
          ttyCharCode = -1;

      return ttyCharCode;
  }

  /*
  ****************************************************************************
  * convertTTYcode2char()                                                    *
  * *********************                                                    *
  * Conversion from tty code into character                                  *
  *                                                                          *
  * input variables:                                                         *
  * - ttyCode      Baudot code (must be within the range 0...63)             *
  *                or -1 if there is nothing to convert                      *
  *                                                                          *
  * return value:  character (or '\0' if ttyCode is not valid)               *
  *                                                                          *
  * Matthias Doerbecker, Ericsson Eurolab Deutschland (EED/N/RV), 2000/02/17 *
  ****************************************************************************
  */
  char convertTTYcode2char(WORD16 ttyCode)
  {
    const char ttyCharTab[] = 
     "\bE\nA SIU\rDRJNFCKTZLWHYPQOBG\0MXV\0\b3\n- "
     "\087\r$4',!:(5\")2=6019\?+\0./;\0";

    char outChar = '\0';
  
      if ((ttyCode>=0) && (ttyCode<64))
          outChar = ttyCharTab[ttyCode];
  
      return outChar;
}

#endif // L1_GTT

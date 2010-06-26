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
*      File             : ctm_receiver.h
*      Author           : EEDN/RV Matthias Doerbecker
*      Tested Platforms : Sun Solaris, Windows NT 4.0
*      Description      : header file for ctm_receiver.c
*
*      Changes since October 13, 2000:
*      - added reset function reset_ctm_receiver()
*
*      $Log: $
*
*******************************************************************************
*/
#ifndef ctm_receiver_h
#define ctm_receiver_h "$Id: $"

/*
*******************************************************************************
*                         INCLUDE FILES
*******************************************************************************
*/

#include "init_interleaver.h"
//#include "tonedemod.h"
#include "wait_for_sync.h"
#include "conv_poly.h"
#include "viterbi.h"
#include "ctm_typedefs.h"
#include "fifo.h"

#include <stdlib.h>
#include <stdio.h> 


/* ******************************************************************/
/* Type definitions for variables that contain all states of the    */
/* Cellular Text Telephone Modem (CTM) Transmitter and Receiver,    */
/* respectively.                                                    */
/* ******************************************************************/

typedef struct {
  /* simple variables */
  WORD16    samplingCorrection;
  WORD16    numBitsWithLowReliability;
  WORD16    cntIdleSymbols;
  WORD16    numDeintlBits;
  WORD16    cntRXBits;
  WORD16    syncCorrect;
  WORD16    cntUnreliableGrossBits;
  WORD16    intl_delay;
  
  /* structs (state types) */
  fifo_state_t          rx_bits_fifo_state;
  fifo_state_t          octet_fifo_state;
  fifo_state_t          net_bits_fifo_state;
  //demod_state_t         tonedemod_state;
  // interleaver_state_t   intl_state;
  interleaver_state_t   deintl_state;
  wait_for_sync_state_t wait_state;
  viterbi_t             viterbi_state;
   
  /* vectors (not to be allocated) */
#if (NUM_MUTE_ROWS>0)
  WORD16    mutePositions[intlvB*NUM_MUTE_ROWS];
#else
  WORD16    mutePositions[1];
#endif

  /* vectors (to be allocated in init_ctm_receiver()) */
  WORD16    *waitSyncOut;
  WORD16    *deintlOut;

} rx_state_t;




/***********************************************************************/
/* init_ctm_receiver()                                                 */
/* *******************                                                 */
/* Initialization of the CTM Receiver.                                 */
/*                                                                     */
/* output vaiables:                                                    */
/* rx_state :   pointer to a variable of rx_state_t containing the     */
/*              initialized states of the receiver                     */
/***********************************************************************/

void init_ctm_receiver(rx_state_t* rx_state);

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
void Shutdown_ctm_receiver(rx_state_t* rx_state);

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
void reset_ctm_receiver(rx_state_t* rx_state);


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

void ctm_receiver(fifo_state_t*  ptr_signal_fifo_state,
                  fifo_state_t*  ptr_output_char_fifo_state,
                  BOOL*          ptr_early_muting_required,
                  rx_state_t*    rx_state);

#endif

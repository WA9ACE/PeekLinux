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
*      File             : diag_interleaver.h
*      Purpose          : diagonal (chain) deinterleaver routine
*
*******************************************************************************
*/
#ifndef diag_deinterleaver_h
#define diag_deinterleaver_h "$Id: $"

/*
*******************************************************************************
*                         INCLUDE FILES
*******************************************************************************
*/

#include "init_interleaver.h"

/*
*******************************************************************************
*                         DECLARATION OF PROTOTYPES
*******************************************************************************
*/

/* ---------------------------------------------------------------------- */
/* diag_deinterleaver:                                                    */
/* Corresponding deinterleaver to diag_interleaver.                       */
/* An arbitrary number of bits can be interleaved, depending of the       */
/* length of the vector "in". The vector "out", which must have the same  */
/* length than "in", contains the interleaved samples.                    */
/* All states (memory etc.) of the interleaver are stored in the variable */
/* *intl_state. Therefore, a pointer to this variable must be handled to  */
/* this function. This variable initially has to be initialized by the    */
/* function init_interleaver, which offers also the possibility to        */
/* specify the dimensions of the deinterleaver matrix.                    */
/* ---------------------------------------------------------------------- */

void diag_deinterleaver(WORD16 *out,
                        WORD16 *in,
                        WORD16 num_valid_bits,
                        interleaver_state_t *intl_state);

/* ---------------------------------------------------------------------- */
/* shift_deinterleaver:                                                   */
/* Shift of the deinterleaver buffer by <shift> samples.                  */
/* shift>0  -> shift to the right                                         */
/* shift<0  -> shift to the left                                          */
/* The elements from <insert_bits> are inserted into the resulting space. */
/* The vector <insert_bits> must have at least abs(shift) elements.       */
/* ---------------------------------------------------------------------- */

void shift_deinterleaver(WORD16 shift,
                         WORD16 *insert_bits,
                         interleaver_state_t *ptr_state);


#if(NEW_WKA_PATCH==1)
  void just_shift(WORD16 shift,WORD16* in_bits,interleaver_state_t *intl_state);
#endif

#if(TTY_SYNC_MCU_2==1)
WORD16 flush_diag_deint(WORD16 *out,
                        WORD16 *in,
                        interleaver_state_t *intl_state);
#endif

#if (TTY_SYNC_MCU == 1)
  void flush_and_shift_deinterleaver(WORD16 shift,WORD16 *insert_bits,interleaver_state_t *ptr_state);
#endif

#if(OPTIMISED==1)
  void new_diag_deinterleaver(WORD16 *out,
                              WORD16 jump,
                              WORD16 *flush_vector,
                              WORD16 *in,
                              WORD16 num_valid_bits,
                              interleaver_state_t *intl_state);
#endif

#endif

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
*      Purpose          : diagonal (chain) interleaver routine
*
*******************************************************************************
*/
#ifndef diag_interleaver_h
#define diag_interleaver_h "$Id: $"

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
/* diag_interleaver:                                                      */
/* Diagonal (chain) interleaver, based on block-by-block processing.      */
/* An arbitrary number of bits can be interleaved, depending of the       */
/* value num_bits. The vector "out", which must have the same             */
/* length than "in", contains the interleaved samples.                    */
/* All states (memory etc.) of the interleaver are stored in the variable */
/* *intl_state. Therefore, a pointer to this variable must be handled to  */
/* this function. This variable initially has to be initialized by the    */
/* function init_interleaver, which offers also the possibility to        */
/* specify the dimensions of the interleaver matrix.                      */
/* ---------------------------------------------------------------------- */

void diag_interleaver(WORD16 *out,
                      WORD16 *in,
                      WORD16  num_bits,
                      interleaver_state_t *intl_state);

/* ---------------------------------------------------------------------- */
/* diag_interleaver_flush:                                                */
/* Execution of the diagonal (chain) interleaver without writing in new   */
/* samples. The number of calculated output samples is returned via the   */
/* value *num_bits.                                                       */
/* ---------------------------------------------------------------------- */

void diag_interleaver_flush(WORD16 *out,
                            WORD16 *num_bits,
                            interleaver_state_t *intl_state);

#endif


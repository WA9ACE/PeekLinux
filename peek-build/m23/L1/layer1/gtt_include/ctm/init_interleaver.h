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
*      File             : init_interleaver.h
*      Purpose          : initialization of the diagonal (chain) interleaver;
*                         definition of the type interleaver_state_t
*
*******************************************************************************
*/
#ifndef init_interleaver_h
#define init_interleaver_h "$Id: $"

/*
*******************************************************************************
*                         INCLUDE FILES
*******************************************************************************
*/

#include "ctm_typedefs.h"

/*
*******************************************************************************
*                         DECLARATION OF PROTOTYPES
*******************************************************************************
*/
#if (TTY_SYNC_MCU == 1)
  #define RESYNC_OFFSET 384
#endif

/* --------------------------------------------------------------------- */
/* interleaver_state_t:                                                  */
/* type definition for storing the states of the diag_interleaver and    */
/* diag_deinterleaver, respectively (each instance of                    */
/* interleavers/deinterleavers must have its own variable of this type   */ 
/* --------------------------------------------------------------------- */

typedef struct {
  WORD16 B, D;           /* dimensions of the (de)interleaver              */
  WORD16 rows;           /* Number of rows in buffer                       */
  WORD16 clmn;           /* actual index within the (de)interleaver matrix */
  WORD16 ready;          /* Number of ready rows in (de)interleaver        */
  WORD16 *vector;        /* memory of the (de)interleaver                  */
  WORD16 num_sync_lines1;/* number of preceding lines in the interl. matrix*/
  WORD16 num_sync_lines2;/* number of preceding lines in the interl. matrix*/
  WORD16 num_sync_bits;  /* number of sync bits (demodulator sync)         */
  WORD16 *sync_index_vec;/* indices of the bits for deintl. synchronization*/
  WORD16 *scramble_vec;  /* sequence for scrambling                        */
  WORD16 *sequence;      /* m-sequence for synchronisation                 */
  
} interleaver_state_t;


/* --------------------------------------------------------------------- */
/* init_interleaver:                                                     */
/* function for initialization of diag_interleaver and                   */
/* diag_deinterleaver, respectively. The dimensions of the interleaver   */
/* must be specified:                                                    */
/* B = (horizontal) blocklength, D = (vertical distance)                 */
/* According to this specifications, this function initializes a         */
/* variable of type interleaver_state_t.                                 */
/*                                                                       */
/* Additionally, this function adds two types of sync information to the */
/* bitstream. The first sync info is for the demodulator and consists    */
/* of a sequence of alternating bits so that the tones produced by the   */
/* modulator are not the same all the time. This is essential for the    */
/* demodulator to find the transitions between adjacent bits. The bits   */
/* for this demodulator synchronization simply precede the bitstream.    */
/* A good choice for this is e.g. num_sync_lines1=2, which results in    */
/* 14 additional bits in case of the (B=7, D=2) interleaver.             */
/*                                                                       */
/* The second sync info is for synchronizing the deinterleaver and       */
/* of a m-sequence with excellent autocorrelation properties. These bits */
/* are positioned at the locations of the dummy bits, which are not used */
/* by the interleaver. In addition, even more bits for this              */
/* can be spent by inserting additional sync bits, which precede the     */
/* interleaver's bitstream. This is indicated by chosing                 */
/* num_sync_lines2>0.                                                    */
/*                                                                       */
/* Example: (B=7, D=2)-interleaver,                                      */
/*          num_sync_lines1=2 (demodulator sync bits are marked by 'd')  */
/*          num_sync_lines2=1 (deinterleaver sync bits are marked by 'x')*/
/*                                                                       */
/*     d     d     d     d     d     d     d                             */
/*     d     d     d     d     d     d     d                             */
/*     x     x     x     x     x     x     x                             */
/*     1     x     x     x     x     x     x                             */
/*     8     x     x     x     x     x     x                             */
/*    15     2     x     x     x     x     x                             */
/*    22     9     x     x     x     x     x                             */
/*    29    16     3     x     x     x     x                             */
/*    36    23    10     x     x     x     x                             */
/*    43    30    17     4     x     x     x                             */
/*    50    37    24    11     x     x     x                             */
/*    57    44    31    18     5     x     x                             */
/*    64    51    38    25    12     x     x                             */
/*    71    58    45    32    19     6     x                             */
/*    78    65    52    39    26    13     x                             */
/*    85    72    59    46    33    20     7                             */
/*    92    79    66    53    40    27    14                             */
/*    99    86    73    60    47    34    21                             */
/* --------------------------------------------------------------------- */

void init_interleaver(interleaver_state_t *intl_state, 
                      WORD16 B, WORD16 D,
                      WORD16 num_sync_lines1, WORD16 num_sync_lines2);
/*
 ***********************************************************************
 * Shutdown_interleaver()                                              *
 * *********************                                               *
 * Deallocate dyamically created buffers.                              *
 *                                                                     *
 * Input variables:                                                    *
 * intl_state : pointer to a variable of interleaver_state_t           *
 *              containing the interleaver state machine.              *
 ***********************************************************************
*/
void Shutdown_interleaver(interleaver_state_t *intl_state);

/* --------------------------------------------------------------------- */
/* reinit_interleaver:                                                   */
/* Same as init_interleaver() but without new allocation of buffers.     */
/* This function shall be used for initiation each new burst.            */
/* --------------------------------------------------------------------- */

void reinit_interleaver(interleaver_state_t *intl_state);


void init_deinterleaver(interleaver_state_t *intl_state, 
                        WORD16 B, WORD16 D);

/*
 ***********************************************************************
 * Shutdown_deinterleaver()                                            *
 * ************************                                            *
 * Deallocate dyamically created buffers.                              *
 *                                                                     *
 * Input variables:                                                    *
 * intl_state : pointer to a variable of interleaver_state_t           *
 *              containing the deinterleaver state machine.            *
 ***********************************************************************
*/
void Shutdown_deinterleaver(interleaver_state_t *intl_state);

void reinit_deinterleaver(interleaver_state_t *intl_state);

/* --------------------------------------------------------------------- */
/* calc_mute_positions:                                                  */
/* Calculation of the indices of the bits that have to be muted within   */
/* one burst. The indices are returned in the vector mute_positions.     */
/* --------------------------------------------------------------------- */

void calc_mute_positions(WORD16 *mute_positions, 
                         WORD16 num_rows_to_mute,
                         WORD16 start_position,
                         WORD16 B, 
                         WORD16 D);

BOOL mutingRequired(WORD16  actualIndex, 
                    WORD16 *mute_positions, 
                    WORD16  length_mute_positions);

/* --------------------------------------------------------------------- */
/* generate_scrambling_sequence:                                         */
/* Generation of the sequence used for scrambling. The sequence consists */
/* of 0 and 1 elements. The sequence is stored into the vector *sequence */
/* and the length of the sequence is specified by the variable length.   */
/* --------------------------------------------------------------------- */

void generate_scrambling_sequence(WORD16 *sequence, WORD16 length);

#if (TTY_SYNC_MCU_2 == 1)
  void zero_pad_deint(interleaver_state_t *intl_state);
#endif

#endif




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
*      File             : init_interleaver.c
*      Purpose          : initialization of the diagonal (chain) interleaver;
*                         definition of the type interleaver_state_t
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
  #include "init_interleaver.h"
  #include "m_sequence.h"
  #include <stdio.h>    
  #include <stdlib.h>
  #include "ctm_typedefs.h"

  extern void *l1gtt_malloc(UWORD32 size);
  extern void l1gtt_free(void *ptr);

  #if (TRACE_TYPE == 1) || (TRACE_TYPE == 4) || (TRACE_TYPE == 5)
    #include "rvt_gen.h"
    #include "l1_trace.h"
  #endif

  /*
  *******************************************************************************
  *                         PUBLIC PROGRAM CODE
  *******************************************************************************
  */

  void init_interleaver(interleaver_state_t *intl_state, 
                        WORD16 B, WORD16 D,
                        WORD16 num_sync_lines1, WORD16 num_sync_lines2)
  {
    const WORD16 demod_syncbits[] = { -1, -1, 1, -1, 1, 1, -1, 1 };
    WORD16 cnt, num_dummy_bits, num_add_bits, num_avail_bits, seq_length;
    WORD16 i,j,k;
        
    intl_state->B = B;
    intl_state->D = D;
    intl_state->scramble_vec = (WORD16*)l1gtt_malloc(B*sizeof(WORD16));
    intl_state->vector 
      = (WORD16*)l1gtt_malloc(((num_sync_lines1+num_sync_lines2+B)*B*D)*
                          sizeof(WORD16));
    intl_state->rows = (num_sync_lines1+num_sync_lines2+B)*B*D/B;
    intl_state->clmn = 0;
    intl_state->ready = (num_sync_lines1 + num_sync_lines2);
    intl_state->num_sync_lines1 = num_sync_lines1;
    intl_state->num_sync_lines2 = num_sync_lines2;
    
    generate_scrambling_sequence(intl_state->scramble_vec, B);
    
    /* fill in the sync bits for the synchronization of the demodulator */
    
    for (cnt=0; cnt<num_sync_lines1*B; cnt++)
      intl_state->vector[cnt] = demod_syncbits[cnt%8];

    /* calculate, how many bits for the deinterleaver synchronization */
    /* are available                                                  */
    
    num_dummy_bits  = B*(B-1)*D/2;       /* dummy bits of the interleaver */
    num_add_bits    = num_sync_lines2*B; /* additional bits               */
    num_avail_bits  = num_dummy_bits+num_add_bits;
    
    intl_state->num_sync_bits = num_avail_bits;
    
    /* Determine the next value (2^n)-1 that is */
    /* greater or equal to num_avail_bits        */
    seq_length = 0;
    for (cnt=2; cnt<10; cnt++)
      if ((1<<cnt)-1 >=num_avail_bits)
        {
          seq_length = (1<<cnt)-1;
          break;
        }
    
    /* Calculate the m-sequence of the according length */

    intl_state->sequence = (WORD16*)l1gtt_malloc(seq_length*sizeof(WORD16));

    if (intl_state->sequence==(WORD16*)NULL)
    {
  #if (TRACE_TYPE == 1) || (TRACE_TYPE == 4) || (TRACE_TYPE == 5)
      if (trace_info.current_config->l1_dyn_trace & 1<<L1_DYN_TRACE_GTT)
      {
        char str[50];

        sprintf(str,
                "Error while allocating memory for m-sequence\n\r");
        
        GTT_send_trace_cpy(str);
      }
  #endif    /* End trace  */
      //exit(1);  Should never come here !!
    }
    
    m_sequence(intl_state->sequence, seq_length);
    
    /* set-up a vector pointing to the bit positions that can be used for */
    /* storing the sync bits                                              */
    
    intl_state->sync_index_vec
      = (WORD16*)l1gtt_malloc(num_avail_bits*sizeof(WORD16));
    
    /* at first, the additional bits */

    for (cnt=0; cnt<num_add_bits; cnt++)
      intl_state->sync_index_vec[cnt] = cnt;
    
    /* now calculate the position of the interleaver's dummy bits */
    cnt = num_add_bits;
    
    for (i=0; i<B-1; i++)
      for (j=0; j<D; j++)
        for (k=i+1; k<B; k++)
          {
            intl_state->sync_index_vec[cnt] = num_add_bits + D*B*i + B*j + k;
            cnt++;
          }

    /* now fill all sync bits with the m_sequence */

    for (cnt=0; cnt<num_avail_bits; cnt++)
      intl_state->vector[num_sync_lines1*B + intl_state->sync_index_vec[cnt]] 
        = intl_state->sequence[cnt];

  }

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
  void Shutdown_interleaver(interleaver_state_t *intl_state)
  {
      l1gtt_free(intl_state->scramble_vec);
      l1gtt_free(intl_state->vector);
      l1gtt_free(intl_state->sequence);
      l1gtt_free(intl_state->sync_index_vec);
  }

  void reinit_interleaver(interleaver_state_t *intl_state)
  {
    const WORD16 demod_syncbits[] = { -1, -1, 1, -1, 1, 1, -1, 1 };
    
    WORD16 cnt;
        
    intl_state->clmn = 0;

    /* fill in the sync bits for the synchronization of the demodulator */
    
    for (cnt=0; cnt<intl_state->num_sync_lines1*intl_state->B; cnt++)
      intl_state->vector[cnt] = demod_syncbits[cnt%8];

    /* now fill all sync bits with the m_sequence */

    for (cnt=0; cnt<intl_state->num_sync_bits; cnt++)
      intl_state->vector[intl_state->num_sync_lines1*intl_state->B + 
                        intl_state->sync_index_vec[cnt]] 
        = intl_state->sequence[cnt];
  }

#if (TTY_SYNC_MCU == 1)
void init_diag_deint(interleaver_state_t *intl_state)
{
  WORD16 cnt;
  for(cnt=0;cnt<((intl_state->D)*(intl_state->B))*(intl_state->B)+RESYNC_OFFSET;cnt++)
  	intl_state->vector[cnt]=0;

}
#endif
  void init_deinterleaver(interleaver_state_t *intl_state, 
                         WORD16 B, WORD16 D)
  {
    intl_state->B = B;
    intl_state->D = D;
    intl_state->scramble_vec = (WORD16*)l1gtt_malloc(B*sizeof(WORD16));
  //  intl_state->scramble_vec = (WORD16*)calloc(B, sizeof(WORD16));
#if (TTY_SYNC_MCU == 1)
    intl_state->vector = (WORD16*)l1gtt_malloc((B*B*D+RESYNC_OFFSET)*sizeof(WORD16));
#else
    intl_state->vector = (WORD16*)l1gtt_malloc((B*B*D)*sizeof(WORD16));
#endif
  //  intl_state->vector = (WORD16*)calloc(B*B*D, sizeof(WORD16));
    intl_state->clmn = 0;
#if (TTY_SYNC_MCU == 1)
  init_diag_deint(intl_state);
#endif

    generate_scrambling_sequence(intl_state->scramble_vec, B);
  }

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
  void Shutdown_deinterleaver(interleaver_state_t *intl_state)
  {
      l1gtt_free(intl_state->scramble_vec);
      l1gtt_free(intl_state->vector);
  }


  void reinit_deinterleaver(interleaver_state_t *intl_state)
  {
    intl_state->clmn = 0;
  }

#if (TTY_SYNC_MCU_2 == 1)
  void zero_pad_deint(interleaver_state_t *intl_state)
  {
    WORD16 cnt;
    for(cnt=0;cnt<((intl_state->D)*(intl_state->B))*(intl_state->B)+RESYNC_OFFSET;cnt++)
  	intl_state->vector[cnt]=0;
  }
#endif

  void calc_mute_positions(WORD16 *mute_positions, 
                           WORD16 num_rows_to_mute,
                           WORD16 start_position,
                           WORD16 B, 
                           WORD16 D)
  {
    WORD16 row, clmn;
    WORD16 cnt=0;
    
    for (row=0; row<num_rows_to_mute; row++)
      {
        for (clmn=0; clmn<B; clmn++)
          {
            mute_positions[cnt] = start_position + B*row + clmn*(B*D-1);
            cnt++;
          }
      }
  }


  BOOL mutingRequired(WORD16  actualIndex, 
                      WORD16 *mute_positions, 
                      WORD16  length_mute_positions)
  {
    WORD16 cnt;
    
    for (cnt=0; cnt<length_mute_positions; cnt++)
      if (actualIndex == mute_positions[cnt])
        return true;
    
    return false;
  }


  void generate_scrambling_sequence(WORD16 *sequence, WORD16 length)
  {
    static const WORD16 scramble_sequence[] 
      = {-1, 1, -1, -1, 1, 1, 
         -1,-1,-1, 1,1,1, -1,-1,-1,-1, 1,1,1,1,-1,-1,-1,-1,-1,1,1,1,1,1};
    WORD16 cnt;

    if ((length > 30) || (length < 1))
    {
  #if (TRACE_TYPE == 1) || (TRACE_TYPE == 4) || (TRACE_TYPE == 5)
      if (trace_info.current_config->l1_dyn_trace & 1<<L1_DYN_TRACE_GTT)
      {
        char str[80];
                  
        sprintf(str,
                "generate_scrambling_sequence(): no lengths > 30 supported yet!\n\r");
        
        GTT_send_trace_cpy(str);
      }
  #endif    /* End trace  */
      //exit(1);  Should never come here !!
    }


    for (cnt=0; cnt<length; cnt++)
      sequence[cnt] = scramble_sequence[cnt];
  }

#endif // L1_GTT

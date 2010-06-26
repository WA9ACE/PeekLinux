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
*      File             : diag_interleaver.c
*      Purpose          : diagonal (chain) interleaver routine
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
  #include "diag_interleaver.h"
  #include "init_interleaver.h"
  #include "ctm_defines.h"

  #include <stdio.h>    
  #include <stdlib.h>



  /*
  *******************************************************************************
  *                         PUBLIC PROGRAM CODE
  *******************************************************************************
  */

  void diag_interleaver(WORD16 *out,
                        WORD16 *in,
                        WORD16  num_bits,
                        interleaver_state_t *intl_state)
  {
    WORD16 cnt, syncoffset;
    WORD16 i,j;
    
    for (cnt=0; cnt<num_bits; cnt++)
      { 
        /* The input values are diagonally inserted into the interleaver */
        /* matrix and the output values are read out line by line.       */
        
        syncoffset = (intl_state->B)*
          ((intl_state->num_sync_lines1)+(intl_state->num_sync_lines2));
        
        intl_state->vector[syncoffset + 
                          ((intl_state->B)*(intl_state->D)+1)*(intl_state->clmn)]
          = in[cnt]*(intl_state->scramble_vec[intl_state->clmn]);
        
        out[cnt] = intl_state->vector[intl_state->clmn];
        
        /* Increase the index of the actual column. If the end of the    */
        /* row is reached, the whole matrix is shifted by one row.       */
        intl_state->clmn++;
        if (intl_state->clmn == intl_state->B)
          {
            intl_state->clmn = 0;
            for (i=0; i<((intl_state->B)*(intl_state->D)-1+
                         (intl_state->num_sync_lines1)+
                         (intl_state->num_sync_lines2)); i++)
              for (j=0; j<(intl_state->B); j++)
                intl_state->vector[i*(intl_state->B)+j] = 
                  intl_state->vector[(i+1)*(intl_state->B)+j];
          }
      }
  }


  void diag_interleaver_flush(WORD16 *out,
                              WORD16  *num_bits_out,
                              interleaver_state_t *intl_state)
  {
    WORD16 cnt, syncoffset;
    WORD16 i,j;
    
    *num_bits_out = (((intl_state->B-1))*(intl_state->D) +
                     (intl_state->num_sync_lines1)+
                     (intl_state->num_sync_lines2)) * intl_state->B;
    
    for (cnt=0; cnt < *num_bits_out; cnt++) 
      { 
        /* The input values are diagonally inserted into the interleaver */
        /* matrix and the output values are read out line by line.       */
        
        syncoffset = (intl_state->B)*
          ((intl_state->num_sync_lines1)+(intl_state->num_sync_lines2));
        
        intl_state->vector[syncoffset + 
                          ((intl_state->B)*(intl_state->D)+1)*(intl_state->clmn)]
          = (intl_state->scramble_vec[intl_state->clmn]);

        out[cnt] = intl_state->vector[intl_state->clmn];
            
        /* Increase the index of the actual column. If the end of the    */
        /* row is reached, the whole matrix is shifted by one row.       */
        intl_state->clmn = (intl_state->clmn) + 1;
        if (intl_state->clmn == intl_state->B)
          {
            intl_state->clmn = 0;
            for (i=0; i<((intl_state->B)*(intl_state->D)-1+
                         (intl_state->num_sync_lines1)+
                         (intl_state->num_sync_lines2)); i++)
              for (j=0; j<(intl_state->B); j++)
                intl_state->vector[i*(intl_state->B)+j] = 
                  intl_state->vector[(i+1)*(intl_state->B)+j];
          }
      }
  }

#endif

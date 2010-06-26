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
*      File             : diag_deinterleaver.cc
*      Purpose          : diagonal (chain) deinterleaver routine
*
*      Changes since November 29, 2000:
*      - Bug in the last line of function shift_deinterleaver() corrected
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
  #include "diag_deinterleaver.h"
  #include "init_interleaver.h"

  #include <stdio.h>
  #include <stdlib.h>

  /*
  *******************************************************************************
  *                         PUBLIC PROGRAM CODE
  *******************************************************************************
  */
#if(TTY_SYNC_MCU_2==1)
#if(OPTIMISED==0)
WORD16 flush_diag_deint(WORD16 *out,
                        WORD16 *in,
                        interleaver_state_t *intl_state)
{
  WORD16 cnt;
  WORD16 i,j;
  WORD16 num_valid_bits=0;
  for (cnt=0; cnt<RESYNC_OFFSET+intl_state->B; cnt++)
    {   
      /* The input values are inserted into the last line of the */
      /* interleaver matrix and the output values are read out   */
      /* diagonally.                          */
      out[cnt] = intl_state->vector[((intl_state->B)*(intl_state->D)+1)*
                                   (intl_state->clmn)]*(intl_state->scramble_vec[intl_state->clmn]);
            /* Increase the index of the actual column. If the end of the    */
      /* row is reached, the whole matrix is shifted by one row.       */
      intl_state->clmn++;
      num_valid_bits++;

      if (intl_state->clmn == intl_state->B)
        {
          intl_state->clmn = 0;
          for (i=0; i<(((intl_state->B)*(intl_state->D)-1)+RESYNC_OFFSET/(intl_state->B))+1; i++)
            for (j=0; j<(intl_state->B); j++)
              intl_state->vector[i*(intl_state->B)+j] = 
                intl_state->vector[(i+1)*(intl_state->B)+j];
        }
     }
  return num_valid_bits;
}
#elif(OPTIMISED==1)
WORD16 flush_diag_deint(WORD16 *out,
                        WORD16 *in,
                        interleaver_state_t *intl_state)
{
  WORD16 cnt;
  WORD16 i,j;
  WORD16 num_valid_bits=0;
  WORD16 rows=0;
  for (cnt=0; cnt<RESYNC_OFFSET+intl_state->B; cnt++)
    {   
      /* The input values are inserted into the last line of the */
      /* interleaver matrix and the output values are read out   */
      /* diagonally.                          */
      out[cnt] = intl_state->vector[((intl_state->B)*(intl_state->D)+1)*
                                   (intl_state->clmn)+rows*intl_state->B]*(intl_state->scramble_vec[intl_state->clmn]);
            /* Increase the index of the actual column. If the end of the    */
      /* row is reached, the whole matrix is shifted by one row.       */
      intl_state->clmn++;
      num_valid_bits++;
      
      if (intl_state->clmn == intl_state->B)
        {
          intl_state->clmn = 0;
          rows++;
        }
     }
  return num_valid_bits;
}
#endif
#endif
#if(OPTIMISED==1)
void new_diag_deinterleaver(WORD16 *out,
                        WORD16 jump,
                        WORD16 *flush_vector,
                        WORD16 *in,
                        WORD16 num_valid_bits,
                        interleaver_state_t *intl_state)
{
  WORD16 cnt;
  WORD16 i,j;
  
  for (cnt=0; cnt<num_valid_bits; cnt++)
    {   
      /* The input values are inserted into the last line of the */
      /* interleaver matrix and the output values are read out   */
      /* diagonally.                                             */
      intl_state->vector[(intl_state->B)*(intl_state->D)*(intl_state->B-1)+RESYNC_OFFSET+
                        (intl_state->clmn)] = in[cnt];

      out[cnt] = flush_vector[jump];
      jump++;
      /* Increase the index of the actual column. If the end of the    */
      /* row is reached, the whole matrix is shifted by one row.       */
      intl_state->clmn++;
      
      if (intl_state->clmn == intl_state->B)
        {
          intl_state->clmn = 0;
          for (i=0; i<(((intl_state->B)*(intl_state->D)-1)+RESYNC_OFFSET/(intl_state->B))+1; i++)
            for (j=0; j<(intl_state->B); j++)
              intl_state->vector[i*(intl_state->B)+j] = 
                intl_state->vector[(i+1)*(intl_state->B)+j];
        }
    }
}
#endif

  void diag_deinterleaver(WORD16 *out,
                          WORD16 *in,
                          WORD16 num_valid_bits,
                          interleaver_state_t *intl_state)
  {
    WORD16 cnt;
    WORD16 i,j;
    
    for (cnt=0; cnt<num_valid_bits; cnt++)
    {   
        /* The input values are inserted into the last line of the */
        /* interleaver matrix and the output values are read out   */
        /* diagonally.                                             */
#if (TTY_SYNC_MCU == 1)
      intl_state->vector[(intl_state->B)*(intl_state->D)*(intl_state->B-1)+RESYNC_OFFSET+
                          (intl_state->clmn)] = in[cnt];
#else
        intl_state->vector[(intl_state->B)*(intl_state->D)*(intl_state->B-1)+
                          (intl_state->clmn)] = in[cnt];
#endif
        out[cnt] = intl_state->vector[((intl_state->B)*(intl_state->D)+1)*
                                     (intl_state->clmn)]*(intl_state->scramble_vec[intl_state->clmn]);

        /* Increase the index of the actual column. If the end of the    */
        /* row is reached, the whole matrix is shifted by one row.       */
        intl_state->clmn++;

        if (intl_state->clmn == intl_state->B)
        {
            intl_state->clmn = 0;
#if (TTY_SYNC_MCU == 1)
          for (i=0; i<(((intl_state->B)*(intl_state->D)-1)+RESYNC_OFFSET/(intl_state->B)+1); i++)
#else
            for (i=0; i<((intl_state->B)*(intl_state->D)-1); i++)
#endif         
              for (j=0; j<(intl_state->B); j++)
                intl_state->vector[i*(intl_state->B)+j] = 
                  intl_state->vector[(i+1)*(intl_state->B)+j];
        }
    }
  }




  void shift_deinterleaver(WORD16 shift,
                           WORD16 *insert_bits,
                           interleaver_state_t *ptr_state)
  {
    WORD16 cnt;
    
    if (shift>0) /* shift right */
    {
      for (cnt=(ptr_state->B-1)*(ptr_state->B)*(ptr_state->D)-1;
           cnt-shift>=0; cnt--)
        ptr_state->vector[cnt] = ptr_state->vector[cnt-shift];
      
      for (cnt=0; cnt<shift; cnt++)
        ptr_state->vector[cnt] = insert_bits[cnt];
    }
    else
    {
      shift = abs(shift);
      for (cnt=0; cnt<(ptr_state->B-1)*(ptr_state->B)*(ptr_state->D)-shift;
           cnt++)
        ptr_state->vector[cnt] = ptr_state->vector[cnt+shift];
      
      for (cnt=0; cnt<shift; cnt++)
        ptr_state->vector[(ptr_state->B-1)*(ptr_state->B)*(ptr_state->D)-shift+cnt]
          = insert_bits[cnt];
    }
  }
#if (TTY_SYNC_MCU == 1)
void flush_and_shift_deinterleaver(WORD16 shift,WORD16 *insert_bits,interleaver_state_t *ptr_state)
{
  WORD16 cnt;

  if (shift>0) /* shift right */
  {
    for (cnt=(ptr_state->B-1)*(ptr_state->B)*(ptr_state->D)+RESYNC_OFFSET+ptr_state->B-1; 
         cnt-(shift+(ptr_state->B-1)*(ptr_state->B)*(ptr_state->D))>=0; cnt--)
      ptr_state->vector[cnt] = ptr_state->vector[cnt-shift];
    
    for (cnt=0; cnt<shift; cnt++)
      ptr_state->vector[cnt+(ptr_state->B-1)*(ptr_state->B)*(ptr_state->D)] = insert_bits[cnt];
  }
  else    /* shift left*/
  {
    shift = -shift;
    for (cnt=(ptr_state->B-1)*(ptr_state->B)*(ptr_state->D); cnt<(ptr_state->B-1)*(ptr_state->B)*(ptr_state->D)+RESYNC_OFFSET+ptr_state->B-shift;
         cnt++)
      ptr_state->vector[cnt] = ptr_state->vector[cnt+shift];
    
    for (cnt=0; cnt<shift; cnt++)
      ptr_state->vector[(ptr_state->B-1)*(ptr_state->B)*(ptr_state->D)+RESYNC_OFFSET+ptr_state->B-shift+cnt]
        = insert_bits[cnt];
  }
 }
#endif //TTY_SYNC_MCU
#if(NEW_WKA_PATCH==1)

void just_shift(WORD16 shift,WORD16* in_bits,interleaver_state_t *intl_state)
{
  WORD16 cnt;
  WORD16 offset;

  if(shift>0)
  {
    offset=shift;
    for(cnt=0;cnt<offset;cnt++)
    {
      intl_state->vector[(intl_state->B)*(intl_state->D)*(intl_state->B-1)+RESYNC_OFFSET+intl_state->clmn-offset+cnt] = in_bits[cnt];
    }

  }
  else if(shift<0)
  {
    offset=-shift;
    for(cnt=0;cnt<offset;cnt++)
    {
      intl_state->vector[(intl_state->B)*(intl_state->D)*(intl_state->B-1)+RESYNC_OFFSET+intl_state->clmn+cnt] = in_bits[cnt];
    }
  }
}
#endif
#endif

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
*      File             : m_sequence.c
*      Purpose          : Calculation of m-sequences 
*                         (maximum-length sequecnes or pseudo noise)
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
  #include "m_sequence.h"
  #include <stdio.h>  
  #include "ctm_typedefs.h"

  #if (TRACE_TYPE == 1) || (TRACE_TYPE == 4) || (TRACE_TYPE == 5)
    #include "rvt_gen.h"
    #include "l1_trace.h"
  #endif

  /*
  *******************************************************************************
  *                         PUBLIC PROGRAM CODE
  *******************************************************************************
  */

  void m_sequence(WORD16 *sequence, WORD16 length)
  {
    WORD16  r, cnt, cnt2, filter_output;
    WORD16  s_vec[10] = {0,0,0,0,0,0,0,0,0,0};

    /* Matrix with primitive polynoms in GF(2).              */
    /* Each row of this table contains the coefficients      */
    /* a1, a2, a3, ..., a10. The coefficient a0 is always 1. */
    const WORD16 a_matrix[9][10] = {
      {1, 1, 0, 0, 0, 0, 0, 0, 0, 0},   /* r=2  */
      {1, 0, 1, 0, 0, 0, 0, 0, 0, 0},   /* r=3  */
      {1, 0, 0, 1, 0, 0, 0, 0, 0, 0},   /* r=4  */
      {0, 1, 0, 0, 1, 0, 0, 0, 0, 0},   /* r=5  */
      {1, 0, 0, 0, 0, 1, 0, 0, 0, 0},   /* r=6  */
      {1, 0, 0, 0, 0, 0, 1, 0, 0, 0},   /* r=7  */
      {1, 0, 0, 0, 1, 1, 0, 1, 0, 0},   /* r=8  */
      {0, 0, 0, 1, 0, 0, 0, 0, 1, 0},   /* r=9  */
      {0, 0, 1, 0, 0, 0, 0, 0, 0, 1}    /* r=10 */
    };
    
    /* Check whether the sequence length fits to */
    /* 2^r-1 and determine the root r            */
    r=0;
    for (cnt=2; cnt<=10; cnt++)
      if ((1<<cnt)-1 == length)
        r=cnt;
    
    if (r==0)
    {
  #if (TRACE_TYPE == 1) || (TRACE_TYPE == 4) || (TRACE_TYPE == 5)
      if (trace_info.current_config->l1_dyn_trace & 1<<L1_DYN_TRACE_GTT)
      {
        char str[80];
                  
        sprintf(str, 
                "m_sequence(): Invalid sequence length; must be 2^r-1 with 2<=r<=10\n\r");

        GTT_send_trace_cpy(str);
      }
  #endif    /* End trace  */
      return;
    }
    
    for (cnt=0; cnt<length; cnt++)
    {
        if (cnt==0)
        {
            sequence[cnt] = 1;
        }
        else
        {
            filter_output = 0;
            for (cnt2=0; cnt2<r; cnt2++)
            {
                filter_output = filter_output+s_vec[cnt2]*a_matrix[r-2][cnt2];
            }
            sequence[cnt] = 1 - (filter_output % 2);
        }
        for (cnt2=r-1; cnt2>0; cnt2--)
        {
            s_vec[cnt2] = s_vec[cnt2-1];
        }
        s_vec[0] = sequence[cnt];
    }
    
    /* transform unipolar sequence into bipolar */
    for (cnt=0; cnt<length; cnt++)
    {
        sequence[cnt] = 1-2*sequence[cnt];
    }
  }

#endif // L1_GTT

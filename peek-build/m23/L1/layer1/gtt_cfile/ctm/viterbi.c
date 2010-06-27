/*******************************************************************************
*
*      COPYRIGHT (C) 2000 BY ERICSSON EUROLAB DEUTSCHLAND GmbH
*
*      The program(s) may be used and/or copied only with the
*      written permission from Ericsson or in accordance
*      with the terms and conditions stipulated in the agreement or
*      contract under which the program(s) have been supplied.
*
*      Changes since October 13, 2000:
*      - Bugfix in function distance(): variable temp is of type Shortint now
*
*******************************************************************************
*
*      File             : viterbi.c
*      Purpose          : Initialization & Execution of the Viterbi decoder
*      Author           : Francisco Javier Gil Gomez
*
*******************************************************************************
*
* $Id: $ 
*
*/

#include "l1_macro.h"
#include "l1_confg.h"

#if (L1_GTT == 1)

  #include "l1_types.h"
  #include "ctm_defines.h"   /* CHC_RATE */
  #include "conv_poly.h"
  #include "l1gtt_const.h"
  #include <stdio.h>
  #include <stdlib.h>
  #include "ctm_typedefs.h"
  #include <string.h> /* For memcpy */

  #if (TRACE_TYPE == 1) || (TRACE_TYPE == 4) || (TRACE_TYPE == 5)
    #include "rvt_gen.h"
    #include "l1_trace.h"
  #endif

  int CHC_K_BLOCK_SIZE = ( BLOCK*CHC_K*2 );
  int CHC_RATE_MINUS_ONE = ( CHC_RATE - 1 );

  #if (CODE_VERSION == SIMULATION)
    #define INLINE
  #else
    #define INLINE inline
  #endif

  /**************************************************/
  /* Forward declarations of locally used functions */
  /**************************************************/

  WORD16 hamming_distance (WORD16 a, WORD16 b);
  /* Returns the Hamming distance between two words of length = CHC_RATE    */
  /* This function is used when a hard-decision decoding is performed       */

  WORD32 distance (WORD16* analog, WORD16  binary);
  /* This function is used instead 'hamming_distance' when a soft-decision  */
  /* decoding is performed                                                  */

  /*INLINE */void butterfly (WORD16* inputvalue, node_t *nodes);
  /* Calculates the new metrics of 4 paths in the trellis diagram at a time */
  /* These four paths define a 'butterfly'                                  */


  /***********************************************************************/
  /* viterbi_init()                                                      */
  /* **************                                                      */
  /* Initialization of the Viterbi decoder.                              */
  /*                                                                     */
  /* output variables:                                                   */
  /* *viterbi_state   initialized state variable of the decoder          */
  /*                                                                     */
  /***********************************************************************/

  void viterbi_init (viterbi_t* viterbi_state)
  {
    WORD16 polya, polyb, polyc, polyd;
    WORD16 out[CHC_RATE];   
    WORD16 i,j,p,temp;
    
    /* Initialize number of steps */
    
    viterbi_state->number_of_steps = 0;
    
    /* Initialize nodes */
    
    for (i=0; i<NUM_NODES; i++)
    {
        viterbi_state->nodes[i].metric    = 0;
        viterbi_state->nodes[i].oldmetric = 0;
        for (p=0; p<BLOCK*CHC_K; p++)
          {
            viterbi_state->nodes[i].path[p] = -1;
            viterbi_state->nodes[i].temppath[p] = -1;
        }
    }
    
    /* Get the polynomials to be used */
    
    polynomials (CHC_RATE, CHC_K, &polya, &polyb, &polyc, &polyd);  
    
    /* Calculate base_output for each node */
    
    for (i=0; i < NUM_NODES/2; i++)
    {  
        temp = 2*i;
        
        out[0] = temp & polya;
        out[1] = temp & polyb;
        if (CHC_RATE>2)
          out[2] = temp & polyc;
        if (CHC_RATE>3)
          out[3] = temp & polyd;
        
        for (p=0; p<CHC_RATE; p++)
        {
            temp = 0;
            for (j=0; j<CHC_K-1; j++)
            {
                temp += (out[p] >> j) & 0x1;  
            }
            out[p] = temp%2;
        }
        
        temp = 0;
        for (p=0; p<CHC_RATE; p++)
          temp += (1<<p) * out[CHC_RATE-1-p];
        
        viterbi_state->nodes[2*i].base_output = temp;
        
        /* To calculate the base_output of the following node, */
        /* just invert the bits                                */
        
        viterbi_state->nodes[2*i+1].base_output = (UWORD16)((~(UWORD16)temp) & ((1<<CHC_RATE)-1));
    }
  }


  /***********************************************************************/
  /* viterbi_reinit()                                                    */
  /* ****************                                                    */
  /* Re-Initialization of the Viterbi decoder. This function should be   */
  /* used for re-setting a Viterbi decoder that has already been         */
  /* initialized. In contrast to viterbi_init(), this reinit function    */
  /* does not calculate the values of all members of viterbi_state that  */
  /* do not change during the execution of the Viterbi algorithm.        */
  /*                                                                     */
  /* output variables:                                                   */
  /* *viterbi_state   initialized state variable of the decoder          */
  /*                                                                     */
  /***********************************************************************/

  void viterbi_reinit(viterbi_t* viterbi_state)
  {
    WORD16 i,p;
    
    /* Initialize number of steps */
    
    viterbi_state->number_of_steps = 0;
    
    /* Initialize nodes */
    
    for (i=0; i<NUM_NODES; i++)
    {
        viterbi_state->nodes[i].metric    = 0;
        viterbi_state->nodes[i].oldmetric = 0;
        for (p=0; p<BLOCK*CHC_K; p++)
        {
            viterbi_state->nodes[i].path[p] = -1;
            viterbi_state->nodes[i].temppath[p] = -1;
        }
    }
  }

  /***********************************************************************/
  /* viterbi_exec()                                                      */
  /* **************                                                      */
  /* Execution of the Viterbi decoder                                    */
  /*                                                                     */
  /* input variables:                                                    */
  /* inputword            Vector with gross bits                         */
  /* length_input         Number of valid gross bits in vector inputword.*/
  /*                      length_input must be an integer multiple of    */
  /*                      CHC_RATE                                       */
  /*                                                                     */
  /* output variables:                                                   */
  /* out                  Vector with the decoded net bits. The net bits */
  /*                      are either 0 or 1.                             */
  /* *num_valid_out_bits  Number of valid bits in vector out             */
  /*                                                                     */
  /* input/output variables:                                             */
  /* *viterbi_state       State variable of the decoder                  */
  /*                                                                     */
  /***********************************************************************/

  void  viterbi_exec(WORD16*  inputword, 
                     WORD16  length_input, 
                     WORD16*  out,       
                     WORD16* num_valid_out_bits,
                     viterbi_t* viterbi_state)
  {
    WORD32 i,j,p;
    WORD32 biggest;
    WORD32 min_metric;
    WORD16 input_norm[CHC_RATE];
    WORD32 groups;

    *num_valid_out_bits = 0;

    if (length_input % CHC_RATE != 0)
    {
  #if (TRACE_TYPE == 1) || (TRACE_TYPE == 4) || (TRACE_TYPE == 5)
      if (trace_info.current_config->l1_dyn_trace & 1<<L1_DYN_TRACE_GTT)
      {
        char str[80];
                  
        sprintf(str, 
                "viterbi_exec(): length_input must be a multiple of CHC_RATE!\n\r");

        GTT_send_trace_cpy(str);
      }
  #endif    /* End trace  */
        //exit(1);  Should never come here !!
    }

    groups = length_input/CHC_RATE;
    
    for (j=0; j<groups; j++)
    {
        for (i=0; i<CHC_RATE; i++)
          input_norm[i] = inputword[CHC_RATE*j+i];
        
        /* Calculate the new metrics */
          butterfly (input_norm, viterbi_state->nodes);
        
        //Update the metrics and the paths  
        for (i=0; i<NUM_NODES; i++)
        {
            viterbi_state->nodes[i].oldmetric = viterbi_state->nodes[i].metric;
            
            for (p=0; p<viterbi_state->number_of_steps; p++)
              viterbi_state->nodes[i].temppath[p] = viterbi_state->nodes[viterbi_state->nodes[i].continue_path_from].path[p];
            viterbi_state->nodes[i].temppath[viterbi_state->number_of_steps] =
                viterbi_state->nodes[i].new_entry;
        }
        
        //Find the path with the lowest metric  
        
        biggest = WORD32_MAX;
        min_metric = 0;
        
        for (i=0; i<NUM_NODES; i++)
        {
             memcpy (viterbi_state->nodes[i].path,
                     viterbi_state->nodes[i].temppath,
                     CHC_K_BLOCK_SIZE);
    
            if (viterbi_state->nodes[i].metric < biggest)
            {
                biggest = viterbi_state->nodes[i].metric;
                min_metric = i;
            }
        }
        
        if (viterbi_state->number_of_steps >= BLOCK*CHC_K-1)
        {
            out[*num_valid_out_bits] = viterbi_state->nodes[min_metric].path[0];
            (*num_valid_out_bits)++;
            
            for (i=0; i<NUM_NODES; i++)
              for (p=0; p<BLOCK*CHC_K-1; p++)
                viterbi_state->nodes[i].path[p] 
                  = viterbi_state->nodes[i].path[p+1];
        }
        else 
          viterbi_state->number_of_steps ++;
    }    
  }


  WORD16 hamming_distance (WORD16 a, WORD16 b)
  {
    WORD16 exor;
    WORD16 cnt;
    WORD16 tmp;
    
    a &= (1<<CHC_RATE)-1;
    b &= (1<<CHC_RATE)-1;
    tmp = 0;
    exor = a ^ b;
    for (cnt=0; cnt<CHC_RATE; cnt++)
      tmp += (exor >> cnt) & 0x1;
    return (tmp);
  }

  WORD32 distance (WORD16 * analog, WORD16  binary)
  {
    //Example:  analog = [13423,22432,-12555] binary = [1,1,0]  
    WORD16 ii;
    WORD16 temp;
    WORD32 dist=0;
    WORD16 analog_tmp;

    binary &= (1<<CHC_RATE)-1;
    
   // We transform 0 -> -16384, 1 -> 16384 
    for (ii=0; ii<CHC_RATE; ii++)
    {  

        // For speed, temp set to 16384 or -16384 (16383 used below prevents overflow)
        temp = ((binary >> ii) << 15) ^ 0xC000;

        analog_tmp = analog[CHC_RATE_MINUS_ONE-ii];
        
        if (analog_tmp > 16383)
          analog_tmp = 16383;
        else if (analog_tmp < -16383)
          analog_tmp = -16383;

        dist += abs(temp-analog_tmp);
    }
    return(dist);
  }

  /*INLINE */void butterfly (WORD16* inputvalue, node_t *nodes)
  {      
   WORD32 num;
   WORD32 halfnum;
   WORD32 half_num_plus_half_NUM_NODES;

   for (num=0; num<NUM_NODES; num+=2)
   {
    WORD32 my_metric         = nodes[num].oldmetric;
    WORD32 my_friends_metric = nodes[num+1].oldmetric;
    WORD32 path0, path1;
    
    halfnum = num>>1;
    half_num_plus_half_NUM_NODES = halfnum+(NUM_NODES>>1);

    path0 = my_metric         + distance(inputvalue, nodes[num].base_output);  
    path1 = my_friends_metric + distance(inputvalue, nodes[num+1].base_output);
    
    if (path0>path1)
    {
        nodes[halfnum].metric = path1;
        nodes[halfnum].continue_path_from = num+1;
    }
    else
    {
        nodes[halfnum].metric = path0;
        nodes[halfnum].continue_path_from = num;
    }
    nodes[halfnum].new_entry = 0;
    
    path0 = (my_metric         
             + distance(inputvalue, (WORD16)(~nodes[num].base_output)));  
    path1 = (my_friends_metric 
             + distance(inputvalue, (WORD16)(~nodes[num+1].base_output)));
    
    if (path0>path1)
    {
        nodes[half_num_plus_half_NUM_NODES].metric = path1;
        nodes[half_num_plus_half_NUM_NODES].continue_path_from = num+1;
    }
    else
    {
        nodes[half_num_plus_half_NUM_NODES].metric = path0;
        nodes[half_num_plus_half_NUM_NODES].continue_path_from = num;
    }
    nodes[half_num_plus_half_NUM_NODES].new_entry = 1;
   }
  }

#endif // L1_GTT

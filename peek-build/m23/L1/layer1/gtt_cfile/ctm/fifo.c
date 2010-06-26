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
*      File             : fifo.c
*      Author           : Matthias Doerbecker
*      Tested Platforms : Sun Solaris
*      Description      : Fifo structures for Shortint and Float
*
*      Revision history
*
*      Rev  Date       Name            Description
*      -------------------------------------------------------------------
*      pA1  14-Dec-99  M.Doerbecker    initial version
*
*******************************************************************************
*/

/*
*******************************************************************************
*                         INCLUDE FILES
*******************************************************************************
*/

#include "l1_macro.h"
#include "l1_confg.h"

#if (L1_GTT == 1)

  #include "l1_types.h"  /* basic data type aliases */
  #include <stdio.h>
  #include "fifo.h"
  #include "ctm_typedefs.h"
  #include <string.h>    /* For memcpy() */

  extern void *l1gtt_malloc(UWORD32 size);
  extern void l1gtt_free(void *ptr);

  #include "l1_macro.h"
  #include "l1_confg.h"
  #include "l1_types.h"
  #include "sys_types.h"
  #include "l1_const.h"
  #include "l1_signa.h" 
  #if TESTMODE
    #include "l1tm_defty.h"
  #endif
  #if (AUDIO_TASK == 1)
    #include "l1audio_const.h"
    #include "l1audio_cust.h"
    #include "l1audio_signa.h"
    #include "l1audio_defty.h"
    #include "l1audio_msgty.h"
  #endif
  
  #if (L1_GTT == 1)
    #include "l1gtt_const.h"
    #include "l1gtt_defty.h"
    #include "l1gtt_msgty.h"
    #include "l1gtt_signa.h"
  #endif
  
  #if (L1_MP3 == 1)
    #include "l1mp3_defty.h"
    #include "l1mp3_signa.h"
    #include "l1mp3_msgty.h"
  #endif
  
  #if (L1_MIDI == 1)
    #include "l1midi_defty.h"
    #include "l1midi_signa.h"
    #include "l1midi_msgty.h"
  #endif
  
  #if (L1_AAC == 1)
    #include "l1aac_defty.h"
    #include "l1aac_signa.h"
    #include "l1aac_msgty.h"
  #endif
  #if (L1_DYN_DSP_DWNLD == 1)
    #include "l1_dyn_dwl_signa.h"
    #include "l1_dyn_dwl_msgty.h"
  #endif  
  
  #include "l1_defty.h"
  #include "cust_os.h"
  #include "l1_msgty.h"
  #include "l1_varex.h"
  #include "l1_proto.h"
  #include "l1_mftab.h"
  #include "l1_tabs.h"
  #include "l1_ver.h"

  #if (TRACE_TYPE == 1) || (TRACE_TYPE == 4) || (TRACE_TYPE == 5)
    #include "rvt_gen.h"
    #include "l1_trace.h"
  #endif


  extern T_GTT_DEBUG l1_gtt_debug;
  /*
  *******************************************************************************
  *                         LOCAL VARIABLES AND TABLES
  *******************************************************************************
  */


  /*
  *******************************************************************************
  *                         LOCAL PROGRAM CODE
  *******************************************************************************
  */


  int Shortint_fifo_init(fifo_state_t *fifo_state,
                         WORD32 length_fifo)
  {
    if (length_fifo<1)
    {
  #if (TRACE_TYPE == 1) || (TRACE_TYPE == 4) || (TRACE_TYPE == 5)
        if (trace_info.current_config->l1_dyn_trace & 1<<L1_DYN_TRACE_GTT)
        {
          char str[70];

          sprintf(str,
                  "Shortint_fifo_init(): length_fifo must be greater than 0!\n\r");

          GTT_send_trace_cpy(str);
        }
  #endif    /* End trace  */
        return -1;
    }


     fifo_state->buffer_Shortint = (WORD16*)l1gtt_malloc(length_fifo*sizeof(WORD16));

    if (fifo_state->buffer_Shortint == (WORD16*)NULL)
    {
  #if (TRACE_TYPE == 1) || (TRACE_TYPE == 4) || (TRACE_TYPE == 5)
        if (trace_info.current_config->l1_dyn_trace & 1<<L1_DYN_TRACE_GTT)
        {
          char str[50];
                  
          sprintf(str,
                  "Shortint_fifo_init(): Memory Allocation Error\n\r");

          GTT_send_trace_cpy(str);
        }
  #endif    /* End trace  */
        return -1;
    }


    fifo_state->buffer_Float       = (Float*)NULL;
    fifo_state->buffer_Char        = (Char*)NULL;
    fifo_state->length_buffer      = length_fifo;
    fifo_state->num_entries_actual = 0;
    fifo_state->fifo_type          = SHORTINT_FIFO;
    fifo_state->magic_number       = 12345;

    return 0;
  }

  /************************************************************************/

  int Shortint_fifo_reset(fifo_state_t *fifo_state)
  {
    fifo_state->num_entries_actual = 0;
    return 0;
  }

  /************************************************************************/

  int Shortint_fifo_exit(fifo_state_t *fifo_state)
  {
    if (fifo_state->magic_number != 12345) 
    {
  #if (TRACE_TYPE == 1) || (TRACE_TYPE == 4) || (TRACE_TYPE == 5)
      if (trace_info.current_config->l1_dyn_trace & 1<<L1_DYN_TRACE_GTT)
      {
        char str[70];
                  
        sprintf(str,
                "Shortint_fifo_exit(): Fifo must be initialized before exit!\n\r");
        
        GTT_send_trace_cpy(str);
      }
  #endif    /* End trace  */
        return -1;
    }


    l1gtt_free(fifo_state->buffer_Shortint);
    //free(fifo_state->buffer_Float);
    fifo_state->buffer_Shortint    = (WORD16*)NULL;
    fifo_state->buffer_Float       = (Float*)NULL;
    fifo_state->num_entries_actual = 0;
    return 0;
  }
    
  /************************************************************************/
#if (L1_GTT_FIFO_TEST_ATOMIC == 1)
   extern void INT_DisableIRQ(void);
   extern void INT_EnableIRQ(void);
#endif

  int Shortint_fifo_push(fifo_state_t *fifo_state, 
                         WORD16     *elements_to_push,
                         WORD32      num_elements_to_push)
  {
    WORD32 cnt;

    if (fifo_state->magic_number != 12345) 
    {
  #if (TRACE_TYPE == 1) || (TRACE_TYPE == 4) || (TRACE_TYPE == 5)
      if (trace_info.current_config->l1_dyn_trace & 1<<L1_DYN_TRACE_GTT)
      {
        char str[80];
                  
        sprintf(str,
                "Function Shortint_fifo_push(): Fifo must be initialized before use!\n\r");
        
        GTT_send_trace_cpy(str);
      }
  #endif    /* End trace  */
        return -1;
      }
    
    if (fifo_state->fifo_type != SHORTINT_FIFO) 
    {
  #if (TRACE_TYPE == 1) || (TRACE_TYPE == 4) || (TRACE_TYPE == 5)
      if (trace_info.current_config->l1_dyn_trace & 1<<L1_DYN_TRACE_GTT)
      {
        char str[80];
                  
        sprintf(str,
                "Shortint_fifo_push(): Initialization was not of type SHORTINT_FIFO!\n\r");
        
        GTT_send_trace_cpy(str);
      }
  #endif    /* End trace  */
        return -1;
      }

    #if (L1_GTT_FIFO_TEST_ATOMIC == 1)
    INT_DisableIRQ(); 
    #endif
    if (num_elements_to_push+fifo_state->num_entries_actual > 
        fifo_state->length_buffer)
    {
  #if (TRACE_TYPE == 1) || (TRACE_TYPE == 4) || (TRACE_TYPE == 5)
      if (trace_info.current_config->l1_dyn_trace & 1<<L1_DYN_TRACE_GTT)
      {
        char str[110];
                  
        l1_gtt_debug.fn_gtt_error_count++;        
                  
        sprintf(str,
                "%ld: fifo_push: Overflow %d into %d \n\r",
                l1s.actual_time.fn_mod42432,
                num_elements_to_push,
                fifo_state->length_buffer);

        GTT_send_trace_cpy(str);
      }
  #endif    /* End trace  */
        #if (L1_GTT_FIFO_TEST_ATOMIC == 1)
        INT_EnableIRQ();
        #endif
        return -1;
      }


    if(num_elements_to_push>4)
    {
      memcpy(&(fifo_state->buffer_Shortint[fifo_state->num_entries_actual]),elements_to_push,2*num_elements_to_push);
    }
    else
    {
    /* append new elements at the end of the buffer */
    for (cnt=0; cnt<num_elements_to_push; cnt++)
      fifo_state->buffer_Shortint[fifo_state->num_entries_actual+cnt] 
        = elements_to_push[cnt];
    }

    fifo_state->num_entries_actual += num_elements_to_push;
    #if (L1_GTT_FIFO_TEST_ATOMIC == 1)
    INT_EnableIRQ();
    #endif
    return 0;
  }

  /************************************************************************/

  int Shortint_fifo_pop(fifo_state_t *fifo_state, 
                        WORD16     *popped_elements,
                        WORD32      num_elements_to_pop)
  {
    WORD32 cnt;

    if (fifo_state->magic_number != 12345) 
    {
  #if (TRACE_TYPE == 1) || (TRACE_TYPE == 4) || (TRACE_TYPE == 5)
      if (trace_info.current_config->l1_dyn_trace & 1<<L1_DYN_TRACE_GTT)
      {
        char str[80];
                  
        sprintf(str,
                "Shortint_fifo_pop(): Fifo must be initialized before exit!\n\r");
        
        GTT_send_trace_cpy(str);
      }
  #endif    /* End trace  */
        return -1;
    }
    
    if (fifo_state->fifo_type != SHORTINT_FIFO) 
    {
  #if (TRACE_TYPE == 1) || (TRACE_TYPE == 4) || (TRACE_TYPE == 5)
      if (trace_info.current_config->l1_dyn_trace & 1<<L1_DYN_TRACE_GTT)
      {
        char str[90];
                  
        sprintf(str,
                "Shortint_fifo_pop(): Initialization was not of type SHORTINT_FIFO!\n\r");
        
        GTT_send_trace_cpy(str);
      }
  #endif    /* End trace  */
        return -1;
    }

     #if (L1_GTT_FIFO_TEST_ATOMIC == 1)
     INT_DisableIRQ();
     #endif
    if (num_elements_to_pop > fifo_state->num_entries_actual)
    {
  #if (TRACE_TYPE == 1) || (TRACE_TYPE == 4) || (TRACE_TYPE == 5)
      if (trace_info.current_config->l1_dyn_trace & 1<<L1_DYN_TRACE_GTT)
      {
        char str[120];
                  
        l1_gtt_debug.fn_gtt_error_count++;        
        sprintf(str, 
                "Shortint_fifo_pop(): Buffer underrun while popping %d elements; only %d elements in buffer!\n\r",
                num_elements_to_pop,
                fifo_state->num_entries_actual);
        
        GTT_send_trace_cpy(str);
      }
  #endif    /* End trace  */
      #if (L1_GTT_FIFO_TEST_ATOMIC == 1)
      INT_EnableIRQ();
      #endif
      return -1;
    }

    
    if(num_elements_to_pop>4)
    {
       memcpy(popped_elements,fifo_state->buffer_Shortint,2*num_elements_to_pop);
    }
    else
    {
    /* read out first (oldest) element from the buffer */
    for (cnt=0; cnt<num_elements_to_pop; cnt++)
      popped_elements[cnt] = fifo_state->buffer_Shortint[cnt];
    }

    fifo_state->num_entries_actual -= num_elements_to_pop;

    /* shift whole buffer to the left */
    
    /* TO Do: Remove before final release. Left here for reference in case
     * optimization is incorrect. 
     *
     * for (cnt=0; cnt<fifo_state->num_entries_actual; cnt++)
     * fifo_state->buffer_Shortint[cnt] = 
     *   fifo_state->buffer_Shortint[cnt+num_elements_to_pop]; */
    memcpy(fifo_state->buffer_Shortint,
           &(fifo_state->buffer_Shortint[num_elements_to_pop]),
           2*fifo_state->num_entries_actual);
    #if (L1_GTT_FIFO_TEST_ATOMIC == 1)
    INT_EnableIRQ();
    #endif
    
    return(0);
  }

  /************************************************************************/

  int Shortint_fifo_peek(fifo_state_t *fifo_state, 
                         WORD16     *peeked_elements,
                         WORD32      num_elements_to_peek)
  {
    WORD32 cnt;

    if (fifo_state->magic_number != 12345) 
    {
  #if (TRACE_TYPE == 1) || (TRACE_TYPE == 4) || (TRACE_TYPE == 5)
      if (trace_info.current_config->l1_dyn_trace & 1<<L1_DYN_TRACE_GTT)
      {
        char str[90];
                  
        sprintf(str,
                "Shortint_fifo_peek(): Fifo must be initialized before exit!\n\r");

        GTT_send_trace_cpy(str);
      }
  #endif    /* End trace  */
      return -1;
    }
    
    if (fifo_state->fifo_type != SHORTINT_FIFO) 
    {
  #if (TRACE_TYPE == 1) || (TRACE_TYPE == 4) || (TRACE_TYPE == 5)
      if (trace_info.current_config->l1_dyn_trace & 1<<L1_DYN_TRACE_GTT)
      {
        char str[90];
                  
        sprintf(str,
                "Shortint_fifo_peek(): Initialization was not of type SHORTINT_FIFO!\n\r");

        GTT_send_trace_cpy(str);
      }
  #endif    /* End trace  */
      return -1;
    }

    #if (L1_GTT_FIFO_TEST_ATOMIC == 1)
    INT_DisableIRQ();
    #endif
    if (num_elements_to_peek > fifo_state->num_entries_actual)
    {
  #if (TRACE_TYPE == 1) || (TRACE_TYPE == 4) || (TRACE_TYPE == 5)
      if (trace_info.current_config->l1_dyn_trace & 1<<L1_DYN_TRACE_GTT)
      {
        char str[110];
                  
        l1_gtt_debug.fn_gtt_error_count++;        
        sprintf(str,
                "Shortint_fifo_peek(): Trying to peek %d elements; only %d elements in buffer!\n\r", 
                num_elements_to_peek,
                fifo_state->num_entries_actual);

        GTT_send_trace_cpy(str);
      }
  #endif    /* End trace  */
      #if (L1_GTT_FIFO_TEST_ATOMIC == 1)
      INT_EnableIRQ();
      #endif
      return -1;
    }
   
    /* read out first (oldest) element from the buffer */
    for (cnt=0; cnt<num_elements_to_peek; cnt++)
      peeked_elements[cnt] = fifo_state->buffer_Shortint[cnt];
    
    #if (L1_GTT_FIFO_TEST_ATOMIC == 1)
    INT_EnableIRQ();
    #endif
    return(0);
  }

  /************************************************************************/
    
  WORD32 Shortint_fifo_check(fifo_state_t *fifo_state)
  { 
    #if (L1_GTT_FIFO_TEST_ATOMIC == 1)  
      WORD32 num_entries_actual;
    #endif
    if (fifo_state->magic_number != 12345) 
    {
  #if (TRACE_TYPE == 1) || (TRACE_TYPE == 4) || (TRACE_TYPE == 5)
      if (trace_info.current_config->l1_dyn_trace & 1<<L1_DYN_TRACE_GTT)
      {
        char str[80];
                  
        sprintf(str,
                "Shortint_fifo_check(): Fifo must be initialized before use!\n\r");

        GTT_send_trace_cpy(str);
      }
  #endif    /* End trace  */
      return -1;
    }
    #if (L1_GTT_FIFO_TEST_ATOMIC == 1)
    INT_DisableIRQ();
    #endif
   
    #if (L1_GTT_FIFO_TEST_ATOMIC == 1)  
    num_entries_actual = fifo_state->num_entries_actual;
    INT_EnableIRQ();
    return(num_entries_actual);
    #else
    return(fifo_state->num_entries_actual);
    #endif
  }

#endif // L1_GTT

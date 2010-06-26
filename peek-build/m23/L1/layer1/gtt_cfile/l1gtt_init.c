/************* Revision Controle System Header *************
 *                  GSM Layer 1 software
 * L1GTT_INIT.C
 *
 *        Filename l1gtt_init.c
 *  Copyright 2003 (C) Texas Instruments
 *
 ************* Revision Controle System Header *************/


#include "l1_macro.h"
#include "l1_confg.h"

#if (L1_GTT == 1)
  #if (CODE_VERSION == SIMULATION)
    #include <string.h>
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
      #include "l1audio_defty.h"
    #endif

    #include "l1_types.h"
    #include "fifo.h"
    #include "l1gtt_baudot_functions.h"
    #include "ctm_transmitter.h"
    #include "ctm_receiver.h"
    #include "l1gtt_const.h"
    #include "l1gtt_defty.h"
    #include "l1gtt_varex.h"

    #if (L1_MP3 == 1)
      #include "l1mp3_defty.h"
    #endif

    #if (L1_MIDI == 1)
      #include "l1midi_defty.h"
    #endif

    #include "l1_defty.h"
    #include "cust_os.h"
    #include "l1_msgty.h"
    #include "l1_varex.h"
    #include "l1_proto.h"
    #include "l1_mftab.h"

    #include "l1_tabs.h"
  
    #if L2_L3_SIMUL
      #include "hw_debug.h"
    #endif      
  #else
    #include <string.h>
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
      #include "l1audio_defty.h"
    #endif  

    #include "fifo.h"
    #include "l1gtt_baudot_functions.h"
    #include "ctm_transmitter.h"
    #include "ctm_receiver.h"
    #include "l1gtt_const.h"
    #include "l1gtt_defty.h"
    #include "l1gtt_varex.h"

    #if (L1_MP3 == 1)
      #include "l1mp3_defty.h"
    #endif

    #if (L1_MIDI == 1)
      #include "l1midi_defty.h"
    #endif

    #include "l1_defty.h"
    #include "cust_os.h"
    #include "l1_msgty.h"
    #include "l1_varex.h"
    #include "l1_proto.h"
    #include "l1_mftab.h"

    #include "l1_tabs.h"
  
    #if L2_L3_SIMUL
      #include "hw_debug.h"
    #endif
  #endif

  #if (TRACE_TYPE == 1) || (TRACE_TYPE == 4) || (TRACE_TYPE == 5)
    #include "rvt_gen.h"
    #include "l1_trace.h"
  #endif

#if (OP_L1_STANDALONE == 0) 
  #if ((LOCOSTO_LITE == 1) && (BAE_STATE == 1))
  #include "typedefs.h"
  #include "os.h"
  #include "vsi.h"
  extern T_HANDLE bae_data_pool_handle;
  #endif
#endif

  #if (CODE_VERSION == SIMULATION)
    // Declaration of FIFO headers
    extern T_TTY_FIFO_struct header_fifo1;
    extern T_TTY_FIFO_struct header_fifo2;
    extern T_TTY_FIFO_struct header_fifo3;
    extern T_TTY_FIFO_struct header_fifo4;

    // Declaration of FIFO data areas
    extern API            fifo1[FIFO_1_size];
    extern API            fifo2[FIFO_2_size];
    extern API            fifo3[FIFO_3_size];
    extern API            fifo4[FIFO_4_size];
  #endif

  /**************************************/
  /* Extern prototypes                  */
  /**************************************/

#if (CODE_VERSION != SIMULATION)
  UWORD16* TTY_addr_dsp2mcu(UWORD16 dsp_address);
#else
  #define TTY_addr_dsp2mcu
  extern UWORD16 *fifo2_write_addr;     // internal write pointer for fifo2.
  extern UWORD8  fifo2_write_counter;    // 8 times write counter for fifo2.
#endif
  void init_baudot_encode(baudot_encode_state_t* state);
  void init_ctm_transmitter(tx_state_t* tx_state);
  void init_ctm_receiver(rx_state_t* rx_state);
  void TTY_fifo_initialize(void);
  extern tx_state_t   *tx_state_ptr;
  extern rx_state_t   *rx_state_ptr;
  extern baudot_encode_state_t *baudot_encode_state_ptr;
  extern fifo_state_t  *signalFifoState_ptr;
  extern fifo_state_t  *ctmOutTTYCodeFifoState_ptr;
  extern fifo_state_t  *baudotToCtmFifoState_ptr;
  extern fifo_state_t  *ctmToBaudotFifoState_ptr;
  extern fifo_state_t  *baudotCodeFifoState_ptr;    
  extern fifo_state_t  *ctmCodeULFifoState_ptr;     
  extern fifo_state_t  *global_ul_ptr;
  extern fifo_state_t  *global_dl_ptr;

#if L2_L3_SIMUL
    // GTT test
    #define GTT_TEST_LENGTH   116 
    char gtt_test_string[] = "ABCDEFG +-=$,':;?!/. HIJKLMNOP 0123456789 =G$T$T$ T?E?S(T) S+T-R'I;N?G (!T.T,Y$S+T1E2P3H4A5N)E E/N/D O/F T/H/E TEST ";
#endif

  T_GTT_DEBUG  l1_gtt_debug;
    
  /*-------------------------------------------------------*/
  /* l1gtt_initialize_var()                                */
  /*-------------------------------------------------------*/
  /* Parameters :                                          */
  /* Return     :                                          */
  /* Functionality :                                       */
  /*  Initialize L1 GTT global variables                   */
  /*-------------------------------------------------------*/
  void l1gtt_initialize_var(void)
  {
    /****************************************/
    /* L1A/L1S communication initialization */
    /****************************************/

    l1a_l1s_com.gtt_task.command.start = FALSE;
    l1a_l1s_com.gtt_task.command.stop  = FALSE;

    l1a_l1s_com.gtt_task.l1_gtt_mode = TTY_L1_NOTTY;

    // Test variables
    //---------------

#if L2_L3_SIMUL
    // GTT test
    l1a_l1s_com.gtt_task.test.scen_enable = 0;
    l1s.gtt_test.scen_index               = 0;
    l1s.gtt_test.scen_length              = GTT_TEST_LENGTH;
    l1s.gtt_test.scen_ptr                 = gtt_test_string;
    l1s.gtt_test.chars_to_push            = 1;
    l1s.gtt_test.frame_count              = 0; 
#endif
  }

  /*-------------------------------------------------------*/
  /* l1gtt_initialize()                                    */
  /*-------------------------------------------------------*/
  /* Parameters :                                          */
  /* Return     :                                          */
  /* Functionality :                                       */
  /*  Initialize GTT                                       */
  /*-------------------------------------------------------*/
  void l1gtt_initialize(void)
  {
    WORD16 i = 0; 
    API* ptr;
    char str[64];
    long retval = 0;    
    
    #if((TRACE_TYPE == 1) || (TRACE_TYPE == 4))     
      sprintf(str,"GTT INIT START %ld \n\r",l1s.actual_time.fn_mod42432); 
      rvt_send_trace_cpy ((T_RVT_BUFFER)str, trace_info.l1_trace_user_id,
          (T_RVT_MSG_LG)strlen(str), RVT_ASCII_FORMAT);    
   #endif       
    
#if (OP_L1_STANDALONE == 1) || (LOCOSTO_LITE==0) || (BAE_STATE == 0)
    tx_state_ptr               = &tx_state_struct;
    rx_state_ptr               = &rx_state_struct;
    baudot_encode_state_ptr    = &baudot_encode_state_struct;
    signalFifoState_ptr        = &signalFifoState_struct;
    ctmOutTTYCodeFifoState_ptr = &ctmOutTTYCodeFifoState_struct;
    baudotToCtmFifoState_ptr   = &baudotToCtmFifoState_struct;
    ctmToBaudotFifoState_ptr   = &ctmToBaudotFifoState_struct;
    baudotCodeFifoState_ptr    = &baudotCodeFifoState_struct;    
    ctmCodeULFifoState_ptr     = &ctmCodeULFifoState_struct;     
    global_ul_ptr              = &global_ul_struct;
    global_dl_ptr              = &global_dl_struct;
#else

    
    retval += os_AllocateMemory ( gsp_get_taskid(), 
            (unsigned long **)&(tx_state_ptr), 
            sizeof(tx_state_t), 
		    OS_NO_SUSPEND, 
            bae_data_pool_handle );
    
    retval += os_AllocateMemory ( gsp_get_taskid(), 
            (unsigned long **)&(rx_state_ptr), 
            sizeof(rx_state_t), 
		    OS_NO_SUSPEND, 
            bae_data_pool_handle );
    
    retval += os_AllocateMemory ( gsp_get_taskid(), 
            (unsigned long **)&(baudot_encode_state_ptr), 
            sizeof(baudot_encode_state_t), 
		    OS_NO_SUSPEND, 
            bae_data_pool_handle );
    
    retval += os_AllocateMemory ( gsp_get_taskid(), 
            (unsigned long **)&(signalFifoState_ptr), 
            sizeof(fifo_state_t), 
		    OS_NO_SUSPEND, 
            bae_data_pool_handle );
    
    retval += os_AllocateMemory ( gsp_get_taskid(), 
            (unsigned long **)&(ctmOutTTYCodeFifoState_ptr), 
            sizeof(fifo_state_t), 
		    OS_NO_SUSPEND, 
            bae_data_pool_handle );

    retval += os_AllocateMemory ( gsp_get_taskid(), 
            (unsigned long **)&(baudotToCtmFifoState_ptr), 
            sizeof(fifo_state_t), 
		    OS_NO_SUSPEND, 
            bae_data_pool_handle );
    
    retval += os_AllocateMemory ( gsp_get_taskid(), 
            (unsigned long **)&(ctmToBaudotFifoState_ptr), 
            sizeof(fifo_state_t), 
		    OS_NO_SUSPEND, 
            bae_data_pool_handle );
    
    retval += os_AllocateMemory ( gsp_get_taskid(), 
            (unsigned long **)&(baudotCodeFifoState_ptr), 
            sizeof(fifo_state_t), 
		    OS_NO_SUSPEND, 
            bae_data_pool_handle );
    
    retval += os_AllocateMemory ( gsp_get_taskid(), 
            (unsigned long **)&(ctmCodeULFifoState_ptr), 
            sizeof(fifo_state_t), 
		    OS_NO_SUSPEND, 
            bae_data_pool_handle );
    
    retval += os_AllocateMemory ( gsp_get_taskid(), 
            (unsigned long **)&(global_ul_ptr), 
            sizeof(fifo_state_t), 
		    OS_NO_SUSPEND, 
            bae_data_pool_handle );
    
    retval += os_AllocateMemory ( gsp_get_taskid(), 
            (unsigned long **)&(global_dl_ptr), 
            sizeof(fifo_state_t), 
		    OS_NO_SUSPEND, 
            bae_data_pool_handle );

  #if (TRACE_TYPE == 1) || (TRACE_TYPE == 4) || (TRACE_TYPE == 5)
    if (trace_info.current_config->l1_dyn_trace & 1<<L1_DYN_TRACE_GTT)
    {
        if(retval == 0)
            GTT_send_trace_cpy("GTT Init Mem Alloc Success.\n\r");
        else
            GTT_send_trace_cpy("GTT Init Mem Alloc ERROR.\n\r");
    }
  #endif /* End trace */

    
#endif // (OP_L1_STANDALONE == 1) || (LOCOSTO_LITE==0) || (BAE_STATE == 0)    

    
    /* Debug Structure Initialization */
    l1_gtt_debug.fn_gtt_sync = 0;
    l1_gtt_debug.fn_gtt_main_start = 0;
    l1_gtt_debug.fn_gtt_main_end = 0;
    l1_gtt_debug.fn_gtt_proc_flag = 0;
    l1_gtt_debug.fn_gtt_error_count = 0;
    l1_gtt_debug.odd_fifo3_count = 0;
    
    /*********************************/
    /* Set up transmitter & receiver */
    /*********************************/

    init_baudot_encode(baudot_encode_state_ptr);
    init_ctm_transmitter(tx_state_ptr);
    init_ctm_receiver(rx_state_ptr);  

    /*****************/
    /* Reset globals */
    /*****************/

    cntFramesSinceBurstInit          = 0;
    cntSamplesSinceLastBypassFromCTM = 0;
    cntSamplesSinceLastCTMFromBypass = 0;
    cntTransmittedEnquiries          = 0;
    ctmFromFarEndDetected     = false;
    ctmCharacterTransmitted   = false;
    enquiryFromFarEndDetected = false;
    ctmTransmitterIsIdle      = true;
    earlyMutingRequired       = false;
    cyclesSinceLastEnquiryBurst      = 0; 
    cntSamplesSinceEnquiryDetected = 0;
    transparentMode = false;

    /******************************************************/
    /* Initialize the fifo buffers used in CTM processing */
    /******************************************************/

    Shortint_fifo_init(signalFifoState_ptr, 
                       48); 
      
#if(TTY_SYNC_MCU_2 == 0)
    Shortint_fifo_init(ctmOutTTYCodeFifoState_ptr,  
                       2);
#else
    Shortint_fifo_init(ctmOutTTYCodeFifoState_ptr,  
                       12);
#endif

    /* To hold TTY code received in DL within 200ms */
    Shortint_fifo_init(ctmToBaudotFifoState_ptr,  
                       12);

    Shortint_fifo_init(baudotToCtmFifoState_ptr,  
                       3);

    Shortint_fifo_init(baudotCodeFifoState_ptr, 
                       MAX_BAUDOT_BITS_OUT);

    Shortint_fifo_init(ctmCodeULFifoState_ptr,96);
    
    /* Initialization of L1S and l1gtt_main_processing buffers */
    Shortint_fifo_init(global_ul_ptr, 
                       global_ul_fifo_length);
    Shortint_fifo_init(global_dl_ptr, 
                       120);

    init_complete = 1;
  #if (TRACE_TYPE == 1) || (TRACE_TYPE == 4) || (TRACE_TYPE == 5)
    if (trace_info.current_config->l1_dyn_trace & 1<<L1_DYN_TRACE_GTT)
    {
        GTT_send_trace_cpy("GTT background initialized.\n\r");
    }
  #endif /* End trace */

  #if((TRACE_TYPE == 1) || (TRACE_TYPE == 4))    
    sprintf(str,"GTT INIT END %ld \n\r",l1s.actual_time.fn_mod42432); 
    rvt_send_trace_cpy ((T_RVT_BUFFER)str, trace_info.l1_trace_user_id,
        (T_RVT_MSG_LG)strlen(str), RVT_ASCII_FORMAT);
  #endif
    

  }

  /*-------------------------------------------------------*/
  /* l1gtt_exit()                                          */ 
  /*-------------------------------------------------------*/
  /*                                                       */
  /* Description:                                          */
  /* ------------                                          */
  /* The purpose of this function is to free the           */
  /* dynamically allocated buffers in the CTM              */
  /* initialization                                        */
  /*-------------------------------------------------------*/
  void l1gtt_exit()
  {
      char str[64];
     
      #if((TRACE_TYPE == 1) || (TRACE_TYPE == 4))      
        sprintf(str,"GTT EXIT START %ld \n\r",l1s.actual_time.fn_mod42432); 
        rvt_send_trace_cpy ((T_RVT_BUFFER)str, trace_info.l1_trace_user_id,
          (T_RVT_MSG_LG)strlen(str), RVT_ASCII_FORMAT);        
      #endif
      
      /* Call the CTM transmitter and receiver shutdown routine. */
      Shutdown_ctm_transmitter(tx_state_ptr);
      Shutdown_ctm_receiver(rx_state_ptr);

      /* free the fifo buffers used in CTM processing */
      Shortint_fifo_exit(signalFifoState_ptr); 
      Shortint_fifo_exit(ctmOutTTYCodeFifoState_ptr);
      Shortint_fifo_exit(ctmToBaudotFifoState_ptr); 
      Shortint_fifo_exit(baudotToCtmFifoState_ptr);
      Shortint_fifo_exit(baudotCodeFifoState_ptr);
      Shortint_fifo_exit(ctmCodeULFifoState_ptr);
    
      /* Free the fifo buffers used by L1S and l1gtt_main_processing */
      Shortint_fifo_exit(global_ul_ptr);
      Shortint_fifo_exit(global_dl_ptr);
      init_complete = 0;

  #if (TRACE_TYPE == 1) || (TRACE_TYPE == 4) || (TRACE_TYPE == 5)
    if (trace_info.current_config->l1_dyn_trace & 1<<L1_DYN_TRACE_GTT)
    {
        GTT_send_trace_cpy("GTT background terminated.\n\r");
    }
  #endif /* End trace */

  #if (OP_L1_STANDALONE == 0)
    #if ((LOCOSTO_LITE == 1) && (BAE_STATE == 1))

      os_DeallocateMemory(gsp_get_taskid(), 
              (T_VOID_STRUCT *)tx_state_ptr );
      os_DeallocateMemory(gsp_get_taskid(), 
              (T_VOID_STRUCT *) rx_state_ptr );
      os_DeallocateMemory(gsp_get_taskid(), 
              (T_VOID_STRUCT *) baudot_encode_state_ptr );
      os_DeallocateMemory(gsp_get_taskid(), 
              (T_VOID_STRUCT *) signalFifoState_ptr );
      os_DeallocateMemory(gsp_get_taskid(), 
              (T_VOID_STRUCT *) ctmOutTTYCodeFifoState_ptr );
      os_DeallocateMemory(gsp_get_taskid(), 
              (T_VOID_STRUCT *) baudotToCtmFifoState_ptr );
      os_DeallocateMemory(gsp_get_taskid(), 
              (T_VOID_STRUCT *) ctmToBaudotFifoState_ptr );
      os_DeallocateMemory(gsp_get_taskid(), 
              (T_VOID_STRUCT *) baudotCodeFifoState_ptr );
      os_DeallocateMemory(gsp_get_taskid(), 
              (T_VOID_STRUCT *) ctmCodeULFifoState_ptr );
      os_DeallocateMemory(gsp_get_taskid(), 
              (T_VOID_STRUCT *) global_ul_ptr );
      os_DeallocateMemory(gsp_get_taskid(), 
              (T_VOID_STRUCT *) global_dl_ptr );

      #if (TRACE_TYPE == 1) || (TRACE_TYPE == 4) || (TRACE_TYPE == 5)
        if (trace_info.current_config->l1_dyn_trace & 1<<L1_DYN_TRACE_GTT)
        {
            GTT_send_trace_cpy("GTT Mem deallocated.\n\r");
        }
      #endif /* End trace */
    #endif
  #endif    
  #if((TRACE_TYPE == 1) || (TRACE_TYPE == 4))        
    sprintf(str,"GTT EXIT END %ld \n\r",l1s.actual_time.fn_mod42432); 
    rvt_send_trace_cpy ((T_RVT_BUFFER)str, trace_info.l1_trace_user_id,
        (T_RVT_MSG_LG)strlen(str), RVT_ASCII_FORMAT);                
  #endif

  }

  /*-------------------------------------------------------*/
  /* l1gtt_init_trace()                                    */
  /*-------------------------------------------------------*/
  /*                                                       */
  /* Parameters :                                          */
  /*                                                       */
  /* Return     :                                          */
  /*                                                       */
  /* Description : Registers GTT trace to RVT.             */
  /*                                                       */
  /*-------------------------------------------------------*/
  void l1gtt_init_trace(void)
  {
    #if (TRACE_TYPE == 1) || (TRACE_TYPE == 4)
      rvt_register_id("GTTBACK", &trace_info.gtt_trace_user_id, (RVT_CALLBACK_FUNC)NULL);
    #endif
  }

#endif


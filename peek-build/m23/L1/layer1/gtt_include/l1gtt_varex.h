/************* Revision Controle System Header *************
 *                  GSM Layer 1 software 
 * L1GTT_VAREX.H
 *
 *        Filename %M%
 *  Copyright 2003 (C) Texas Instruments  
 *
 ************* Revision Controle System Header *************/


#if (L1_GTT == 1)

  #ifdef L1GTT_ASYNC_C
    // Every BUFFER are associated with a variable
    T_TTY_FIFO_struct *TTY_FIFO_1;
    T_TTY_FIFO_struct *TTY_FIFO_2;
    T_TTY_FIFO_struct *TTY_FIFO_3;
    T_TTY_FIFO_struct *TTY_FIFO_4;

#if (OP_L1_STANDALONE == 1) || (LOCOSTO_LITE ==0) || (BAE_STATE == 0)    
    /* state variables for CTM transmitter and CTM receiver,respectively */
    tx_state_t   tx_state_struct;
    rx_state_t   rx_state_struct;
#endif     

    tx_state_t   *tx_state_ptr;
    rx_state_t   *rx_state_ptr;    
  
#if (OP_L1_STANDALONE == 1) || (LOCOSTO_LITE==0) || (BAE_STATE == 0)    
    /* State variables for the Baudot decoder and encoder,respectively. */
    baudot_encode_state_t baudot_encode_state_struct;
#endif    
    baudot_encode_state_t *baudot_encode_state_ptr;    
 
 
    /* Define fifo state variables */
    WORD16        global_ul_fifo_length = 60;
#if (OP_L1_STANDALONE == 1) || (LOCOSTO_LITE==0) || (BAE_STATE == 0)    
    fifo_state_t  signalFifoState_struct;
    fifo_state_t  ctmOutTTYCodeFifoState_struct;
    fifo_state_t  baudotToCtmFifoState_struct;
    fifo_state_t  ctmToBaudotFifoState_struct;
#endif    
    
    fifo_state_t  *signalFifoState_ptr;
    fifo_state_t  *ctmOutTTYCodeFifoState_ptr;
    fifo_state_t  *baudotToCtmFifoState_ptr;
    fifo_state_t  *ctmToBaudotFifoState_ptr;

    WORD16     cntFramesSinceBurstInit          = 0;
    WORD16     cntSamplesSinceLastBypassFromCTM = 0;
    // Adding the below variable as in TTY2.x implementation, CTM modulator is within DSP
    // and there is no easy way to synch up between MCU and DSP
    WORD16     cntSamplesSinceLastCTMFromBypass = 0;
    WORD16     cntTransmittedEnquiries          = 0;
    BOOL       ctmFromFarEndDetected     = false;
    BOOL       ctmCharacterTransmitted   = false;
    BOOL       enquiryFromFarEndDetected = false;
    BOOL       ctmTransmitterIsIdle      = true;
    BOOL       earlyMutingRequired       = false;
    BOOL       transparentMode           = false;

    /* Used to keep track of time since last Enquiry burst. 
     * Currently 1 cycle == 120 ms */
    WORD16     cyclesSinceLastEnquiryBurst      = 0; 

#if (OP_L1_STANDALONE == 1) || (LOCOSTO_LITE==0) || (BAE_STATE == 0)    
    /* Stores the output from baudot encoder in downlink */
    fifo_state_t  baudotCodeFifoState_struct;   
#endif    
    fifo_state_t  *baudotCodeFifoState_ptr;    

#if (OP_L1_STANDALONE == 1) || (LOCOSTO_LITE==0) || (BAE_STATE == 0)   
    /* Store the output from CTM transmitter in uplink */
    fifo_state_t  ctmCodeULFifoState_struct;    
#endif    
    fifo_state_t  *ctmCodeULFifoState_ptr;     

    WORD16 cntSamplesSinceEnquiryDetected = 0;

#if (OP_L1_STANDALONE == 1) || (LOCOSTO_LITE==0) || (BAE_STATE == 0)   
    /* Shared global buffers between L1S and l1gtt_main_processing. */
    fifo_state_t  global_ul_struct;
    fifo_state_t  global_dl_struct;
#endif    
    
    fifo_state_t  *global_ul_ptr;
    fifo_state_t  *global_dl_ptr;

    int init_complete = 0;
  #else
    extern T_TTY_FIFO_struct  *TTY_FIFO_1;
    extern T_TTY_FIFO_struct  *TTY_FIFO_2;
    extern T_TTY_FIFO_struct  *TTY_FIFO_3;
    extern T_TTY_FIFO_struct  *TTY_FIFO_4;
    extern tx_state_t   *tx_state_ptr;
    extern rx_state_t   *rx_state_ptr;
    extern baudot_encode_state_t *baudot_encode_state_ptr;
    extern WORD16        global_ul_fifo_length;
    extern fifo_state_t  *signalFifoState_ptr;
    extern fifo_state_t  *ctmOutTTYCodeFifoState_ptr;
    extern fifo_state_t  *baudotToCtmFifoState_ptr;
    extern fifo_state_t  *ctmToBaudotFifoState_ptr;
    extern WORD16     cntFramesSinceBurstInit;
    extern WORD16     cntSamplesSinceLastBypassFromCTM;
    extern WORD16     cntSamplesSinceLastCTMFromBypass;    
    extern WORD16     cntTransmittedEnquiries;
    extern BOOL       ctmFromFarEndDetected;
    extern BOOL       ctmCharacterTransmitted;
    extern BOOL       enquiryFromFarEndDetected;
    extern BOOL       ctmTransmitterIsIdle;
    extern BOOL       earlyMutingRequired;
    extern BOOL       transparentMode;
    extern WORD16     cyclesSinceLastEnquiryBurst; 
    extern fifo_state_t  *baudotCodeFifoState_ptr;    
    extern fifo_state_t  *ctmCodeULFifoState_ptr;     
    extern WORD16 cntSamplesSinceEnquiryDetected;
    extern fifo_state_t  *global_ul_ptr;
    extern fifo_state_t  *global_dl_ptr;
    extern int init_complete;

    extern tx_state_t    tx_state_struct;
    extern rx_state_t    rx_state_struct;
    extern baudot_encode_state_t baudot_encode_state_struct;
    extern fifo_state_t  signalFifoState_struct;
    extern fifo_state_t  ctmOutTTYCodeFifoState_struct;
    extern fifo_state_t  baudotToCtmFifoState_struct;
    extern fifo_state_t  ctmToBaudotFifoState_struct;
    extern fifo_state_t  baudotCodeFifoState_struct;    
    extern fifo_state_t  ctmCodeULFifoState_struct;     
    extern fifo_state_t  global_ul_struct;
    extern fifo_state_t  global_dl_struct;
    
  #endif

#endif

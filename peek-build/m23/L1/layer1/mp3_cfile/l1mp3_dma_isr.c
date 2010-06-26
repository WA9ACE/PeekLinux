/************* Revision Controle System Header *************
 *                  GSM Layer 1 software
 * L1mp3_DMA_ISR.C
 *
 *        Filename l1mp3_dma_isr.c
 *  Copyright 2004 (C) Texas Instruments
 *
 ************* Revision Controle System Header *************/

#if (L1_MP3_SIX_BUFFER == 1)
#include "nucleus.h"
#include "l1_confg.h"
#include "l1_types.h"
#include "sys_types.h"
#include "cust_os.h"
#if (AUDIO_TASK == 1)
  #include "l1audio_signa.h"
  #include "l1audio_const.h"
  #include "l1audio_cust.h"
  #include "l1audio_defty.h"
#endif
#include "l1_const.h"
#if TESTMODE
  #include "l1tm_defty.h"
#endif
#if (L1_GTT == 1)
  #include "l1gtt_const.h"
  #include "l1gtt_defty.h"
#endif
#if (OP_RIV_AUDIO == 1)
  #include "rv/rv_general.h"
#endif
#if (L1_DYN_DSP_DWNLD == 1)
  #include "l1_dyn_dwl_const.h"
  #include "l1_dyn_dwl_defty.h"
#endif
#if (L1_MP3 == 1)
  #include "l1mp3_defty.h"
  #include "l1mp3_msgty.h"
  #include "l1mp3_const.h"
  #include "l1mp3_signa.h"
  #include "l1mp3_error.h"
  #include "l1mp3_proto.h"
#endif
#if (L1_MIDI == 1)
  #include "l1midi_defty.h"
#endif
#if (L1_AAC == 1)
  #include "l1aac_defty.h"
#endif
#include "l1_defty.h"
#include "l1_varex.h"
#include "l1_trace.h"
#include "sys_dma.h"
#include "l1_api_hisr.h"

#if (L1_MP3 == 1)
UWORD16 dma_it_occurred=0;
UWORD16 mp3_dma_csr;
#if (CODE_VERSION == NOT_SIMULATION)
/*----------------------------------------------------------*/
/* l1mp3_dma_it_handler()                                   */
/*----------------------------------------------------------*/
/*                                                          */
/* Parameters : dma_status                                  */
/*                                                          */
/* Return     : none                                        */
/*                                                          */
/* Description :                                            */
/* This function is used to handle a DMA interrupt          */
/* that will notify that a frame of decoded mp3 output is   */
/* played out and a new frame has started playback.         */
/*                                                          */
/*----------------------------------------------------------*/
void l1mp3_dma_it_handler(SYS_UWORD16 d_dma_channel_it_status)
{

  // Store the DMA CSR register value in the shared variable
 mp3_dma_csr=(UWORD16)d_dma_channel_it_status;
 dma_it_occurred=1;
  // Generate API interrupt
  #if (OP_RIV_AUDIO == 1)
          {
            // WARNING: temporary until os_activate_hisr() is declared in L3 functions
            extern NU_HISR apiHISR;
            NU_Activate_HISR(&apiHISR);
          }
#else
          os_activate_hisr(API_HISR);
#endif
//}

}
#endif    // CODE_VERSION
#endif    // L1_MP3
#endif    //#if (L1_MP3_SIX_BUFFER == 1)


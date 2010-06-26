/************* Revision Controle System Header *************
 *                  GSM Layer 1 software
 * L1AAC_DMA_ISR.C
 *
 *        Filename l1aac_dma_isr.c
 *  Copyright 2004 (C) Texas Instruments
 *
 ************* Revision Controle System Header *************/
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
#if (L1_AAC == 1)
  #include "l1aac_defty.h"
  #include "l1aac_msgty.h"
  #include "l1aac_const.h"
  #include "l1aac_signa.h"
  #include "l1aac_error.h"
  #include "l1aac_proto.h"
#endif
#if (L1_MIDI == 1)
  #include "l1midi_defty.h"
#endif
#if (L1_MP3 == 1)
  #include "l1mp3_defty.h"
#endif
#include "l1_rf61.h"
#include "l1_defty.h"
#include "l1_varex.h"
#include "l1_trace.h"
#include "sys_dma.h"
#include "l1_api_hisr.h"

#if (L1_AAC == 1)

// AAC NDB API
extern T_AAC_MCU_DSP *aac_ndb;

#if (CODE_VERSION == NOT_SIMULATION)
/*----------------------------------------------------------*/
/* l1aac_dma_it_handler()                                   */
/*----------------------------------------------------------*/
/*                                                          */
/* Parameters : dma_status                                  */
/*                                                          */
/* Return     : none                                        */
/*                                                          */
/* Description :                                            */
/* This function is used to handle a DMA interrupt          */
/* that will notify that a frame of decoded AAC output is   */
/* played out and a new frame has started playback.         */
/*                                                          */
/*----------------------------------------------------------*/
void l1aac_dma_it_handler(SYS_UWORD16 d_dma_channel_it_status)
{
  // Store the DMA CSR register value in the shared variable
  l1_apihisr.aac.dma_csr=(UWORD16)d_dma_channel_it_status;

  // Set the DMA IT occured flag in API memory
  aac_ndb->d_aac_api_dma_it_occured=(API)1;

  // Generate API interrupt
  l1_trigger_api_interrupt();

}
#endif    // CODE_VERSION

#endif    // L1_AAC

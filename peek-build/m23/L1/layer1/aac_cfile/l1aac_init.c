/************* Revision Controle System Header *************
 *                  GSM Layer 1 software
 * L1AAC_INIT.C
 *
 *        Filename l1aac_init.c
 *  Copyright 2004 (C) Texas Instruments
 *
 ************* Revision Controle System Header *************/
#include "l1_confg.h"
#include "nucleus.h"
#include "sys_types.h"
#include "l1_types.h"
#if (AUDIO_TASK == 1)
  #include "l1audio_cust.h"
  #include "l1audio_defty.h"
  #include "l1audio_const.h"
  #include "l1audio_signa.h"
#endif
#include "l1_const.h"
#if TESTMODE
  #include "l1tm_defty.h"
#endif
#if (L1_GTT == 1)
  #include "l1gtt_const.h"
  #include "l1gtt_defty.h"
#endif
#if (L1_DYN_DSP_DWNLD == 1)
  #include "l1_dyn_dwl_const.h"
  #include "l1_dyn_dwl_defty.h"
#endif
#if (L1_AAC == 1)
  #include "l1aac_const.h"
  #include "l1aac_error.h"
  #include "l1aac_defty.h"
#endif
#if (L1_MIDI == 1)
  #include "l1midi_defty.h"
#endif
#if (L1_MP3 == 1)
  #include "l1mp3_defty.h"
#endif
#if (RF_FAM == 61)
#include "l1_rf61.h"
#endif 
#include "l1_defty.h"
#include "cust_os.h"
#include "l1_varex.h"
#include "l1_macro.h"
#include "l1_trace.h"

#if (L1_AAC == 1)

extern T_AAC_MCU_DSP *aac_ndb;

/*-------------------------------------------------------*/
/* l1aac_initialize_var()                                */
/*-------------------------------------------------------*/
/* Parameters :                                          */
/* Return     :                                          */
/* Functionality :                                       */
/*  Initialize AAC variables                             */
/*-------------------------------------------------------*/
void l1aac_initialize_var()
{
  // init AAC NDB
  aac_ndb=(T_AAC_MCU_DSP *)API_address_dsp2mcu(C_AAC_API_BASE_ADDRESS);
#if (CODE_VERSION == SIMULATION)
  {
    extern T_AAC_MCU_DSP aac_ndb_sim;
    aac_ndb=&aac_ndb_sim;
  }
#endif

  // init AAC DSP background
  l1s_dsp_com.dsp_ndb_ptr->a_background_tasks[C_BGD_AAC] = (API)((C_BGD_AAC<<11) | 1);
  if(l1s_dsp_com.dsp_ndb_ptr->d_max_background<(C_BGD_AAC+1))
    l1s_dsp_com.dsp_ndb_ptr->d_max_background=(API)(C_BGD_AAC+1);
}


/*-------------------------------------------------------*/
/* l1aac_reset()                                         */
/*-------------------------------------------------------*/
/* Parameters : none                                     */
/* Return     : nothing                                  */
/* Functionality :                                       */
/*  Reset AAC variables (bar the ones located in API)    */
/*-------------------------------------------------------*/
void l1aac_reset()
{
  // Reset L1A-L1S commands
  l1a_l1s_com.aac_task.command.start           =FALSE;
  l1a_l1s_com.aac_task.command.stop            =FALSE;

  // Reset L1A-API HISR commands
  l1a_apihisr_com.aac.command.start            =FALSE;
  l1a_apihisr_com.aac.command.stop             =FALSE;
  l1a_apihisr_com.aac.command.pause            =FALSE;
  l1a_apihisr_com.aac.command.resume           =FALSE;
  l1a_apihisr_com.aac.command.restart          =FALSE;

  // Reset AAC API HISR variables
  l1_apihisr.aac.state                         =0;
  l1_apihisr.aac.on_pause                      =FALSE;
  l1_apihisr.aac.error_code                    =C_AAC_ERR_NONE;
  l1_apihisr.aac.dma_csr                       =0;
  l1_apihisr.aac.wait_data                     =FALSE;
}

/*-------------------------------------------------------*/
/* l1aac_reset_api()                                     */
/*-------------------------------------------------------*/
/* Parameters : none                                     */
/* Return     : nothing                                  */
/* Functionality :                                       */
/*  Reset API AAC variables                              */
/*-------------------------------------------------------*/
void l1aac_reset_api()
{
  // init L1S-DSP communication structure
  aac_ndb->d_aac_api_dma_it_occured     =(API)0;
  aac_ndb->d_aac_api_init               =(API)0;
  aac_ndb->d_aac_api_play               =(API)0;
  aac_ndb->d_aac_api_pause              =(API)0;
  aac_ndb->d_aac_api_restart            =(API)0;
  aac_ndb->d_aac_api_stop               =(API)0;
  aac_ndb->d_aac_api_end                =(API)0;
  aac_ndb->d_aac_api_request_index      =(API)0;
  aac_ndb->d_aac_api_request_size       =(API)0;
  aac_ndb->d_aac_api_provided_size      =(API)0;
  aac_ndb->d_aac_api_error_code[0]      =(API)0;
  aac_ndb->d_aac_api_error_code[1]      =(API)0;
  aac_ndb->d_aac_api_aac_format         =(API)0;
  aac_ndb->d_aac_api_frequency_index    =(API)0;
  aac_ndb->d_aac_api_bitrate[0]         =(API)0;
  aac_ndb->d_aac_api_bitrate[1]         =(API)0;
  aac_ndb->d_aac_api_channels           =(API)0;
}

#endif  // L1_AAC

/************* Revision Controle System Header *************
 *                  GSM Layer 1 software
 * L1MP3_INIT.C
 *
 *        Filename l1mp3_init.c
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
#if (L1_MP3 == 1)
  #include "l1mp3_const.h"
  #include "l1mp3_error.h"
  #include "l1mp3_defty.h"
#endif
#if (L1_MIDI == 1)
  #include "l1midi_defty.h"
#endif
#include "l1_defty.h"
#include "cust_os.h"
#include "l1_varex.h"
#include "l1_macro.h"
#include "l1_trace.h"

#if (L1_MP3 == 1)

extern T_MP3_MCU_DSP *mp3_ndb;

/*-------------------------------------------------------*/
/* l1mp3_initialize_var()                                */
/*-------------------------------------------------------*/
/* Parameters :                                          */
/* Return     :                                          */
/* Functionality :                                       */
/*  Initialize MP3 variables                             */
/*-------------------------------------------------------*/
void l1mp3_initialize_var()
{
  // init MP3 NDB
  mp3_ndb=(T_MP3_MCU_DSP *)API_address_dsp2mcu(C_MP3_API_BASE_ADDRESS);
#if (CODE_VERSION == SIMULATION)
  {
    extern T_MP3_MCU_DSP mp3_ndb_sim;
    mp3_ndb=&mp3_ndb_sim;
  }
#endif

  // init MP3 DSP background
  l1s_dsp_com.dsp_ndb_ptr->a_background_tasks[C_BGD_MP3] = (API)((C_BGD_MP3<<11) | 1);
  if(l1s_dsp_com.dsp_ndb_ptr->d_max_background<(C_BGD_MP3+1))
    l1s_dsp_com.dsp_ndb_ptr->d_max_background=(API)(C_BGD_MP3+1);
}


/*-------------------------------------------------------*/
/* l1mp3_reset()                                         */
/*-------------------------------------------------------*/
/* Parameters : none                                     */
/* Return     : nothing                                  */
/* Functionality :                                       */
/*  Reset MP3 variables (bar the ones located in API)    */
/*-------------------------------------------------------*/
void l1mp3_reset()
{
  // Reset L1A-L1S commands
  l1a_l1s_com.mp3_task.command.start           =FALSE;
  l1a_l1s_com.mp3_task.command.stop            =FALSE;

  // Reset L1A-API HISR commands
  l1a_apihisr_com.mp3.command.start            =FALSE;
  l1a_apihisr_com.mp3.command.stop             =FALSE;
  l1a_apihisr_com.mp3.command.pause            =FALSE;
  l1a_apihisr_com.mp3.command.resume           =FALSE;
  l1a_apihisr_com.mp3.command.restart          =FALSE;

  // Reset MP3 API HISR variables
  l1_apihisr.mp3.state                         =0;
  l1_apihisr.mp3.on_pause                      =FALSE;
  l1_apihisr.mp3.error_code                    =C_MP3_ERR_NONE;
  l1_apihisr.mp3.wait_data                     =FALSE;
}

/*-------------------------------------------------------*/
/* l1mp3_reset_api()                                     */
/*-------------------------------------------------------*/
/* Parameters : none                                     */
/* Return     : nothing                                  */
/* Functionality :                                       */
/*  Reset API MP3 variables                              */
/*-------------------------------------------------------*/
void l1mp3_reset_api()
{
  // init L1S-DSP communication structure
  mp3_ndb->d_mp3_api_header[0]    =(API)0;
  mp3_ndb->d_mp3_api_header[1]    =(API)0;
  mp3_ndb->d_mp3_api_channel      =(API)0;
  mp3_ndb->d_mp3_api_init         =(API)0;
  mp3_ndb->d_mp3_api_play         =(API)0;
  mp3_ndb->d_mp3_api_pause        =(API)0;
  mp3_ndb->d_mp3_api_restart      =(API)0;
  mp3_ndb->d_mp3_api_stop         =(API)0;
  mp3_ndb->d_mp3_api_end          =(API)0;
  mp3_ndb->d_mp3_api_request_index=(API)0;
  mp3_ndb->d_mp3_api_request_size =(API)0;
  mp3_ndb->d_mp3_api_provided_size=(API)0;
  mp3_ndb->d_mp3_api_error_code   =(API)0;
#if (L1_MP3_SIX_BUFFER == 1)  
  mp3_ndb->d_mp3_ver_id           =(API)0;
  mp3_ndb->d_mp3_out_dma_it_occured =(API)0;
#endif 
}

#endif  // L1_MP3

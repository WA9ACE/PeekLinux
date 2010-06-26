/************* Revision Controle System Header *************
 *                  GSM Layer 1 software
 * L1AAC_SYNC.C
 *
 *        Filename l1aac_sync.c
 *  Copyright 2004 (C) Texas Instruments
 *
 ************* Revision Controle System Header *************/
#include "l1_confg.h"
#if (L1_AAC == 1)
  #include "nucleus.h"
#endif //omaps00090550
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
#if (L1_DYN_DSP_DWNLD == 1)
  #include "l1_dyn_dwl_const.h"
  #include "l1_dyn_dwl_defty.h"
#endif
#if (L1_AAC == 1)
  #include "l1aac_defty.h"
  #include "l1aac_signa.h"
  #include "l1aac_const.h"
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
#include "l1_varex.h"
#include "l1_macro.h"
#include "l1_api_hisr.h"


#if (L1_AAC == 1)

extern NU_HISR apiHISR;
extern UWORD8 Cust_get_next_buffer_status(void);

/*-------------------------------------------------------*/
/* l1s_aac_manager()                                     */
/*-------------------------------------------------------*/
/*                                                       */
/* Parameters :  n/a                                     */
/*                                                       */
/* Return     :  n/a                                     */
/*                                                       */
/* Description : AAC L1S manager task.                   */
/*                                                       */
/*-------------------------------------------------------*/
void l1s_aac_manager(void)
{
  enum states
  {
    IDLE = 0,
    WAIT_TDMA,
    WAIT_STOP
  };

  UWORD8 *state = &l1s.audio_state[L1S_AAC_STATE];
  xSignalHeaderRec *conf_msg;

  switch(*state)
  {
    case IDLE:
    {
      if(l1a_l1s_com.aac_task.command.start==TRUE)
      {
        // make sure the DSP is able to receive API interrupt
        // for this, we wait 1 TDMA frame
        // change state

        *state=WAIT_TDMA;
      }
    }
    break;

    case WAIT_TDMA:
    {
      // reset the command
      l1a_l1s_com.aac_task.command.start=FALSE;
       // Reset API variables
       l1aac_reset_api();

      // send start background to the DSP
      l1s_dsp_com.dsp_ndb_ptr->d_background_enable|=(API)(1<<C_BGD_AAC);
      l1_trigger_api_interrupt();

      // send confirmation to the L1A
      conf_msg=os_alloc_sig(0);
      DEBUGMSG(status,NU_ALLOC_ERR)
      conf_msg->SignalCode=L1_AAC_ENABLE_CON;
      os_send_sig(conf_msg,L1C1_QUEUE);
      DEBUGMSG(status,NU_SEND_QUEUE_ERR)

      // change state
      *state=WAIT_STOP;
    }
    break;

    case WAIT_STOP:
    {

      UWORD8 status = Cust_get_next_buffer_status();
	  if((l1_apihisr.aac.wait_data) && (!status))
      {
        //Activate the API HISR
        NU_Activate_HISR(&apiHISR);
        l1_apihisr.aac.wait_data = FALSE;
      }

      if(l1a_l1s_com.aac_task.command.stop==TRUE)
      {
        // reset the command
        l1a_l1s_com.aac_task.command.stop=FALSE;

        // send stop background to the DSP
        l1s_dsp_com.dsp_ndb_ptr->d_background_enable&=(API)(~(1<<C_BGD_AAC));
        l1_trigger_api_interrupt();

        // send confirmation to the L1A
        conf_msg=os_alloc_sig(0);
        DEBUGMSG(status,NU_ALLOC_ERR)
        conf_msg->SignalCode=L1_AAC_DISABLE_CON;
        os_send_sig(conf_msg,L1C1_QUEUE);
        DEBUGMSG(status,NU_SEND_QUEUE_ERR)

        // change state
        *state=IDLE;
      }
    }
    break;
  }
}

#endif    // L1_AAC==1

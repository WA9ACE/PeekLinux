/************* Revision Controle System Header *************
 *                  GSM Layer 1 software
 * L1MP3_SYNC.C
 *
 *        Filename l1mp3_sync.c
 *  Copyright 2004 (C) Texas Instruments
 *
 ************* Revision Controle System Header *************/
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
#if (L1_MP3 == 1)
  #include "l1mp3_defty.h"
  #include "l1mp3_signa.h"
  #include "l1mp3_const.h"
  #include "nucleus.h"
#endif
#if (L1_MIDI == 1)
  #include "l1midi_defty.h"
#endif
#include "l1_defty.h"
#include "l1_varex.h"
#include "l1_macro.h"
#include "l1_api_hisr.h"


#if (L1_MP3 == 1)

extern NU_HISR apiHISR;
extern UWORD8 Cust_get_next_buffer_status(void);

#if(L1_BT_AUDIO ==1)
      extern T_L1_BT_AUDIO bt_audio;
extern UWORD16 dma_it_occurred;
 #endif
/*-------------------------------------------------------*/
/* l1s_mp3_manager()                                     */
/*-------------------------------------------------------*/
/*                                                       */
/* Parameters :  n/a                                     */
/*                                                       */
/* Return     :  n/a                                     */
/*                                                       */
/* Description : MP3 L1S manager task.                   */
/*                                                       */
/*-------------------------------------------------------*/
void l1s_mp3_manager(void)
{
  enum states
  {
    IDLE = 0,
    WAIT_TDMA,
    WAIT_STOP
  };

  UWORD8 *state = &l1s.audio_state[L1S_MP3_STATE];
  xSignalHeaderRec *conf_msg;

  switch(*state)
  {
    case IDLE:
    {
      if(l1a_l1s_com.mp3_task.command.start==TRUE)
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
      l1a_l1s_com.mp3_task.command.start=FALSE;
       // Reset API variables
       l1mp3_reset_api();

      // send start background to the DSP
      l1s_dsp_com.dsp_ndb_ptr->d_background_enable|=(API)(1<<C_BGD_MP3);
      l1_trigger_api_interrupt();

      // send confirmation to the L1A
      conf_msg=os_alloc_sig(0);
      DEBUGMSG(status,NU_ALLOC_ERR)
      conf_msg->SignalCode=L1_MP3_ENABLE_CON;
      os_send_sig(conf_msg,L1C1_QUEUE);
      DEBUGMSG(status,NU_SEND_QUEUE_ERR)

      // change state
      *state=WAIT_STOP;
    }
    break;

    case WAIT_STOP:
    {


      UWORD8 status = Cust_get_next_buffer_status();
	  if((l1_apihisr.mp3.wait_data) && (!status))
      {
	#if(L1_BT_AUDIO ==1)
	 if(bt_audio.connected_status == TRUE)
	   {
	   	dma_it_occurred=1;
	       l1_apihisr.mp3.dsp_hint_occurred=1; 
	 }
	#endif
        //Activate the API HISR
        NU_Activate_HISR(&apiHISR);
        l1_apihisr.mp3.wait_data = FALSE;
      }


      if(l1a_l1s_com.mp3_task.command.stop==TRUE)
      {
        // reset the command
        l1a_l1s_com.mp3_task.command.stop=FALSE;

        // send stop background to the DSP
        l1s_dsp_com.dsp_ndb_ptr->d_background_enable&=(API)(~(1<<C_BGD_MP3));
        l1_trigger_api_interrupt();

        // send confirmation to the L1A
        conf_msg=os_alloc_sig(0);
        DEBUGMSG(status,NU_ALLOC_ERR)
        conf_msg->SignalCode=L1_MP3_DISABLE_CON;
        os_send_sig(conf_msg,L1C1_QUEUE);
        DEBUGMSG(status,NU_SEND_QUEUE_ERR)

        // change state
        *state=IDLE;
      }
    }
    break;
  }
}

#endif    // L1_MP3==1

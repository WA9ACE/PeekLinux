/************* Revision Controle System Header *************
 *                  GSM Layer 1 software 
 * L1GTT_SYNC.C
 *
 *        Filename l1gtt_sync.c
 *  Copyright 2003 (C) Texas Instruments  
 *
 ************* Revision Controle System Header *************/

/************************************/
/* Include files...                 */
/************************************/
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

    #include "ctm_transmitter.h"
    #include "ctm_receiver.h"
    #include "l1gtt_baudot_functions.h"

    #include "l1gtt_const.h"
    #include "l1gtt_defty.h"
    #include "l1gtt_msgty.h"
    #include "l1gtt_signa.h"
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

    #include "ctm_transmitter.h"
    #include "ctm_receiver.h"
    #include "l1gtt_baudot_functions.h"

    #include "l1gtt_const.h"
    #include "l1gtt_defty.h"
    #include "l1gtt_msgty.h"
    #include "l1gtt_signa.h"
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

    #if (OP_L1_STANDALONE == 0)
      #include "rvf_api.h"
    #endif

    #if L2_L3_SIMUL
      #include "hw_debug.h"
    #endif
  #endif

  /**************************************/
  /* Prototypes for L1 SYNCH manager    */
  /**************************************/

  void l1s_gtt_manager(void);

  /**************************************/
  /* extern Prototypes                  */
  /**************************************/

  extern UWORD16 TTY_fifo_write_buff2(WORD16 *data_exchanged,
                                      UWORD16 max_size);
  extern void    l1g_ctm_check_read(void);
  extern void    l1g_DSP_buffer_writes(void);

  extern T_GTT_DEBUG  l1_gtt_debug;  
  #if (OP_L1_STANDALONE == 0)
    #include "tty_i.h"
    extern T_TTY_ENV_CTRL_BLK *tty_env_ctrl_blk_p;
  #endif

  /*-------------------------------------------------------*/
  /* l1s_gtt_manager()                                     */
  /*-------------------------------------------------------*/
  /*                                                       */
  /* Parameters :                                          */
  /*                                                       */
  /* Return     :                                          */
  /*                                                       */
  /* Description : Global manager of the L1S tty task.     */
  /*                                                       */
  /*-------------------------------------------------------*/
  void l1s_gtt_manager(void)
  {
    enum states 
    {
      RESET              = 0,
      WAIT_GTT_ON        = 1,
      WAIT_GTT_OFF       = 2,
      IN_SPEECH          = 3
    };

    UWORD8            *state      = &l1s.tty_state;
    xSignalHeaderRec  *msg;
    static UWORD8     reset=TRUE;
    static int silence_pad_time = 0;

    switch(*state)
    {
      case RESET:
      {
        // GTT task activated ?
        if (l1a_l1s_com.gtt_task.command.start)
        {
          // Set customizable data in the API
          l1s_dsp_com.dsp_ndb_ptr->d_ctm_detect_shift                  = C_CTM_DETECT_SHIFT;
          l1s_dsp_com.dsp_ndb_ptr->d_tty2x_baudot_mod_amplitude_scale  = C_TTY_MOD_NORM;
          l1s_dsp_com.dsp_ndb_ptr->d_tty2x_samples_per_baudot_stop_bit = C_TTY_NUM_SAMPLES_STOP_BIT;
          l1s_dsp_com.dsp_ndb_ptr->d_ctm_mod_norm                      = C_CTM_MOD_0_25; // 0.25 factor
          l1s_dsp_com.dsp_ndb_ptr->d_tty2x_offset_normalization        = C_TTY_OFFSET_NORM;
          l1s_dsp_com.dsp_ndb_ptr->d_tty2x_threshold_startbit          = C_TTY_THRES_START_BIT;
          l1s_dsp_com.dsp_ndb_ptr->d_tty2x_threshold_diff              = C_TTY_THRES_DIFF;
          l1s_dsp_com.dsp_ndb_ptr->d_tty2x_duration_startdetect        = C_TTY_DURA_START_DETECT;
          l1s_dsp_com.dsp_ndb_ptr->d_tty2x_startbit_thres              = C_TTY_START_BIT_THRES;

          // Start the DSP GTT task
          // Initially do not mute the bypass path until we receive CTM in the downlink
          l1s_dsp_com.dsp_ndb_ptr->d_tty2x_dl_bypass_mute = C_DL_BYPASS_MUTE_DISABLE;

          l1s_dsp_com.dsp_ndb_ptr->d_audio_init  |= B_GTT_START;
          l1s_dsp_com.dsp_ndb_ptr->d_tty_reset_buffer_ul |= ((l1a_l1s_com.gtt_task.baudot_keyboard_rate) << 3);
          
          *state = WAIT_GTT_ON;
        }
      }
      break;

      case WAIT_GTT_ON:
      {
        // The DSP acknowledged the L1S init request by clearing the start bit.
        if (!(l1s_dsp_com.dsp_ndb_ptr->d_audio_init & B_GTT_START))
        {
          // Send the start confirmation message
          // Allocate confirmation message...
          msg = os_alloc_sig(0);
          DEBUGMSG(status,NU_ALLOC_ERR)
          msg->SignalCode = L1_GTT_START_CON;

          // Send confirmation message...
          os_send_sig(msg, L1C1_QUEUE);
          DEBUGMSG(status,NU_SEND_QUEUE_ERR)

          *state = IN_SPEECH;
        }
      }
      break;

      case IN_SPEECH:
      {
        // GTT task disabled ?
        if (l1a_l1s_com.gtt_task.command.stop)
        {
          // Stop the DSP GTT task
          l1s_dsp_com.dsp_ndb_ptr->d_audio_init  |= B_GTT_STOP;

          *state = WAIT_GTT_OFF;
          return;
        }

        // GTT processing
        //---------------

        // Done if speech mode
        if (l1a_l1s_com.dedic_set.aset != NULL)
        {
          // Speech channel enabled ?
        #if (AMR == 1)
          if ((l1a_l1s_com.dedic_set.aset->achan_ptr->mode == TCH_HS_MODE)   ||
              (l1a_l1s_com.dedic_set.aset->achan_ptr->mode == TCH_EFR_MODE)  ||
              (l1a_l1s_com.dedic_set.aset->achan_ptr->mode == TCH_FS_MODE)   ||
              (l1a_l1s_com.dedic_set.aset->achan_ptr->mode == TCH_AFS_MODE)  ||
              (l1a_l1s_com.dedic_set.aset->achan_ptr->mode == TCH_AHS_MODE))
        #else
          if ((l1a_l1s_com.dedic_set.aset->achan_ptr->mode == TCH_HS_MODE)   ||
              (l1a_l1s_com.dedic_set.aset->achan_ptr->mode == TCH_EFR_MODE)  ||
              (l1a_l1s_com.dedic_set.aset->achan_ptr->mode == TCH_FS_MODE))
        #endif
          {
            UWORD8 fn_report_mod13 = l1s.next_time.fn_in_report%13;

          #if L2_L3_SIMUL
            // GTT test

            // Test code: increments periodically "chars_to_push" in CTM transmitter
            // according to the selected rate
            if (l1a_l1s_com.gtt_task.test.scen_enable)
            {
              if (l1s.gtt_test.frame_count++ == (l1a_l1s_com.gtt_task.test.rate))
              {
                l1s.gtt_test.frame_count = 0;
                l1s.gtt_test.chars_to_push++;
              }
            }
          #endif

            if((fn_report_mod13 == 0) && (init_complete != 0)) /* every 13-frame (60ms) */
            {
              /* Check data in shared NDB and read them from NDB to internal buffer */
              l1g_ctm_check_read();

              #if (OP_L1_STANDALONE == 1)
                // Send data indication message to L1GTT background task
                os_set_event(L1GTTBACK_EVENTS, DATA_AVAIL_EVENT);
              #else
                // Send data indication message to Riviera TTY Background Task
                rvf_send_event (tty_env_ctrl_blk_p->addr_id, TTY_DATA_EVENT_MASK);
              #endif

              /* Check data in internal buffer and write them to NDB */
              l1g_DSP_buffer_writes();
              
              l1_gtt_debug.fn_gtt_sync = l1s.actual_time.fn_mod42432;
              l1_gtt_debug.fn_gtt_proc_flag++;              
            }
          } // End if speech channel
        } // End if dedicated mode
      }
      break;

      case WAIT_GTT_OFF:
      {
        // DSP GTT is stopped (stop bit cleared by the DSP to acknowledge)
        if (!(l1s_dsp_com.dsp_ndb_ptr->d_audio_init & B_GTT_STOP))
        {
          // Send the stop confirmation message
          // Allocate confirmation message...
          msg = os_alloc_sig(0);
          DEBUGMSG(status,NU_ALLOC_ERR)
          msg->SignalCode = L1_GTT_STOP_CON;

          // Send confirmation message...
          os_send_sig(msg, L1C1_QUEUE);
          DEBUGMSG(status,NU_SEND_QUEUE_ERR)

          *state = RESET;
        }
      }
      break;

    } // End switch
  }

#endif // L1_GTT

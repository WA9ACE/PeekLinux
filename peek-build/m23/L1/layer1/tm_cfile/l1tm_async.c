/************* Revision Controle System Header *************
 *                  GSM Layer 1 software 
 * L1TM_ASYNC.C
 *
 *        Filename l1tm_async.c
 *  Copyright 2003 (C) Texas Instruments
 *
 ************* Revision Controle System Header *************/

#include "l1_macro.h"
#include "l1_confg.h"

#if TESTMODE
  #define L1TM_ASYNC_C

  #include <string.h>
  #include <stdlib.h>
  #include "l1_types.h"
  #include "sys_types.h"
  #include "l1_const.h"
  #include "l1_time.h"
  #include "l1_signa.h"
  #include "cust_os.h"

  #include "l1tm_defty.h"

  #if (AUDIO_TASK == 1)
    #include "l1audio_const.h"
    #include "l1audio_cust.h"
    #include "l1audio_defty.h"
    #include "l1audio_signa.h"
    #include "l1audio_proto.h"
  #endif

  #if (L1_GTT == 1)
    #include "l1gtt_const.h"
    #include "l1gtt_defty.h"
  #endif

  #if (L1_MP3 == 1)
    #include "l1mp3_defty.h"
  #endif

  #if (L1_MIDI == 1)
    #include "l1midi_defty.h"
  #endif

  #include "l1_defty.h"
  #include "l1_msgty.h"
  #include "l1_varex.h"
  #include "l1_proto.h"

  #include "l1tm_msgty.h"
  #include "l1tm_signa.h"
  #include "l1tm_varex.h"

  #if (L1_STEREOPATH == 1)
    #include "sys_dma.h"
    #include "l1audio_stereo.h"
  #endif

  #if L1_GPRS
    #include "l1p_cons.h"
    #include "l1p_msgt.h"
    #include "l1p_deft.h"
    #include "l1p_vare.h"
    #include "l1p_sign.h"
  #endif

  #ifndef ETM_PROTOCOL
    #error ETM_PROTOCOL is not defined
  #endif

  #ifndef ETM_MODE
    #error ETM_MODE is not defined
  #endif

//------------------------------------
// Prototypes from external functions
//------------------------------------
#if (OP_L1_STANDALONE == 1)
  void etm_core(xSignalHeaderRec *msg);
#endif

  void Cust_tm_init(void);

  void l1tm_stats_fb_confirm        (T_TMODE_FB_CON *prim, WORD32 test);
  void l1tm_stats_sb_confirm        (T_TMODE_NCELL_SYNC_IND *prim, WORD32 test);
  void l1tm_stats_bcch_confirm      (T_TMODE_BCCHS_CON *prim);
  void l1tm_stats_tch_confirm       (T_TMODE_TCH_INFO  *prim);
  void l1tm_stats_mon_confirm       (T_TMODE_FB_CON    *prim);
  void l1tm_stats_full_list_meas_confirm(T_TMODE_RXLEV_REQ *prim);
  BOOL l1tm_is_rx_counter_done      (void);
  void l1tm_reset_rx_state          (void);

  void l1tm_rf_param_write       (T_TESTMODE_PRIM *prim, T_TM_RETURN *tm_return);
  void l1tm_rf_param_read        (T_TESTMODE_PRIM *prim, T_TM_RETURN *tm_return);
  void l1tm_rf_table_write       (T_TESTMODE_PRIM *prim, T_TM_RETURN *tm_return);
  void l1tm_rf_table_read        (T_TESTMODE_PRIM *prim, T_TM_RETURN *tm_return);
  void l1tm_rx_param_write       (T_TESTMODE_PRIM *prim, T_TM_RETURN *tm_return);
  void l1tm_rx_param_read        (T_TESTMODE_PRIM *prim, T_TM_RETURN *tm_return);
  void l1tm_tx_param_write       (T_TESTMODE_PRIM *prim, T_TM_RETURN *tm_return);
  void l1tm_tx_param_read        (T_TESTMODE_PRIM *prim, T_TM_RETURN *tm_return);
  void l1tm_tx_template_write    (T_TESTMODE_PRIM *prim, T_TM_RETURN *tm_return);
  void l1tm_tx_template_read     (T_TESTMODE_PRIM *prim, T_TM_RETURN *tm_return);

  void l1tm_special_param_write  (T_TESTMODE_PRIM *prim, T_TM_RETURN *tm_return);
  void l1tm_special_param_read   (T_TESTMODE_PRIM *prim, T_TM_RETURN *tm_return);
  void l1tm_special_table_write  (T_TESTMODE_PRIM *prim, T_TM_RETURN *tm_return);
  void l1tm_special_table_read   (T_TESTMODE_PRIM *prim, T_TM_RETURN *tm_return);
  void l1tm_special_enable       (T_TESTMODE_PRIM *prim, T_TM_RETURN *tm_return);
  void l1tm_rf_enable            (T_TESTMODE_PRIM *prim, T_TM_RETURN *tm_return);
  void l1tm_stats_config_write   (T_TESTMODE_PRIM *prim, T_TM_RETURN *tm_return);
  void l1tm_stats_config_read    (T_TESTMODE_PRIM *prim, T_TM_RETURN *tm_return);
  void l1tm_statistics           (T_TESTMODE_PRIM *prim, T_TM_RETURN *tm_return);
  void l1tm_stats_read           (T_TM_RETURN *tm_return, WORD16 type, UWORD16 bitmask);
  #if (L1_DRP == 1)
    void l1tm_drp_sw_write(T_TESTMODE_PRIM *prim, T_TM_RETURN *tm_return);
  #endif

  
  void l1a_tmode_send_ul_msg     (T_TM_RETURN *tm_ret);
  UWORD16 l1tm_convert_arfcn2l1ch(UWORD16 arfcn, UWORD8 *error_flag);
  void l1tm_fill_burst           (UWORD16 pattern, UWORD16 *TM_ul_data);
  void l1tm_initialize_var       (void);
  UWORD16 Convert_l1_radio_freq  (SYS_UWORD16 radio_freq);

  void tm_transmit(T_TM_RETURN *tm_ret);

  #if (CODE_VERSION != SIMULATION)
    void l1tm_tpu_table_write      (T_TESTMODE_PRIM *prim, T_TM_RETURN *tm_return);
    void l1tm_tpu_table_read       (T_TESTMODE_PRIM *prim, T_TM_RETURN *tm_return);
  #endif

  #if L1_GPRS
    T_TRANSFER_SET  *l1pa_get_free_transfer_set   (UWORD8  new_tbf);
    T_CRES_LIST_PARAM *l1pa_get_free_cres_list_set(void);
    void l1tm_stats_pdtch_confirm                 (T_TMODE_PDTCH_INFO *prim);
  #endif


  #if (L1_TPU_DEV == 1)
    void l1tm_flexi_tpu_table_read(T_TESTMODE_PRIM * prim, T_TM_RETURN * tm_return);
    void l1tm_flexi_tpu_table_write(T_TESTMODE_PRIM * prim, T_TM_RETURN * tm_return);

//Flexi ABB Delay
    void l1tm_flexi_abb_read(T_TESTMODE_PRIM * prim, T_TM_RETURN * tm_return);
    void l1tm_flexi_abb_write(T_TESTMODE_PRIM * prim, T_TM_RETURN * tm_return);
  #endif

// DRP Calibration
  void l1tm_drp_calib_read(T_TESTMODE_PRIM * prim, T_TM_RETURN * tm_return);
  void l1tm_drp_calib_write(T_TESTMODE_PRIM * prim, T_TM_RETURN * tm_return);

  // Prototypes from internal functions
  //------------------------------------
  void l1a_tmode_fb0_process(xSignalHeaderRec *msg);
  void l1a_tmode_fb1_process(xSignalHeaderRec *msg);
  void l1a_tmode_sb_process(xSignalHeaderRec *msg);
  void l1a_tmode_bcch_reading_process(xSignalHeaderRec *msg);
  void l1a_tmode_dedicated_process(xSignalHeaderRec *msg);
  void l1a_tmode_access_process(xSignalHeaderRec *msg);
  void l1a_tmode_full_list_meas_process(xSignalHeaderRec *msg);

  #if L1_GPRS
    void l1a_tmode_transfer_process(xSignalHeaderRec *msg);
  #endif
  #if ((L1_STEREOPATH == 1) && (OP_L1_STANDALONE == 1))
    void l1a_tmode_audio_stereopath_process(xSignalHeaderRec *msg);
    extern void l1tm_stereopath_DMA_handler(SYS_UWORD16 dma_status);
    extern void l1tm_stereopath_fill_buffer(void* buffer_address);
    extern UWORD16 l1tm_stereopath_get_pattern(UWORD16 sampling_freq, UWORD16 sin_freq_left,UWORD16 sin_freq_right, UWORD8 data_type);
    extern void l1a_audio_send_confirmation(UWORD32 SignalCode);
    extern void l1_trigger_api_interrupt(void);
#if ( ANLG_FAM == 11)
    //Add the two new sampling frequencies in the test mode for Locosto - 12khz and 24 khz
    const UWORD16 l1tm_stereopath_sampling_freqs[9] = {8000,11025,12000,16000,22050,24000,32000,44100,48000};
#else
    const UWORD16 l1tm_stereopath_sampling_freqs[8] = {48000,0,44100,32000,22050,16000,11025,8000}; 
#endif
    const UWORD16 l1tm_stereopath_sin_freqs[4][2] = {{0,0},
                                                    {100,1000},
                                                    {1000,10000},
                                                    {1000,1000}};   // 4 different pattern of two freqs
    const UWORD16 l1tm_stereopath_buffer[(480+1)*2];

    T_STP_DRV_MCU_DSP *stp_drv_ndb = (T_STP_DRV_MCU_DSP *)API_address_dsp2mcu(C_STP_DRV_API_BASE_ADDRESS);

#if (CODE_VERSION == NOT_SIMULATION)
  #pragma DATA_SECTION(TM_stereo_buf,".TM_stereo_buf");
#endif

#if (CHIPSET == 15)
  #pragma DATA_SECTION(TM_stereo_buf_ext_mem, ".TM_stereo_buf_ext_mem");
#endif
    volatile WORD16 TM_stereo_buf[STEREOPATH_MAX_NB_OF_FRAMES*2];
#if (CHIPSET == 15)
    volatile WORD16 TM_stereo_buf_ext_mem[STEREOPATH_MAX_NB_OF_FRAMES*2];
#endif
  #endif // ((L1_STEREOPATH == 1) && (OP_L1_STANDALONE == 1))

/*****************************************************************************
 * etm_rf()                                                                  
 *                                                                           
 * Description: Processing of actions from orders coming from etm_rf PC DLL  
 * When RF_TRANSACTION is called at the ETM PC side. The Messages sent in 
 * the primitive are  processed by this function.
 *****************************************************************************/

void etm_rf(xSignalHeaderRec *msg) {
  T_TESTMODE_PRIM   *prim; 
  T_TM_RETURN       tm_ret;

  // use CID to decide what to do
  prim = (T_TESTMODE_PRIM *)(msg->SigP);

  #if (ETM_PROTOCOL == 1)
    // fill in the mid
    tm_ret.mid = prim->mid;
  #endif

    // fill in the cid also named fid in the ETM protocol
  tm_ret.cid = prim->cid;

  switch (prim->cid)
  {
    case RF_ENABLE:                  l1tm_rf_enable(prim, &tm_ret);                    break;
    case STATS_READ:                 l1tm_statistics(prim, &tm_ret);                   break;
    case STATS_CONFIG_WRITE:         l1tm_stats_config_write(prim, &tm_ret);           break;
    case STATS_CONFIG_READ:          l1tm_stats_config_read(prim, &tm_ret);            break;
    case RF_PARAM_WRITE:             l1tm_rf_param_write(prim, &tm_ret);               break;
    case RF_PARAM_READ:              l1tm_rf_param_read(prim, &tm_ret);                break;
    case RF_TABLE_WRITE:             l1tm_rf_table_write(prim, &tm_ret);               break;
    case RF_TABLE_READ:              l1tm_rf_table_read(prim, &tm_ret);                break;
    case RX_PARAM_WRITE:             l1tm_rx_param_write(prim, &tm_ret);               break;
    case RX_PARAM_READ:              l1tm_rx_param_read(prim, &tm_ret);                break;
    case TX_PARAM_WRITE:             l1tm_tx_param_write(prim, &tm_ret);               break;
    case TX_PARAM_READ:              l1tm_tx_param_read(prim, &tm_ret);                break;
    case TX_TEMPLATE_WRITE:          l1tm_tx_template_write(prim, &tm_ret);            break;
    case TX_TEMPLATE_READ:           l1tm_tx_template_read(prim, &tm_ret);             break;
    case SPECIAL_PARAM_WRITE:        l1tm_special_param_write(prim, &tm_ret);          break;
    case SPECIAL_PARAM_READ:         l1tm_special_param_read(prim, &tm_ret);           break;
    case SPECIAL_TABLE_WRITE:        l1tm_special_table_write(prim, &tm_ret);          break;
    case SPECIAL_TABLE_READ:         l1tm_special_table_read(prim, &tm_ret);           break;
    case SPECIAL_ENABLE:             l1tm_special_enable(prim, &tm_ret);               break;
    #if (L1_DRP == 1)
      case DRP_SW_WRITE:             l1tm_drp_sw_write(prim, &tm_ret);               break;
    #endif
#if (CODE_VERSION != SIMULATION)
    case TPU_TABLE_WRITE:            l1tm_tpu_table_write(prim, &tm_ret);              break;
    case TPU_TABLE_READ:             l1tm_tpu_table_read(prim, &tm_ret);               break;
#endif
    #if(L1_TPU_DEV == 1)
      case FLEXI_TPU_TABLE_WRITE:  l1tm_flexi_tpu_table_write(prim, &tm_ret); break;
      case FLEXI_TPU_TABLE_READ:   l1tm_flexi_tpu_table_read(prim, &tm_ret); break;

      case FLEXI_ABB_WRITE:  l1tm_flexi_abb_write(prim, &tm_ret); break;
      case FLEXI_ABB_READ:   l1tm_flexi_abb_read(prim, &tm_ret); break;  
    #endif
#if (CODE_VERSION != SIMULATION)
      case DRP_CALIB_WRITE:  l1tm_drp_calib_write(prim, &tm_ret); break;
      case DRP_CALIB_READ :  l1tm_drp_calib_read(prim, &tm_ret); break;  // TBD for Future Use
#endif // CODE_VERSION
    default:
      #if (OP_L1_STANDALONE == 1)
        etm_core(msg);
        return;
      #else
        tm_ret.size = 0;
        #if (ETM_PROTOCOL == 1)
          tm_ret.status = -ETM_BADOP;
        #else
          tm_ret.status = E_BADCID;
        #endif
      #endif // ETM_PROTOCOL
      break;
    } // end of switch
  tm_transmit(&tm_ret);
}

/***********************************************************************/
/*                           TESTMODE 3.X                              */
/***********************************************************************/

 // omaps00090550 static UWORD8 tx_param_band=0;  // used in tx_param_write/read; default is GSM900
/*-----------------------------------------------------------*/
/* l1a_tmode()                                               */
/*-----------------------------------------------------------*/
/* Description : State machine controls TestMode processes   */
/*                                                           */
/* Starting messages:        TMODE_BCCH_REQ                  */
/*                           TMODE_PM_REQ                    */
/*                           TMODE_FB0_REQ                   */
/*                           TMODE_FB1_REQ                   */
/*                           TMODE_SB_REQ                    */
/*                                                           */
/*                                                           */
/* Result messages (input):  L1_TMODE_MEAS_REPORT            */
/*                           L1_SB_INFO                      */
/*                           L1_BCCHS_INFO                   */
/*                                                           */
/* Result messages (output): TMODE_PM_CON                    */
/*                           MPH5_NCELL_SB_IND               */
/*                                                           */
/* Reset messages (input):   MPH5_STOP_BCCH_READING          */
/*                                                           */
/*-----------------------------------------------------------*/
void l1a_tmode(xSignalHeaderRec *msg)
{
#if (ETM_PROTOCOL == 1)
  T_TESTMODE_PRIM   *prim; 
  T_TM_RETURN       tm_ret;
#endif

  int SignalCode = msg->SignalCode;

#if (OP_WCP==1) && (OP_L1_STANDALONE!=1)
  // Security check for Operating System platforms (open platforms) 
  // We forbid TESTMODE if the phone is running with an OS
  // so users can not invoke TESTMODE for malicious goals
  extern unsigned long GC_RunningWithOs();

  if(GC_RunningWithOs())
    return;
#endif

  if (SignalCode == TESTMODE_PRIM)
  {
  #if (ETM_PROTOCOL == 1)
    prim = (T_TESTMODE_PRIM *)(msg->SigP);
    switch (prim->mid)
    {
      case ETM_RF:
        etm_rf(msg);
      break;
    #if (OP_L1_STANDALONE == 1)
      case ETM_CORE:
        etm_core(msg);
      break;
    #endif
      default:
        // fill in the mid
        tm_ret.mid = prim->mid;
        tm_ret.status = -ETM_BADOP;
        // fill in the cid
        tm_ret.cid = prim->cid;
        tm_ret.size = 0;
        tm_transmit(&tm_ret);
      break;
    } //switch
  #elif (ETM_PROTOCOL == 0)
    etm_rf(msg);
  #endif
  } //end of TESTMODE_PRIM
  #if L1_GPRS
  else if ( ((SignalCode <= TMODE_PDTCH_INFO) && (SignalCode >= TMODE_RXLEV_REQ)) || (l1tm.tm_msg_received == TRUE) )
  #else
  else if ( ((SignalCode <= TMODE_TCH_INFO) && (SignalCode >= TMODE_RXLEV_REQ)) || (l1tm.tm_msg_received == TRUE) )  
  #endif
  {
  #if (CODE_VERSION == SIMULATION)
    static BOOL tm_init = FALSE;

    if (! tm_init)
    {
      Cust_tm_init();
      l1tm_initialize_var();
      l1_config.TestMode = 1;
      tm_init=TRUE;
    }
  #endif
    l1a_tmode_fb0_process(msg);
    l1a_tmode_fb1_process(msg);
    l1a_tmode_sb_process(msg);
    l1a_tmode_bcch_reading_process(msg);
    l1a_tmode_dedicated_process(msg);
    l1a_tmode_access_process(msg);
    l1a_tmode_full_list_meas_process(msg);
  #if L1_GPRS
    l1a_tmode_transfer_process(msg);
  #endif
  #if ((L1_STEREOPATH == 1) && (OP_L1_STANDALONE == 1))
    l1a_tmode_audio_stereopath_process(msg);
  #endif
  }  //end of not TESTMODE_PRIM
} // end of procedure.


/*-------------------------------------------------------*/
/* l1a_tmode_fb0_process()                               */
/*-------------------------------------------------------*/
/* Description : This state machine handles the 1st      */
/* synchronization with the network in Test Mode.        */
/*                                                       */
/* Starting messages:        TMODE_FB0_REQ               */
/*                                                       */
/* Result messages (input):  L1C_FB_INFO                 */
/*                                                       */
/*-------------------------------------------------------*/
void l1a_tmode_fb0_process(xSignalHeaderRec *msg)
{
  enum states
  {
    RESET              =  0,  // Reset state.
    WAIT_INIT          =  1,  // Initial state.
    SET_FS_FB_MODE0    =  2,  // First Synchro, Setting of 1st FB mode 0.
    WAIT_FS_FB_MODE0   =  3   // First Synchro, 1st FB mode 0 state.
  };

  UWORD8  *state      = &l1a.state[TMODE_FB0];
  UWORD32  SignalCode = msg->SignalCode;
  BOOL     done = 0;
  
#if (VCXO_ALGO == 1)
    #define FS_FB_MODE0_CENTER   1
    #define FS_FB_MODE0_MAX      2
    #define FS_FB_MODE0_MIN      3

    static WORD16   state_vcxo;
    static WORD16   static_attempt_counter;
#endif

  BOOL end_process = 0;

  while(!end_process)
  {
    switch(*state)
    {
      case RESET:
      {
        // Step in state machine.
        *state = WAIT_INIT;

#if (VCXO_ALGO == 1)
        if(l1_config.params.eeprom_afc == 0) {
          // Go to the initial VCXO AFC_INIT algorithm state
          state_vcxo = FS_FB_MODE0_CENTER;
          static_attempt_counter = 0;
        }
#endif

        // Reset tasks used in the process.
        l1a_l1s_com.l1s_en_task[FBNEW] = TASK_DISABLED;  // in tmode, not ALR
        l1a_l1s_com.l1s_en_task[NSYNC] = TASK_DISABLED;

        // Disable neighbour sync 0.
        l1a_l1s_com.nsync.list[0].status = NSYNC_FREE;

      }
      break;

      case WAIT_INIT:
      {
        if (SignalCode == TMODE_FB0_REQ)
        {
          #if (CODE_VERSION == SIMULATION)
            l1tm_reset_rx_state();
          #endif

          // Flag msg received
          l1tm.tm_msg_received = TRUE;

          // Set task semaphores.
          l1a_l1s_com.task_param[FBNEW] = SEMAPHORE_SET;  // Set "parameter synchro semaphore for FB task.
          l1a_l1s_com.task_param[NSYNC] = SEMAPHORE_SET;  // Set "parameter synchro semaphore for FB task.

          l1a_l1s_com.nsync.current_list_size = 0;

          // Downlink stuff timeslot is 0 (default in CS)
          l1a_l1s_com.dl_tn = 0;

          // Set arfcn
          l1a_l1s_com.nsync.list[0].radio_freq =l1_config.tmode.rf_params.bcch_arfcn;
          
          //Set timing validity for FB no a priori info
          l1a_l1s_com.nsync.list[0].timing_validity = 0;

          // Reset offset and time alignment
          l1a_l1s_com.nsync.list[0].fn_offset    = 0;
          l1a_l1s_com.nsync.list[0].time_alignmt = 0;

          // Set functional mode.
          l1a_l1s_com.mode = CS_MODE;  //Needs to be set for l1ddsp_load_monit_task()

          // Wideband search for FB detection.
          l1a_l1s_com.fb_mode = FB_MODE_0;

          // Enable SYNCHRO task to cleanup the MFTAB.
          l1a_l1s_com.l1s_en_task[SYNCHRO] = TASK_ENABLED;

          // Initialize AFC control function.
          #if AFC_ALGO
          if (l1_config.afc_enable) { // l1_config.TestMode MUST == 1
            #if (VCXO_ALGO == 1)
                // The TM rfpw 10 parameter has a different meaning when using
                // a VCXO. Instead of containing the AFC used for the FB0, which
                // is stored in the rftw 9 table now, it tells the TM which "state" must
                // be used for the VCXO algorithm
                //
                switch(l1_config.params.eeprom_afc) {
                    case 0: // Full VCXO algo
                        // The AFC_INIT state is controlled by the state machine
                        // Reset attempt counter
                        static_attempt_counter = 0;
                        state_vcxo = FS_FB_MODE0_CENTER;
                        break;
                    case FS_FB_MODE0_CENTER * 8:
                        state_vcxo = FS_FB_MODE0_CENTER;
                        break;
                    case FS_FB_MODE0_MIN * 8:
                        state_vcxo = FS_FB_MODE0_MIN;
                        break;
                    case FS_FB_MODE0_MAX * 8:
                        state_vcxo = FS_FB_MODE0_MAX;
                        break;
                     default:
                        state_vcxo = FS_FB_MODE0_CENTER;
                }
            #endif
          }
          #endif
          
          // Step in state machine
          *state = SET_FS_FB_MODE0;
        }
        else
        {
          // End of process.
          return;
        }
      }
      break;

      case SET_FS_FB_MODE0:
      {
        // Step in state machine.
        *state = WAIT_FS_FB_MODE0;
        
        // Enable neighbour sync 0.
        l1a_l1s_com.nsync.list[0].status = NSYNC_PENDING; //Used by l1s_schedule_tasks in l1_sync

        // Enable NSYNC task for FB detection mode 0.
        l1a_l1s_com.l1s_en_task[NSYNC] = TASK_ENABLED;
          if (l1_config.afc_enable) {// l1_config.TestMode MUST == 1
            #if (VCXO_ALGO == 1)
                switch(state_vcxo) {
                    case FS_FB_MODE0_CENTER:
                        l1s.afc = l1ctl_afc(AFC_INIT_CENTER, &l1s.afc_frame_count, NULL, 0, l1a_l1s_com.nsync.list[0].radio_freq,l1a_l1s_com.mode);
                        break;
                    case FS_FB_MODE0_MIN:
                        l1s.afc = l1ctl_afc(AFC_INIT_MIN, &l1s.afc_frame_count, NULL, 0, l1a_l1s_com.nsync.list[0].radio_freq,l1a_l1s_com.mode);
                        break;
                    case FS_FB_MODE0_MAX:
                        l1s.afc = l1ctl_afc(AFC_INIT_MAX, &l1s.afc_frame_count, NULL, 0, l1a_l1s_com.nsync.list[0].radio_freq,l1a_l1s_com.mode);
                        break;
                     default:
                        l1s.afc = l1ctl_afc(AFC_INIT_CENTER, &l1s.afc_frame_count, NULL, 0, l1a_l1s_com.nsync.list[0].radio_freq,l1a_l1s_com.mode);
                }
            #else
                l1s.afc = l1ctl_afc(AFC_INIT, &l1s.afc_frame_count, l1_config.params.eeprom_afc, 0, l1a_l1s_com.nsync.list[0].radio_freq);
            #endif
           } // (l1_config.afc_enable) is TRUE

        // End of process.
        end_process = 1;
      }
      break;
      
      case WAIT_FS_FB_MODE0:
      {
        if(SignalCode == L1C_FB_INFO)
        // Frequency Burst acquisition result.
        //------------------------------------
        {
          BOOL fb_found = ((T_L1C_FB_INFO *) (msg->SigP))->fb_flag;
     
          if (fb_found)
          {
            // We consider the result of this successfull FB search attempt
            // as a good a-priori information for next attempt.
            // "fn_offset" is reversed to satisfy its definition,
            // fn_offset = Fn_neigh - Fn_serving.
            l1a_l1s_com.nsync.list[0].timing_validity = 1;
            l1a_l1s_com.nsync.list[0].fn_offset = 51 - l1a_l1s_com.nsync.list[0].fn_offset;
          }
#if (VCXO_ALGO == 1)
          else
          {
            if(l1_config.params.eeprom_afc == 0)
            {
              //- Full VCXO algo

              // Increment "static_attempt_counter".
              static_attempt_counter++;
              if(static_attempt_counter < 4)
              {
                // Max number of attemps not reached yet...
                // try again with the same VCXO state
                *state = SET_FS_FB_MODE0;
                // Do not accumulate the statistics yet, just try again
                break;
              }
              else
              {
                // Max number of attempt is reached... go back to 1st FB mode 0.
                // Step in state machine.
                static_attempt_counter = 0;

                // Go to the next FS_FB_MODE0_CENTER state (CENTER -> MAX -> MIN)
                // After MIN go to CENTER again, which means that the attempt failed
                switch(state_vcxo)
                {
                  case FS_FB_MODE0_CENTER:
                    state_vcxo = FS_FB_MODE0_MAX;
                  break;
                  case FS_FB_MODE0_MAX:
                    state_vcxo = FS_FB_MODE0_MIN;
                  break;
                  default: // i.e. case FS_FB_MODE0_MAX:
                    // The algorithm tried all the AFC_INIT values (CENTER, MAX & MIN)
                    // but did not detect an FB in any of the attemps for these values:
                    // The current attempt FAILED => Continue and accumulate the statistics
                    state_vcxo = FS_FB_MODE0_CENTER;
                  break;
                }

                if (state_vcxo != FS_FB_MODE0_CENTER)
                {
                  *state = SET_FS_FB_MODE0;
                  // Do not accumulate the statistics yet, just try again with the new
                  // selected state_vcxo
                  break; // This breaks from the switch(*state), and thus re-loops thanks to the while(!end_process)
                } // (state_vcxo != FS_FB_MODE0_CENTER)
              }  // (static_attempt_counter >= 4)
            }  // (l1_config.params.eeprom_afc != 0)
          }  //  (fb_found) is FALSE
#endif

#if (VCXO_ALGO == 1)
          if(l1_config.params.eeprom_afc == 0)
          {
            // If we got this far, the attempt ended (with a fail or a success)
            // So we can go back to the initial state
            state_vcxo = FS_FB_MODE0_CENTER;
          }
#endif

          //accumulate FB stats
          l1tm_stats_fb_confirm( (T_TMODE_FB_CON*) ((T_L1C_FB_INFO *) (msg->SigP)), 0);

          done = l1tm_is_rx_counter_done();
          if (done == 1)
          {
            // Loop counter expired, stop the test
            *state = RESET;

            // Reset TM msg flag
            // No new L1S result messages may be received before a new TM command
            l1tm.tm_msg_received = FALSE;
            break;  // break out of switch
          }
          *state = SET_FS_FB_MODE0;
        }
        else if (SignalCode == TMODE_STOP_RX_TX)
        // Stop SYNC mode message.
        //--------------------------------
        {
            // Reset TM msg flag
            // No new L1S result messages may be received before a new TM command
            l1tm.tm_msg_received = FALSE;

            // Step in state machine.
            *state = RESET;

            return;
        }
        else
        {
          // End of process.
          return;
        }
      }
      break;
    }
  }
}


/*-------------------------------------------------------*/
/* l1a_tmode_fb1_process()                               */
/*-------------------------------------------------------*/
/* Description : This state machine handles the 1st      */
/* synchronization with the network in Test Mode.        */
/*                                                       */
/* Starting messages:        TMODE_FB1_REQ               */
/*                                                       */
/* Result messages (input):  L1C_FB_INFO                 */
/*                                                       */
/*-------------------------------------------------------*/
void l1a_tmode_fb1_process(xSignalHeaderRec *msg)
{
  enum states
  {
    RESET              =  0,  // Reset state.
    WAIT_INIT          =  1,  // Initial state.
    SET_FS_FB_MODE1    =  2,  // First Synchro, Setting of 1st FB mode 1.
    WAIT_FS_FB_MODE1   =  3   // First Synchro, FB mode 1 state.
  };

  UWORD8  *state      = &l1a.state[TMODE_FB1];
  UWORD32  SignalCode = msg->SignalCode;
  BOOL     done = 0;

  BOOL end_process = 0;

  while(!end_process)
  {
    switch(*state)
    {
      case RESET:
      {
        // Step in state machine.
        *state = WAIT_INIT;

        // Reset of tasks used in this process is carried out in WAIT_FS_FBMODE1 state
        // Otherwise we would possibly reset the task set by l1a_tmode_fb0_process()
      }
      break;

      case WAIT_INIT:
      {
        if (SignalCode == TMODE_FB1_REQ)
        {
          #if (CODE_VERSION == SIMULATION)
            l1tm_reset_rx_state();
          #endif

          // Flag msg received
          l1tm.tm_msg_received = TRUE;

          // Set task semaphores.
          l1a_l1s_com.task_param[FBNEW] = SEMAPHORE_SET;  // Set "parameter synchro semaphore for FB task.
          l1a_l1s_com.task_param[NSYNC] = SEMAPHORE_SET;  // Set "parameter synchro semaphore for FB task.

          // Downlink stuff timeslot is 0 (default in CS)
          l1a_l1s_com.dl_tn = 0;

          // Set arfcn
          l1a_l1s_com.nsync.list[0].radio_freq =l1_config.tmode.rf_params.bcch_arfcn;

          // Set functional mode.
          l1a_l1s_com.mode = CS_MODE;  //Needs to be set for l1ddsp_load_monit_task()

          // Set FB detection mode.
          l1a_l1s_com.fb_mode = FB_MODE_1;

          // Step in state machine
          *state = SET_FS_FB_MODE1;
        }
        else
        {
          // End of process.
          return;
        }
      }
      break;

      case SET_FS_FB_MODE1:
      {
        // Step in state machine.
        *state = WAIT_FS_FB_MODE1;

        // Enable neighbour sync 0.
        l1a_l1s_com.nsync.list[0].status = NSYNC_PENDING;

        // Enable NSYNC task for FB detection mode 1.
        l1a_l1s_com.l1s_en_task[NSYNC] = TASK_ENABLED;

        // End of process.
        end_process = 1;
      }
      break;

      case WAIT_FS_FB_MODE1:
      {
        // Use incoming message.
        //----------------------
        if(SignalCode == L1C_FB_INFO)
        {
          //accumulate FB stats
          l1tm_stats_fb_confirm( (T_TMODE_FB_CON*) ((T_L1C_FB_INFO *) (msg->SigP)), 0);

          // increment counter
          done = l1tm_is_rx_counter_done();

          if (done == 1)
          {
            // Loop counter expired, stop the test
            *state = RESET;

            // Reset TM msg flag
            // No new L1S result messages may be received before a new TM command
            l1tm.tm_msg_received = FALSE;

            // Reset tasks used in the process.
            l1a_l1s_com.l1s_en_task[FBNEW] = TASK_DISABLED;  // in tmode, not ALR
            l1a_l1s_com.l1s_en_task[NSYNC] = TASK_DISABLED;

            // Disable neighbour sync 0.
            l1a_l1s_com.nsync.list[0].status = NSYNC_FREE;

            break;  // break out of switch
          }

          *state = SET_FS_FB_MODE1;
        } // end if L1C_FB_INFO
        else if (SignalCode == TMODE_STOP_RX_TX)
        // Stop SYNC mode message.
        //--------------------------------
        {
            // Reset TM msg flag
            // No new L1S result messages may be received before a new TM command
            l1tm.tm_msg_received = FALSE;

            // Step in state machine.
            *state = RESET;

            return;
        }
        else
        // No action in this machine for other messages.
        //----------------------------------------------
        {
          // End of process.
          return;
        }
      }
      break;
    }
  }
}


/*-------------------------------------------------------*/
/* l1a_tmode_sb_process()                                */
/*-------------------------------------------------------*/
/* Description : This state machine handles the 1st      */
/* synchronization with the network in Test Mode.        */
/*                                                       */
/* Starting messages:        TMODE_SB_REQ                */
/*                                                       */
/* Result messages (input):  L1C_SB_INFO                 */
/*                                                       */
/*-------------------------------------------------------*/
void l1a_tmode_sb_process(xSignalHeaderRec *msg)
{
  enum states
  { 
    RESET        =  0,  // Reset state.
    WAIT_INIT    =  1,  // Initial state.
    SET_FS_SB    =  2,  // Set SB
    WAIT_FS_SB   =  3,  // Wait for SB result
    NEW_SYNCHRO  =  4   // Camp on cell
  };

  UWORD8  *state      = &l1a.state[TMODE_SB];
  UWORD32  SignalCode = msg->SignalCode;
  BOOL     done = 0;

  static UWORD8   static_sb_found_flag;
  static UWORD8   static_bsic;
  static UWORD32  static_fn_offset;
  static UWORD32  static_time_alignmt;

  BOOL end_process = 0;
  while(!end_process)
  {
    switch(*state)
    {
      case RESET:
      {
        // Step in state machine.
        *state = WAIT_INIT;

        // Reset of tasks used in this process is carried out in WAIT_FS_SB state
        // Otherwise we would possibly reset the NSYNC task set by l1a_tmode_fb0_process()
        // or l1a_tmode_fb1_process
      }
      break;

      case WAIT_INIT:
      {
        if (SignalCode == TMODE_SB_REQ)
        {
          #if (CODE_VERSION == SIMULATION)
            l1tm_reset_rx_state();
          #endif

          // Flag msg received
          l1tm.tm_msg_received = TRUE;

          // Set arfcn
          l1a_l1s_com.nsync.list[0].radio_freq =l1_config.tmode.rf_params.bcch_arfcn;

          // Enable NSYNC task for SB detection (SB2).
        #if ((REL99 == 1) && ((FF_BHO == 1) || (FF_RTD == 1)))
          l1a_l1s_com.nsync.list[0].timing_validity = SB_ACQUISITION_PHASE ;
        #else
          l1a_l1s_com.nsync.list[0].timing_validity = 3;
        #endif

          // Step in state machine
          *state = SET_FS_SB;
        }
        else
        {
          // End of process.
          return;
        }
      }
      break;

      case SET_FS_SB:
      {
        // Step in state machine.
        *state = WAIT_FS_SB;

        // Enable neighbour sync 0.
        l1a_l1s_com.nsync.list[0].status = NSYNC_PENDING;

        // Enable NSYNC task for SB detection.
        l1a_l1s_com.l1s_en_task[NSYNC] = TASK_ENABLED;

        // End of process.
        end_process = 1;
      }
      break;

      case WAIT_FS_SB:
      {
        // Use incoming message.
        //----------------------
        if(SignalCode == L1C_SB_INFO)
        {
          UWORD8  sb_found = ((T_MPHC_NCELL_SYNC_IND *)(msg->SigP))->sb_flag;
          UWORD8  bsic     = ((T_MPHC_NCELL_SYNC_IND *)(msg->SigP))->bsic;
                   
          if(sb_found == TRUE)
          // SB detection is a success...
          //-----------------------------
          {
            // Save Results.
            static_sb_found_flag = TRUE;
            static_bsic          = ((T_MPHC_NCELL_SYNC_IND *)(msg->SigP))->bsic;
            static_fn_offset     = ((T_MPHC_NCELL_SYNC_IND *)(msg->SigP))->fn_offset;
            static_time_alignmt  = ((T_MPHC_NCELL_SYNC_IND *)(msg->SigP))->time_alignmt;            
          }

          l1tm_stats_sb_confirm( (T_TMODE_NCELL_SYNC_IND*) ((T_MPHC_NCELL_SYNC_IND*)(msg->SigP)), 0);
                    
          // if just an SB test, increment counter
          // if not done, just stay in this state, schedule a new attempt
          done = l1tm_is_rx_counter_done();

          if (done == 1)
          {
            // step in state machine
            *state = NEW_SYNCHRO;

            // Reset NSYNC task and SB2 task enable flags.
            l1a_l1s_com.l1s_en_task[SB2]   = TASK_DISABLED;    // in tmode, not ALR
            l1a_l1s_com.l1s_en_task[NSYNC] = TASK_DISABLED;
               
            // Disable neighbour sync 0.
            l1a_l1s_com.nsync.list[0].status = NSYNC_FREE;

            // Save results.
            l1a_l1s_com.nsync.list[0].fn_offset    = static_fn_offset;
            l1a_l1s_com.nsync.list[0].time_alignmt = static_time_alignmt;

            // Correct "ntdma" and "time_alignment" to shift 20 bit to the 
            // future for Normal Burst reading tasks.
            l1a_add_time_for_nb(&l1a_l1s_com.nsync.list[0].time_alignmt,
                                &l1a_l1s_com.nsync.list[0].fn_offset);

            break; // break out of switch
          }
          else
          // Make a new SB attempt
          {
            l1a_l1s_com.nsync.list[0].status = NSYNC_PENDING;
            
            // End of process.
            end_process = 1;
          } 
        }
        else if (SignalCode == TMODE_STOP_RX_TX)
        // Stop SYNC mode message.
        //--------------------------------
        {
          // Reset TM msg flag
          // No new L1S result messages may be received before a new TM command
          l1tm.tm_msg_received = FALSE;

          // Step in state machine.
          *state = RESET;

          end_process = 1;
        }
        else
        // No action in this machine for other messages.
        //----------------------------------------------
        {      
          // End of process.
          end_process = 1;
        }
      }//End Case WAIT_FS_SB
      break;

      case NEW_SYNCHRO:
      {
        // Reset the Neighbor Cell information structure.
        l1a_reset_cell_info(&(l1a_l1s_com.Scell_info));
           
        // STILL SAVING TSC WITHIN BSIC !!!!!!!!!!!!!!!!

        // Download ARFCN, timing information and bitmap from the command message.
        l1a_l1s_com.Scell_info.radio_freq   = l1_config.tmode.rf_params.bcch_arfcn;
        l1a_l1s_com.Scell_info.bsic         = static_bsic;
        l1a_l1s_com.Scell_info.time_alignmt = l1a_l1s_com.nsync.list[0].time_alignmt;
        l1a_l1s_com.Scell_info.fn_offset    = l1a_l1s_com.nsync.list[0].fn_offset;

        // tn_difference -> loaded with the number of timeslot to shift.
        // dl_tn         -> loaded with the new timeslot.
        l1a_l1s_com.tn_difference = 0 - l1a_l1s_com.dl_tn;
        l1a_l1s_com.dl_tn         = 0; // Camping on timeslot 0.

        // Layer 1 internal mode is set to IDLE MODE.
        l1a_l1s_com.mode = I_MODE;

        // Set flag for toa init.
        #if (TOA_ALGO != 0)
          l1a_l1s_com.toa_reset = TRUE;
        #endif

        // Enable SYNCHRO tasks.
        l1a_l1s_com.l1s_en_task[SYNCHRO] = TASK_ENABLED;

        // Step in state machine.
        *state = RESET;

        // Reset TM msg flag
        // No new L1S result messages may be received before a new TM command
        l1tm.tm_msg_received = FALSE;
      }
        break;  // break out of switch
 // omaps00090550       break;
    }
  }
}

/*-------------------------------------------------------*/
/* l1a_tmode_bcch_reading_process()                      */
/*-------------------------------------------------------*/
/* Description : This state machine handles serving cell */
/* BCCH reading in Test Mode.                            */
/*                                                       */
/* This process happens for a TestMode BCCH test, after  */
/* completing FB's and SB's in                           */
/* l1a_tmode_initial_network_sync_process,               */
/* and then passing through l1a_tmode_cres_process,      */
/*                                                       */
/* OR                                                    */
/*                                                       */
/* it can also happen for a TestMode TCH with synch      */
/* test where, FB's and SB's have already been detected, */
/* and 4 BCCH's will be received before moving to the    */
/* TCH and dedicated mode.                               */
/*                                                       */
/*                                                       */
/* Starting messages:        TMODE_SCELL_NBCCH_REQ       */
/* ------------------                                    */
/*                                                       */
/*                                                       */
/* Result messages (input):  L1C_BCCHS_INFO              */
/* ------------------------                              */
/* System information data block from L1S.               */
/*                                                       */
/*                                                       */
/* Reset messages (input):   TMODE_STOP_SCELL_BCCH_REQ   */
/* -----------------------                               */
/*                                                       */
/*-------------------------------------------------------*/
void l1a_tmode_bcch_reading_process(xSignalHeaderRec *msg)
{  
  enum states
  { 
    RESET              = 0,
    WAIT_INIT          = 1,
    NBCCHS_CONFIG      = 2,
    WAIT_BCCHS_RESULT  = 3
  };

  UWORD8  *state      = &l1a.state[TMODE_BCCH];
  UWORD32  SignalCode = msg->SignalCode;
  BOOL     done = 0;

  BOOL end_process = 0;
  while(!end_process)
  {
    switch(*state)
    {
      case RESET:
      {
        // Step in state machine.
        *state = WAIT_INIT;

        // Reset CS_MEAS process.
        l1a_l1s_com.l1s_en_task[NBCCHS] = TASK_DISABLED;   // Clear NBCCHS task enable flag.
        //l1a_l1s_com.l1s_en_task[EBCCHS] = TASK_DISABLED;   // Clear EBCCHS task enable flag.
      }
      break;

      case WAIT_INIT:
      {
        if(SignalCode == TMODE_SCELL_NBCCH_REQ)
        {

          #if (CODE_VERSION == SIMULATION)
            l1tm_reset_rx_state();
          #endif

          // Flag msg received
          l1tm.tm_msg_received = TRUE;

          // Request to read Normal BCCH from serving cell.
          l1a_l1s_com.Scell_info.radio_freq = l1_config.tmode.rf_params.bcch_arfcn;
          
          // Step in state machine.
          *state = NBCCHS_CONFIG;
        }

        else
        // No action in this machine for other messages.
        //----------------------------------------------
        {
          // End of process.
          return;
        }
      }
      break;

      case NBCCHS_CONFIG:
      {
        UWORD8 i;

        // Set semaphores for Normal Serving BCCH reading task.
         l1a_l1s_com.task_param[NBCCHS] = SEMAPHORE_SET;

        // Download message content.
        //--------------------------
        l1a_l1s_com.nbcchs.schedule_array_size = 1;

        for(i=0;i<l1a_l1s_com.nbcchs.schedule_array_size;i++)
        {
          l1a_l1s_com.nbcchs.schedule_array[i].modulus = 1;
          l1a_l1s_com.nbcchs.schedule_array[i].relative_position = 0;
        }

        // Enable NBCCHS task.
        l1a_l1s_com.l1s_en_task[NBCCHS] = TASK_ENABLED;

        // Step in state machine.
        *state = WAIT_BCCHS_RESULT;

        // End of process.
        end_process = 1;
      }
      break;

      case WAIT_BCCHS_RESULT:
      {
        if(SignalCode == L1C_BCCHS_INFO)
        // Serving cell BCCH reading result.
        //----------------------------------
        {
          // this function takes care of loops management
          l1tm_stats_bcch_confirm( (T_TMODE_BCCHS_CON*) ((T_MPHC_DATA_IND *)(msg->SigP)) );

          done = l1tm_is_rx_counter_done();

          if (done == 1)
          {
            // Reset TM msg flag
            // No new L1S result messages may be received before a new TM command
            l1tm.tm_msg_received = FALSE;

            // This process must be reset.
            *state = RESET;

            break;
          }
          else
          {
            // End of process.
            end_process = 1;
          }
        }
        else if (SignalCode == TMODE_STOP_RX_TX)
        // Stop BCCH mode message.
        //--------------------------------
        {
            // Reset TM msg flag
            // No new L1S result messages may be received before a new TM command
            l1tm.tm_msg_received = FALSE;

            // Step in state machine.
            *state = RESET;

            end_process = 1;
        }
        else
        // No action in this machine for other messages.
        //----------------------------------------------
        {
          // End of process.
          end_process = 1;
        }
      }
      break;
    } // end of "switch".
  } // end of "while"
} // end of procedure.

/*-------------------------------------------------------*/
/* l1a_tmode_dedicated_process()                         */
/*-------------------------------------------------------*/
/* Description : This state machine handles the dedicated*/
/* mode setup in Test Mode (L1A side).                   */
/*                                                       */
/* Starting messages:        TMODE_IMMED_ASSIGN_REQ      */
/*                                                       */
/* Subsequent messages:                                  */
/*                                                       */
/* Result messages (input):  L1C_DEDIC_DONE              */
/*                                                       */
/* Result messages (output):                             */
/*                                                       */
/* Reset messages (input):   TMODE_STOP_RX_TX            */
/*                                                       */
/*-------------------------------------------------------*/
void l1a_tmode_dedicated_process(xSignalHeaderRec *msg)
{
  enum states
  {
    RESET     = 0,
    WAIT_INIT = 1,
    WAIT_MSG  = 2
  };
  
  static  UWORD32       confirm_SignalCode;
          T_DEDIC_SET  *free_set;
          UWORD8       *state      = &l1a.state[TMODE_DEDICATED];
          UWORD32       SignalCode = msg->SignalCode;

  BOOL end_process = 0;
  while(!end_process)
  {
    switch(*state)
    {
      case RESET:
      {
        // Step in state machine.
        *state = WAIT_INIT;

        l1a_l1s_com.l1s_en_task[FB26] = TASK_DISABLED; // Reset FB26 task enable flag.

        // Reset D_NMEAS process.
        l1a_l1s_com.l1s_en_meas  &= D_BAMS_MEAS_MASK; // Reset D_BAMS Measurement enable flag.
        l1a.l1a_en_meas[D_NMEAS] &= D_BAMS_MEAS_MASK;

        // Reset L1S dedicated mode manager trigger.
        l1a_l1s_com.dedic_set.SignalCode = NULL;
      }
      break;

      case WAIT_INIT:
      {
        switch(SignalCode)
        // switch on input message.
        //-------------------------------
        {
          case TMODE_IMMED_ASSIGN_REQ:
          // Immediate assignement message.
          //-------------------------------
          {
            UWORD8  maio_bef_sti;
#if(L1_FF_MULTIBAND == 1)
            UWORD8 operative_radio_freq;
#endif
            
            // Flag msg received
            l1tm.tm_msg_received = TRUE;

            #if (CODE_VERSION == SIMULATION)
              l1_config.tmode.rf_params.down_up = ((T_TMODE_IMMED_ASSIGN_REQ*)(msg->SigP))->ul_dl;
              l1tm_reset_rx_state();
              l1_config.tmode.stats_config.num_loops            = 26; // 0 actually means infinite
            #endif

            // save this info for later
            if (l1_config.tmode.rf_params.down_up == (TMODE_DOWNLINK|TMODE_UPLINK) &&
                l1_config.tmode.rf_params.mon_tasks == 1)
            {
              l1a_l1s_com.nsync.list[0].radio_freq = l1_config.tmode.rf_params.mon_arfcn;
            }

            // Get Ptr to the free dedicated parameter set.
            // All important fields are initialised.
            free_set = l1a_get_free_dedic_set();

            // Save given dedicated channel parameters from MPHC_IMMED_ASSIGN_REQ msg.

//========================================================================

            free_set->chan1.desc.chan_sel.h   = 0;  // no hopping

            free_set->chan1.desc.chan_sel.rf_channel.single_rf.radio_freq = l1_config.tmode.rf_params.tch_arfcn;
            //DON'T: union with  radio_freq     
            //free_set->chan1.desc.chan_sel.rf_channel.hopping_rf.maio=0;
            //free_set->chan1.desc.chan_sel.rf_channel.hopping_rf.hsn=0;

            free_set->chan1.desc.channel_type = l1_config.tmode.rf_params.channel_type;
            free_set->chan1.desc.subchannel   = l1_config.tmode.rf_params.subchannel;  
            free_set->chan1.desc.timeslot_no  = l1_config.tmode.rx_params.slot_num;
            free_set->chan1.desc.tsc          = l1_config.tmode.tx_params.tsc;

            //Set the loopback mode
            // 0: No loopback, 1: Loop A ... 6: Loop F
            if(l1_config.tmode.tx_params.burst_data >= 5 && l1_config.tmode.tx_params.burst_data <= 10 &&
               l1_config.tmode.rf_params.down_up == (TMODE_DOWNLINK | TMODE_UPLINK))
            {
              free_set->chan1.tch_loop =  l1_config.tmode.tx_params.burst_data - 4;

              // For loop back the channel mode needs to be set to TCH/FS
              free_set->chan1.mode = TCH_FS_MODE;
            }
            else
            {
              free_set->chan1.tch_loop = 0;  // no loopback

              // Rem1: Mode is forced to Signalling Only.            
              if (l1_config.tmode.rf_params.channel_type == TCH_F)
                free_set->chan1.mode = TCH_FS_MODE;
              else
                free_set->chan1.mode = SIG_ONLY_MODE;

            }
/*
;--------------------------------------------------------------------------
;     channel_desc_1 :
;           chan_sel     : ( h=FALSE, arfcn=5a )
;           channel_type = 1 (TCHFS)
;           subchannel   = 0
;           timeslot_no  = 0
;           tsc          = 5
;     timing_advance = 0
;     frequency_list :
;           rf_chan_cnt = 0000
;           rf_chan_no  : (0000, 0000, ...(total of 64)... 0000)
;     starting_time  =
;           start_time_present = FALSE
;           start_time :
;                n32 =
;                n51 =
;                n26 =
;     frequency_list_bef_sti
;           rf_chan_cnt = 0000
;           rf_chan_no  : (0000, 0000, ...(total of 64)... 0000)
;     maio_bef_sti = 0
;     dtx_allowed  = FALSE
;     bcch_allocation = 0 carriers (...)   UNUSED
;     ba_id           = 0                  UNUSED
;     pwrc            = 5
;--------------------------------------------------------------------------
*/
            free_set->ma.freq_list.rf_chan_cnt = 0;
            free_set->ma.freq_list.rf_chan_no.A[0] = 0;
            free_set->ma.freq_list_bef_sti.rf_chan_cnt = 0;
            free_set->ma.freq_list_bef_sti.rf_chan_no.A[0] = 0; //DedicNew

            maio_bef_sti                   = 0;

            free_set->new_timing_advance   = l1_config.tmode.tx_params.timing_advance;
            free_set->dtx_allowed          = 0;
            // New Timing Advance value must be applied on 1st frame of dedic. channel.
            free_set->timing_advance = free_set->new_timing_advance; 

            l1a_l1s_com.dedic_set.pwrc     = l1_config.tmode.tx_params.txpwr;
            // l1a_l1s_com.dedic_set.pwrc  = 5; // change from TM2!

            // TXPWR command was given in Idle, save it in dedicated mode structure.
            free_set->new_target_txpwr = l1s.applied_txpwr = l1_config.tmode.tx_params.txpwr;

            // Serving Cell stays the same.
            free_set->cell_desc                  = l1a_l1s_com.Scell_info;
            
#if(L1_FF_MULTIBAND == 0)
            
            free_set->cell_desc.traffic_meas_beacon 
                                                 = l1a_l1s_com.last_input_level[l1a_l1s_com.Scell_info.radio_freq - l1_config.std.radio_freq_index_offset];
            free_set->cell_desc.traffic_meas     = l1a_l1s_com.last_input_level[l1a_l1s_com.Scell_info.radio_freq - l1_config.std.radio_freq_index_offset];

#else // L1_FF_MULTIBAND = 1 below

            operative_radio_freq = 
                l1_multiband_radio_freq_convert_into_operative_radio_freq(l1a_l1s_com.Scell_info.radio_freq);

            free_set->cell_desc.traffic_meas_beacon
                                                 = l1a_l1s_com.last_input_level[operative_radio_freq];
            free_set->cell_desc.traffic_meas     = l1a_l1s_com.last_input_level[operative_radio_freq];

#endif // #if(L1_FF_MULTIBAND == 0) else


            // Decode the "starting time field", since staying on the same serving
            // the same STI fn is saved in both "neig_sti_fn" and "serv_sti_fn".
            free_set->neig_sti_fn = -1; //l1a_decode_starting_time(((T_MPHC_IMMED_ASSIGN_REQ *)(msg->SigP))->starting_time);
            free_set->serv_sti_fn = free_set->neig_sti_fn;

            // Check/Fill "before starting time" fields.
            //l1a_fill_bef_sti_param(free_set, ((T_MPHC_IMMED_ASSIGN_REQ *)(msg->SigP))->starting_time.start_time_present);

            //No hopping channel

            // Save the "timeslot difference" between new and old configuration 
            // in "tn_difference".
            //   tn_difference -> loaded with the number of timeslot to shift.
            //   dl_tn         -> loaded with the new timeslot.
            l1a_l1s_com.tn_difference = free_set->chan1.desc.timeslot_no - l1a_l1s_com.dl_tn;
            l1a_l1s_com.dl_tn         = free_set->chan1.desc.timeslot_no;  // Save new TN id.

            // Set "fset" pointer to the new parameter set.
            l1a_l1s_com.dedic_set.fset = free_set;

            // Give new msg code to L1S. //TestMode: use existing L1S primitive name
            l1a_l1s_com.dedic_set.SignalCode = MPHC_IMMED_ASSIGN_REQ;

            // Set confirmation message name.
            confirm_SignalCode = TMODE_IMMED_ASSIGN_CON;

            // step in state machine.
            *state = WAIT_MSG;
          }
          break;

        } // end of "switch(SignalCode)".
        // end of process.
        end_process = 1;
      }
      break;

      case WAIT_MSG:
      {
        switch(SignalCode)
        // switch on input message.
        //-------------------------------
        {
          case L1C_DEDIC_DONE:
          // Dedicated channel activated.
          //-----------------------------
          {
            // if MON tasks are enabled, set up FB26 and D_BAMS_MEAS tasks now as well
            if (l1_config.tmode.rf_params.down_up == (TMODE_DOWNLINK|TMODE_UPLINK) &&
                l1_config.tmode.rf_params.mon_tasks == 1)
            {
              l1a_l1s_com.task_param[FB26] = SEMAPHORE_SET;
              l1a_l1s_com.l1s_en_task[FB26] = TASK_ENABLED;
#if (L1_12NEIGH ==1)
              //Set timing validity for FB no a priori info
              l1a_l1s_com.nsync.list[0].timing_validity = 0;

              // Enable neighbour sync 0.
              l1a_l1s_com.nsync.list[0].status = NSYNC_PENDING; //Used by l1s_schedule_tasks in l1_sync
              l1a_l1s_com.l1s_en_task[NSYNC] = TASK_ENABLED;
#endif
            }

            if (l1_config.tmode.rx_params.pm_enable)
            {
              // Reset the BA list structure
              l1a_reset_ba_list();

              // Next measurement report must indicate INVALID.
              //meas_valid = FALSE;

              l1a_l1s_com.ba_list.nbr_carrier = 1;

              // l1a_l1s_com.nsync.list[0].radio_freq was set to mon_arfcn above
              l1a_l1s_com.ba_list.A[0].radio_freq = l1a_l1s_com.nsync.list[0].radio_freq;

              // Set parameter synchro semaphore for D_BAMS task.
              // Enable Dedicated mode BA list measurement task.
              l1a_l1s_com.meas_param   |= D_BAMS_MEAS;
              l1a.l1a_en_meas[D_NMEAS] |= D_BAMS_MEAS;
            }

            // keep track of dedicated mode state.
            l1tm.tmode_state.dedicated_active = 1;

            // End of process.
           end_process = 1;
          }
          break;

          case TMODE_TCH_INFO:
          // TCH result messages.
          //-----------------------
          {
            // Check if RX stats done in TCH
            if (l1_config.tmode.rf_params.mon_report == 0)
            {
              BOOL done;

              // loop and stats management done within this function
              l1tm_stats_tch_confirm((T_TMODE_TCH_INFO *) (msg->SigP));

              done = l1tm_is_rx_counter_done();

              if (done == 1)
              // if done, send stop message
              {
                l1tm.tmode_state.dedicated_active = 0;

                // Give new msg code to L1S.
                l1a_l1s_com.dedic_set.SignalCode = MPHC_STOP_DEDICATED_REQ;

                // Reset TM msg flag
                // No new L1S result messages may be received before a new TM command
                l1tm.tm_msg_received = FALSE;

#if (L1_12NEIGH ==1)
                l1a_l1s_com.l1s_en_task[NSYNC] = TASK_DISABLED;
                // Disable neighbour sync 0.
                l1a_l1s_com.nsync.list[0].status = NSYNC_FREE; //Used by l1s_schedule_tasks in l1_sync
#endif

                // Step in state machine.
                *state = RESET;
              }
            }

            // end of process
            end_process = 1;
          }
          break;

          case L1C_FB_INFO:
          // MON result messages.
          //-----------------------
          {
            // Check if RX stats done in Monitor channel
            if (l1_config.tmode.rf_params.mon_report == 1)
            {
              BOOL done;

              // loop and stats management done within this function
              l1tm_stats_mon_confirm( (T_TMODE_FB_CON*) ((T_L1C_FB_INFO *) (msg->SigP)));

              done = l1tm_is_rx_counter_done();

              if (done == 1)
              // if done, send stop message
              {
                l1tm.tmode_state.dedicated_active = 0;

                // Give new msg code to L1S.
                l1a_l1s_com.dedic_set.SignalCode = MPHC_STOP_DEDICATED_REQ;

                // Reset TM msg flag
                // No new L1S result messages may be received before a new TM command
                l1tm.tm_msg_received = FALSE;

#if (L1_12NEIGH ==1)
                l1a_l1s_com.l1s_en_task[NSYNC] = TASK_DISABLED;
                // Disable neighbour sync 0.
                l1a_l1s_com.nsync.list[0].status = NSYNC_FREE; //Used by l1s_schedule_tasks in l1_sync
#endif

                // Step in state machine.
                *state = RESET;
              }
            }

            // end of process
            end_process = 1;
          }
          break;

          case TMODE_STOP_RX_TX:
          // Release dedicated mode message.
          //--------------------------------
          {
              l1tm.tmode_state.dedicated_active = 0;

              // Give new msg code to L1S.
              l1a_l1s_com.dedic_set.SignalCode = MPHC_STOP_DEDICATED_REQ;

              // Reset TM msg flag
              // No new L1S result messages may be received before a new TM command
              l1tm.tm_msg_received = FALSE;

#if (L1_12NEIGH ==1)
                l1a_l1s_com.l1s_en_task[NSYNC] = TASK_DISABLED;
                // Disable neighbour sync 0.
                l1a_l1s_com.nsync.list[0].status = NSYNC_FREE; //Used by l1s_schedule_tasks in l1_sync
#endif
              // Step in state machine.
              *state = RESET;

              end_process = 1;
          }
          break;

          default:
          // End of process.
          //----------------
          {
            end_process = 1;
          }
        } // end of "switch(SignalCode)".
      }
      break;

    } // end of "switch".
  } // end of "while"
} // end of procedure.

/*-------------------------------------------------------*/
/* l1a_tmode_ra_process()                                */
/*-------------------------------------------------------*/
/* Description : This state machine handles the TestMode */
/*               access to the network (IDLE mode).      */
/*                                                       */
/* Starting messages:        TMODE_RA_START              */
/*                                                       */
/* Result messages (input):  L1C_RA_DONE                 */
/*                                                       */
/* Result messages (output): TMODE_RA_DONE               */
/*                                                       */
/* Stop messages (input):    TMODE_STOP_RX_TX            */
/*                                                       */
/*-------------------------------------------------------*/
void l1a_tmode_access_process(xSignalHeaderRec *msg)
{
  enum states
  {
    RESET       = 0,
    WAIT_INIT   = 1,
    WAIT_RESULT = 2
  };

  UWORD8 *state      = &l1a.state[TMODE_RA];
  UWORD32 SignalCode = msg->SignalCode;
  BOOL    done = 0;

  while(1)
  {
    switch(*state)
    {
      case RESET:
      {
        // Step in state machine.
        *state = WAIT_INIT;

        // Reset RAACC process.
        l1a_l1s_com.l1s_en_task[RAACC] = TASK_DISABLED;   // Clear RAACC task enable flag.
      }
      break;
      
      case WAIT_INIT:
      {
        if(SignalCode == TMODE_RA_START)
        // Configuration message for Access Link.
        //---------------------------------------
        {
          // Flag msg received
          l1tm.tm_msg_received = TRUE;

          l1s.applied_txpwr = l1_config.tmode.tx_params.txpwr;
          l1a_l1s_com.ra_info.channel_request = 2; //l1_config.tm_params.channel_request;
          
          // Initialize rand counter for RAACC process. 
          l1a_l1s_com.ra_info.rand = 1;

          // rand is random number of frames to wait between each AB transmission.
          // In actual L1 code this changes with each burst.
          // It is set to 1 here so the test runs fast.

          // also, channel_request is constant in TestMode for all bursts.
          // Actual L1 changes channel_request message each time with a different
          // random reference.  [channel_request = 3 bits for establishment cause
          //                     and 5 bits of random reference]

          // Use 2 multiframes (0.5 seconds) in reading all serving normal bursts
          // (like paging reorganization) to refine TOA since we must have the quarter
          // bit accuracy for RACH transmission.

          // Delay the start of RACH transmission (by incrementing rand
          // counter), only at the start of the test.

          if(l1a_l1s_com.bcch_combined)
          {
            l1a_l1s_com.ra_info.rand += 54;  // Combined: 2 multiframes = 54 slots.
          }
          else
          {
            l1a_l1s_com.ra_info.rand += 102; // Not combined: 2 multiframes = 102 slots.
          }

          // step in state machine.
          *state = WAIT_RESULT;

          // TestMode does not set up full BCCH reading

          // Activate RAACC task (no semaphore for UL tasks).
          // TestMode does not enable Paging Reorg and Normal paging tasks.
          l1a_l1s_com.l1s_en_task[RAACC] = TASK_ENABLED; 

          // Change mode to connection establishment part 1.
          l1a_l1s_com.mode = CON_EST_MODE1;   // used for toa calc.
        }

        else
        // No action in this machine for other messages.
        //----------------------------------------------
        {
          // End of process.
          return;
        }
      }
      break;

      case WAIT_RESULT:
      {
        if(SignalCode == L1C_RA_DONE)
        // Random access acknowledge message.
        //-----------------------------------
        {
          // Change mode to connection establishment part 2.
          l1a_l1s_com.mode = CON_EST_MODE2;   // used for toa calc.

          //change power level and arfcn on the fly
          l1s.applied_txpwr = l1_config.tmode.tx_params.txpwr;
//          l1a_l1s_com.Scell_info.radio_freq = l1_config.tmode.rf_params.tch_arfcn;

          done = l1tm_is_rx_counter_done();

          if (done)
          {
            // Reset TM msg flag
            // No new L1S result messages may be received before a new TM command
            l1tm.tm_msg_received = FALSE;

            // No stats, just report end
            //tmstats_ra_confirm( (T_TMODE_RA_DONE*) ((T_MPHC_RA_CON *)(msg->SigP)) );

            *state = RESET;
          }
          else  // there are more loops to do...
          {
            l1a_l1s_com.ra_info.rand += 10;  // 1 chosen/set for quicker test
            // Activate RAACC task (no semaphore for UL tasks).
            l1a_l1s_com.l1s_en_task[RAACC] = TASK_ENABLED;   
            // end of process
            return;
          }
        }

        else if(SignalCode == TMODE_STOP_RX_TX)
        {
          // Reset TM msg flag
          // No new L1S result messages may be received before a new TM command
          l1tm.tm_msg_received = FALSE;

          // No stats, just report end
          //tmstats_ra_confirm( (T_TMODE_RA_DONE*) ((T_MPHC_RA_CON *)(msg->SigP)) );
          *state = RESET;
          return;
        }

        else
        // No action in this machine for other messages.
        //----------------------------------------------
        {
          // End of process.
          return;
        }
      }
      break;
    } // end of "switch".
  } // end of "while"
} // end of procedure.

/*-------------------------------------------------------*/
/* l1a_tmode_full_list_meas_process()                    */
/*-------------------------------------------------------*/
/*                                                       */
/* Description:                                          */
/* ------------                                          */
/* This function is a state machine which handles the    */
/* Cell Selection Full List Power Measurement L1/L3      */
/* interface and it handles the neigbour cell            */
/* measurement process in IDLE mode with FULL list.      */
/* When a message MPHC_RXLEV_REQ is received             */
/* the L1S task FSMS_MEAS is enabled. When this task     */
/* is completed a reporting message L1C_VALID_MEAS_INFO  */
/* is received and forwarded to L3.                      */
/*                                                       */
/* Starting messages:        MPHC_RXLEV_REQ.             */
/*                                                       */
/* Result messages (input):  L1C_VALID_MEAS_INFO         */
/*                                                       */
/* Result messages (output): MPHC_RXLEV_IND              */
/*                                                       */
/* Reset messages (input):   none                        */
/*                                                       */
/* Stop message (input):     MPHC_STOP_RXLEV_REQ         */
/*                                                       */
/* Stop message (output):    MPHC_STOP_RXLEV_CON         */
/*                                                       */
/* Rem:                                                  */
/* ----                                                  */
/* L3 is in charge of the number of pass to follow the   */
/* GSM recommendation.                                   */
/*                                                       */
/*-------------------------------------------------------*/
void l1a_tmode_full_list_meas_process(xSignalHeaderRec *msg)
{
  enum states 
  {
    RESET       = 0,
    WAIT_INIT   = 1,
    WAIT_RESULT = 2
  };

  UWORD8   *state      = &l1a.state[TMODE_FULL_MEAS];
  UWORD32   SignalCode = msg->SignalCode;

  BOOL end_process = 0;

  while(!end_process)
  {
    switch(*state)
    {
      case RESET:
      {
        // Step in state machine.
        *state = WAIT_INIT;

        // Reset FULL_MEAS process.
        l1a_l1s_com.l1s_en_meas &= FSMS_MEAS_MASK; // Clear Cell Selection Measurement enable flag.
      }
      break;

      case WAIT_INIT:
      {
        if(SignalCode == TMODE_RXLEV_REQ)
        // Request to enter the Cell Selection measurements.
        //--------------------------------------------------
        {
          UWORD16 i;

          // Flag msg received
          l1tm.tm_msg_received = TRUE;

          // Do NOT download info from message
          // In TestMode always a full scanning is done, therefore primitive does not send the list
          l1a_l1s_com.full_list_ptr=(T_FULL_LIST_MEAS *)((T_TMODE_RXLEV_REQ *)(msg->SigP));

          // Single power measurement carried out on monitor channel
          l1a_l1s_com.full_list_ptr->power_array_size = 1;
          l1a_l1s_com.full_list_ptr->power_array[0].radio_freq = l1_config.tmode.rf_params.mon_arfcn;

          // Set "parameter synchro semaphores"
          l1a_l1s_com.meas_param |= FSMS_MEAS;

          // Reset the full list structure.
          l1a_reset_full_list();

          // Reset the Input Level (IL) memory table.
#if (L1_FF_MULTIBAND == 1)
          for(i=0; i<= NBMAX_CARRIER; i++)
#else
          for(i=0; i<=l1_config.std.nbmax_carrier; i++)
#endif          
          {
            l1a_l1s_com.last_input_level[i].input_level = l1_config.params.il_min;
            l1a_l1s_com.last_input_level[i].lna_off     = 0;
          }

          // Enable Cell Selection Full list measurement task.
          l1a.l1a_en_meas[TMODE_FULL_MEAS] |= FSMS_MEAS;

          // Step in state machine.
          *state = WAIT_RESULT;
        }

        // End of process.
        end_process = 1;
      }
      break;

      case WAIT_RESULT:
      {
        if(SignalCode == L1C_VALID_MEAS_INFO)
        // One valid measurement pass has been completed over the full list of carriers.
        //------------------------------------------------------------------------------
        {
          BOOL done = FALSE;

          //--------------------------------------------------------
          // WE COULD PUT HERE THE CODE TO TRANSLATE IL -> RXLEV !!!
          //--------------------------------------------------------

          l1tm_stats_full_list_meas_confirm((T_TMODE_RXLEV_REQ *)(msg->SigP));

          done = l1tm_is_rx_counter_done();

          if (done)
          {
            // Reset TM msg flag
            // No new L1S result messages may be received before a new TM command
            l1tm.tm_msg_received = FALSE;

            // Reset the machine.
            *state = RESET;
          }
          else
          {
            // Reset the full list structure.
            l1a_reset_full_list(); 

            // Enable Cell Selection Full list measurement task.
            l1a.l1a_en_meas[TMODE_FULL_MEAS] |= FSMS_MEAS;

            // End of process
            end_process = 1;
          }
        }

        else if (SignalCode == TMODE_STOP_RX_TX)
        {
          // Reset TM msg flag
          // No new L1S result messages may be received before a new TM command
          l1tm.tm_msg_received = FALSE;

          // Forward result message to L3.
          l1a_send_confirmation(TMODE_STOP_RXLEV_CON,RRM1_QUEUE);
          // Reset the machine.
          *state = RESET;
          end_process = 1;
        }
        else
        // No action in this machine for other messages.
        //----------------------------------------------
        {
          // End of process.
          end_process = 1;
        }
      }
      break;
    } // end of "switch".
  } // end of "while"
} // end of procedure.


  #if L1_GPRS
  /*-------------------------------------------------------*/
  /* l1pa_tmode_transfer_process()                         */
  /*-------------------------------------------------------*/
  /* Description:                                          */
  /* ------------                                          */
  /*                                                       */
  /* Starting messages:                                    */
  /* ------------------                                    */
  /*                                                       */
  /* Subsequent messages:                                  */
  /* --------------------                                  */
  /*                                                       */
  /* Result messages (input):                              */
  /* ------------------------                              */
  /*                                                       */
  /* Result messages (output):                             */
  /* -------------------------                             */
  /*                                                       */
  /* Reset messages (input):                               */
  /* -----------------------                               */
  /*                                                       */
  /*-------------------------------------------------------*/
  void l1a_tmode_transfer_process(xSignalHeaderRec *msg)
  {
    enum states
    {
      RESET       = 0,
      WAIT_INIT   = 1,
      WAIT_MSG    = 2
    };

    UWORD8   *state     = &l1a.state[TMODE_TRANSFER];
    UWORD32  SignalCode = msg->SignalCode;

    static UWORD8 stat_gprs_slots;

    BOOL end_process = 0;

    while(!end_process)
    {
      switch(*state)
      {
        case RESET:
        {
          // Step in state machine.
          *state = WAIT_INIT;

          // Reset FB26 task enable flag.
          l1a_l1s_com.l1s_en_task[FB26] = TASK_DISABLED;

          // Reset TCR_MEAS process.
          l1pa_l1ps_com.l1ps_en_meas  &= P_TCRMS_MEAS_MASK; // Disable Neighbour Measurement task.
          l1pa.l1pa_en_meas[TCR_MEAS] &= P_TCRMS_MEAS_MASK; // Reset Neighbour Measurement task.

          // Rise transfert parameter semaphore.
          l1pa_l1ps_com.transfer.semaphore = TRUE;
        }
        break;

        case WAIT_INIT:
        {
          switch(SignalCode)
          // switch on input message.
          //-------------------------
          {       
            case TMODE_PDTCH_ASSIGN_REQ:
            // Assignement message.
            //---------------------
            {
              static UWORD32 count =0;

              T_TRANSFER_SET  *free_set;
              UWORD8           assignment_command;
              UWORD8           timeslot_alloc;
              UWORD8           timeslot;
              UWORD32          i;

              count++ ;
              
              #if (CODE_VERSION == SIMULATION)
                l1tm_reset_rx_state();
              #endif

              // Flag msg received
              l1tm.tm_msg_received = TRUE;

              // Rise transfert parameter semaphore to prevent L1S to use partial configuration.
              l1pa_l1ps_com.transfer.semaphore = TRUE;

              assignment_command = BOTH_TBF;

              // Get Ptr to the free dedicated parameter set.
              // All important fields are initialised.
              free_set = l1pa_get_free_transfer_set(assignment_command);

              // Download message containt.
              free_set->assignment_id      = 1;
              free_set->assignment_command = assignment_command;
              #if (CODE_VERSION == SIMULATION)
                free_set->multislot_class    = ((T_TMODE_PDTCH_ASSIGN_REQ *)(msg->SigP))->multislot_class;
              #else
                free_set->multislot_class    = l1_config.tmode.rf_params.multislot_class;
              #endif
              free_set->dl_pwr_ctl.p0      = 255; // no power control
              free_set->packet_ta.ta       = 0;
              free_set->packet_ta.ta_index = 255;
              free_set->packet_ta.ta_tn    = 255;
              free_set->tsc                = l1_config.tmode.tx_params.tsc;
              free_set->freq_param.chan_sel.h   = FALSE; // no hopping            
              free_set->freq_param.chan_sel.rf_channel.single_rf.radio_freq = l1_config.tmode.rf_params.pdtch_arfcn;
              free_set->mac_mode           = FIX_ALLOC_NO_HALF; // fixed allocation
              free_set->tbf_sti.present    = TRUE; // STI present
			  #if (CODE_VERSION == NOT_SIMULATION)
              // In order to reduce the latency for the ETM command "rfe 4", make absolute_fn as
              // next_time.fn+1. This was originally +100 because of which we had to wait for some
              // time before L1 actually starts the TBF.
              free_set->tbf_sti.absolute_fn= l1s.next_time.fn + 1;
			  #else
			// In PC simulation, keep the old +100 to keep output logs same as reference
			  free_set->tbf_sti.absolute_fn= l1s.next_time.fn + 100; // force to current FN+100
			  #endif

              free_set->interf_meas_enable = FALSE; // Interference measurements disabled
              free_set->pc_meas_chan       = TRUE;  // No measurement on the beacon (6 per MF52)

              // Allocation of both UL and DL time slots
              free_set->dl_tbf_alloc.tfi             = 1; // DL TFI ID
              free_set->ul_tbf_alloc->tfi            = 2; // UL TFI ID
              #if (CODE_VERSION == SIMULATION)
                free_set->dl_tbf_alloc.timeslot_alloc   = ((T_TMODE_PDTCH_ASSIGN_REQ *)(msg->SigP))->dl_ts_alloc;
                free_set->ul_tbf_alloc->timeslot_alloc  = ((T_TMODE_PDTCH_ASSIGN_REQ *)(msg->SigP))->ul_ts_alloc;
                l1_config.tmode.stats_config.num_loops  = ((T_TMODE_PDTCH_ASSIGN_REQ *)(msg->SigP))->ul_alloc_length;
                l1_config.tmode.rf_params.mon_tasks     = ((T_TMODE_PDTCH_ASSIGN_REQ *)(msg->SigP))->mon_enable;
                l1_config.tmode.rf_params.mon_report    = ((T_TMODE_PDTCH_ASSIGN_REQ *)(msg->SigP))->mon_enable;
                l1_config.tmode.rx_params.pm_enable     = ((T_TMODE_PDTCH_ASSIGN_REQ *)(msg->SigP))->pm_enable;
              #else
                free_set->dl_tbf_alloc.timeslot_alloc  = l1_config.tmode.rx_params.timeslot_alloc;            
                free_set->ul_tbf_alloc->timeslot_alloc = l1_config.tmode.tx_params.timeslot_alloc;
              #endif
              // free_set->ul_tbf_alloc->fixed_alloc.bitmap_length = l1_config.tmode.stats_config.num_loops;
              // force to 127
              free_set->ul_tbf_alloc->fixed_alloc.bitmap_length = 127;

              // Init fixed allocation bitmap
              for (i=0;i<free_set->ul_tbf_alloc->fixed_alloc.bitmap_length;i++)
                free_set->ul_tbf_alloc->fixed_alloc.bitmap[i] = free_set->ul_tbf_alloc->timeslot_alloc;

              free_set->allocated_tbf                = BOTH_TBF;

              // Process the downlink TBF first allocated timeslot
              timeslot_alloc = free_set->dl_tbf_alloc.timeslot_alloc;
              timeslot       = 0;

              while((timeslot<7) && !(timeslot_alloc & (0x80>>timeslot))) 
              {
                timeslot++;
              }

              free_set->dl_tbf_synchro_timeslot   = timeslot;

              // Fill "synchro_timeslot" which will be the frame synchro slot.
              free_set->transfer_synchro_timeslot = timeslot;

              // save stats bitmap
              stat_gprs_slots = l1_config.tmode.stats_config.stat_gprs_slots;
              // Adjust stats bit map
              l1_config.tmode.stats_config.stat_gprs_slots <<= timeslot;

              // Process the uplink TBF first allocated timeslot
              // Fixed mode: the 1st allocated timeslot is the downlink control
              // timeslot allocated by the network, which is a timeslot allocated
              // in uplink
              timeslot_alloc = free_set->ul_tbf_alloc->timeslot_alloc;

              timeslot = 0;
              while((timeslot<7) && !(timeslot_alloc & (0x80>>timeslot)))
              {
                timeslot++;
              }
              // if UL synchro TS > DL synchro TS, then fixed alloc ctrl TS is the UL sync TS
              // else fixed alloc ctrl TS is the DL sync TS
              if (timeslot > free_set->dl_tbf_synchro_timeslot)
                free_set->ul_tbf_alloc->fixed_alloc.ctrl_timeslot = timeslot;
              else
                free_set->ul_tbf_alloc->fixed_alloc.ctrl_timeslot = free_set->dl_tbf_synchro_timeslot;

              free_set->ul_tbf_synchro_timeslot   = free_set->ul_tbf_alloc->fixed_alloc.ctrl_timeslot;

              // Init txpwr levels for multi slot TX
              // txpwr[index] is calculated according to TX allocation given by MACS
              // timeslot contains the first allocated TS in UL
              for(i = timeslot; i < 8; i++)
              {
                l1pa_l1ps_com.transfer.dl_pwr_ctrl.txpwr[i] 
                  = l1_config.tmode.tx_params.txpwr_gprs[i];
              }

              // Step in state machine.
              *state = WAIT_MSG;

              // Store signalcode.
              free_set->SignalCode = MPHP_ASSIGNMENT_REQ;

              // Clear transfer parameter semaphore to let L1S use the new parameters.
              l1pa_l1ps_com.transfer.semaphore = FALSE;

              // end of process.
              end_process = 1;
            }
            break;

            default:
            // End of process.
            //----------------
            {
              return;
            }
          } // end switch(SignalCode)
        } // end case WAIT_INIT
        case WAIT_MSG:
        {
          switch(SignalCode)
          // switch on input message.
          //-------------------------
          {
            case L1P_TRANSFER_DONE:
            // Switch to TRANSFER mode has been done.
            {
              T_CRES_LIST_PARAM *free_list;

              // Set up TCR_MEAS task, if MON tasks are enabled set up FB26
              if (l1_config.tmode.rf_params.mon_tasks == 1)
              {
                // Set FB26 task semaphore
                l1a_l1s_com.task_param[FB26] = SEMAPHORE_SET;
                              
                // This process always use the first element of "nsync" structure.
                l1a_l1s_com.nsync.current_list_size = 0;
                // l1a_l1s_com.nsync.list[0].radio_freq was set to mon_arfcn above
                l1a_l1s_com.nsync.list[0].radio_freq = l1_config.tmode.rf_params.mon_arfcn;

                // Enable FB detection during packet transfer
                l1a_l1s_com.l1s_en_task[FB26] = TASK_ENABLED;
#if (L1_12NEIGH ==1)
                //Set timing validity for FB no a priori info
                l1a_l1s_com.nsync.list[0].timing_validity = 0;
                // Enable neighbour sync 0.
                l1a_l1s_com.nsync.list[0].status = NSYNC_PENDING; //Used by l1s_schedule_tasks in l1_sync
                l1a_l1s_com.l1s_en_task[NSYNC] = TASK_ENABLED;
#endif
              }

              if (l1_config.tmode.rx_params.pm_enable)
              {
                // Set parameter synchro semaphore for P_TCRMS_MEAS task.
                l1pa_l1ps_com.meas_param |= P_TCRMS_MEAS;

                // Reset Neighbour Cell measurement parameters.
                l1pa_l1ps_com.tcr_freq_list.tcr_next_to_ctrl = 0;
                l1pa_l1ps_com.tcr_freq_list.tcr_next_to_read = 0;

                // Get Ptr to the free Neighbour meas list.
                // The number of carriers in the list and the list
                // identification are initialized.
                free_list = l1pa_get_free_cres_list_set();

                // Download new list within T_CRES_LIST_PARAM structure.
                free_list->nb_carrier = 1;
                free_list->freq_list[0] = l1_config.tmode.rf_params.mon_arfcn;

                free_list->list_id = 0;

                // Set "flist" with Circuit Swithed BA frequency list parameters
                l1pa_l1ps_com.cres_freq_list.alist = free_list;

                // Reset flags.
                l1pa_l1ps_com.tcr_freq_list.ms_ctrl      = 0;
                l1pa_l1ps_com.tcr_freq_list.ms_ctrl_d    = 0;
                l1pa_l1ps_com.tcr_freq_list.ms_ctrl_dd   = 0;

                // Reset measures made on beacon frequency.
                l1pa_l1ps_com.tcr_freq_list.beacon_meas  = 0;

                // Enable Packet Transfer Neighbour Measurement task.
                l1pa.l1pa_en_meas[TCR_MEAS] |= P_TCRMS_MEAS;
              }

              // Flag packet transfer mode active
              l1tm.tmode_state.packet_transfer_active = TRUE;

              // End of process.
              end_process = 1;
            }
            break;
            
            case TMODE_PDTCH_INFO:
            // TCH result messages.
            //-----------------------
            {
              // Check if RX stats done in PDTCH
              if (l1_config.tmode.rf_params.mon_report == 0)
              {
                BOOL done;

                // loop and stats management done within this function
                l1tm_stats_pdtch_confirm((T_TMODE_PDTCH_INFO *) (msg->SigP));

                done = l1tm_is_rx_counter_done();

                if (done == 1)
                // if done, send stop TBFs
                {
                  // Rise transfer parameter semaphore to prevent L1S to use partial configuration.
                  l1pa_l1ps_com.transfer.semaphore = TRUE;

                  // Enables the TBF release processing in L1S.
                  l1pa_l1ps_com.transfer.tbf_release_param.tbf_release_cmd = TRUE;

                  // Download msg info into L1PA_L1PS_COM.
                  l1pa_l1ps_com.transfer.tbf_release_param.released_tbf = BOTH_TBF;

                  // Clear transfer parameter semaphore to let L1S use the new parameters.
                  l1pa_l1ps_com.transfer.semaphore = FALSE;
                }
              }

              // end of process
              end_process = 1;
            }
            break;

            case L1C_FB_INFO:
            // MON result messages.
            //-----------------------
            {
              // Check if RX stats done in Monitor channel
              if (l1_config.tmode.rf_params.mon_report == 1)
              {
                BOOL done;

                // loop and stats management done within this function
                l1tm_stats_mon_confirm( (T_TMODE_FB_CON*) ((T_L1C_FB_INFO *) (msg->SigP)));

                done = l1tm_is_rx_counter_done();

                if (done == 1)
                // if done, send stop TBFs
                {
                  // Rise transfer parameter semaphore to prevent L1S to use partial configuration.
                  l1pa_l1ps_com.transfer.semaphore = TRUE;

                  // Enables the TBF release processing in L1S.
                  l1pa_l1ps_com.transfer.tbf_release_param.tbf_release_cmd = TRUE;

                  // Download msg info into L1PA_L1PS_COM.
                  l1pa_l1ps_com.transfer.tbf_release_param.released_tbf = BOTH_TBF;

                  // Clear transfer parameter semaphore to let L1S use the new parameters.
                  l1pa_l1ps_com.transfer.semaphore = FALSE;
                }
              }

              // end of process
              end_process = 1;
            }
            break;

            case TMODE_STOP_RX_TX:
            // TBF Release.
            //-------------
            {
              // Rise transfer parameter semaphore to prevent L1S to use partial configuration.
              l1pa_l1ps_com.transfer.semaphore = TRUE;

              // Enables the TBF release processing in L1S.
              l1pa_l1ps_com.transfer.tbf_release_param.tbf_release_cmd = TRUE;

              // Download msg info into L1PA_L1PS_COM.
              l1pa_l1ps_com.transfer.tbf_release_param.released_tbf = BOTH_TBF;
              
              // Clear transfer parameter semaphore to let L1S use the new parameters.
              l1pa_l1ps_com.transfer.semaphore = FALSE;
              
              // end of process.
              end_process = 1;
            }
            break;

            case L1P_TBF_RELEASED:
            // TBF has been release by L1S.
            {
              // Reset TM msg flag
              // No new L1S result messages may be received before a new TM command
              l1tm.tm_msg_received = FALSE;

              // Reset transfer active state
              l1tm.tmode_state.packet_transfer_active = FALSE;

              // Restore stats bitmap
              l1_config.tmode.stats_config.stat_gprs_slots = stat_gprs_slots;

#if (L1_12NEIGH ==1)
              l1a_l1s_com.l1s_en_task[NSYNC] = TASK_DISABLED;
              // Disable neighbour sync 0.
              l1a_l1s_com.nsync.list[0].status = NSYNC_FREE; //Used by l1s_schedule_tasks in l1_sync
#endif

              // Step in state machine.
              *state = RESET;
              
              // End of process.
              end_process = 1;
            }
            break;

            default:
            // End of process.
            //----------------
            {
              end_process = 1;
            }
          } // end of switch(SignalCode)
        } // end of case WAIT_MSG.
      } // end of "switch".
    } // end of "while"
  } // end of procedure.
  #endif

  #if ((L1_STEREOPATH == 1) && (OP_L1_STANDALONE == 1))
    /*----------------------------------------------------------------*/
    /* l1a_tmode_audio_stereopath_process()                           */
    /*----------------------------------------------------------------*/
    /*                                                                */
    /* Description:                                                   */
    /* ------------                                                   */
    /* This function is a state machine which handles the             */
    /* stereopath feature.                                            */
    /*                                                                */
    /* Starting messages:      TMODE_AUDIO_STEREOPATH_DRV_START_REQ   */
    /*                                                                */
    /* Result messages (input):  L1_STEREOPATH_DRV_START_CON          */
    /*                                                                */
    /* Result messages (output): TMODE_AUDIO_STEREOPATH_DRV_START_CON */
    /*                                                                */
    /* Reset messages (input):   none                                 */
    /*                                                                */
    /* Stop message (input):     TMODE_AUDIO_STEREOPATH_DRV_STOP_REQ  */
    /*                           L1_STEREOPATH_DRV_STOP_CON           */
    /*                                                                */
    /* Stop message (output):    TMODE_AUDIO_STEREOPATH_DRV_STOP_CON  */
    /*                                                                */
    /* Rem:                                                           */
    /* ----                                                           */
    /*                                                                */
    /*----------------------------------------------------------------*/
    void l1a_tmode_audio_stereopath_process(xSignalHeaderRec *msg)
    {
#if (CODE_VERSION == NOT_SIMULATION)
      enum states
      {
        RESET             = 0,
        WAIT_START_REQ    = 1,
        WAIT_START_CON    = 2,
        WAIT_STOP         = 3,
        WAIT_DSP_STOP     = 4

      };

      UWORD8    *state      = &l1a.state[TMODE_AUDIO_STEREOPATH_DRV_STATE];
      UWORD32   SignalCode  = msg->SignalCode;

      static UWORD8 previous_config = 0;

      BOOL end_process = 0;

      while(!end_process)
      {
        switch(*state)
        {
          case RESET:
          {
            // initialize global variable
            l1tm.stereopath.stereopath_source_timeout = 0;
            l1tm.stereopath.stereopath_dest_timeout   = 0;
            l1tm.stereopath.stereopath_drop           = 0;
            l1tm.stereopath.stereopath_frame          = 0;
            l1tm.stereopath.stereopath_block          = 0;
            l1tm.stereopath.stereopath_half_block     = 0;
            l1tm.stereopath.stereopath_current_sample = 0;
            l1tm.stereopath.stereopath_buffer_number  = 0;

            // initialize ndb
            stp_drv_ndb->d_cport_api_dma_install   = 0;
            stp_drv_ndb->d_cport_api_dma_channel   = 0;
            stp_drv_ndb->d_cport_api_dma_rootcause = 0;

            // Init DSP background
            l1s_dsp_com.dsp_ndb_ptr->a_background_tasks[C_BGD_STP_DRV] = (API)((C_BGD_STP_DRV<<11) | 1);
            if (l1s_dsp_com.dsp_ndb_ptr->d_max_background<(C_BGD_STP_DRV+1))
              l1s_dsp_com.dsp_ndb_ptr->d_max_background=(API)(C_BGD_STP_DRV+1);


            *state = WAIT_START_REQ;
          }
          break;

          case WAIT_START_REQ:
          {
            if (SignalCode == TMODE_AUDIO_STEREOPATH_START_REQ)
            {
              // receive a request to start stereopath
              T_TMODE_AUDIO_STEREOPATH_START_REQ* tmode_audio_sp_conf_ptr;

              // Flag msg received
              l1tm.tm_msg_received = TRUE;

              /******************************************************************/
              /**************** GET STEREOPATH PARAMETERS ***********************/
              /******************************************************************/

              if (((T_TMODE_AUDIO_STEREOPATH_START_REQ *)(msg->SigP))->configuration == AUDIO_SP_SELF_CONF)
              {
                // no use of a predefined configuration, we have to get parameters from the message
                tmode_audio_sp_conf_ptr =  ((T_TMODE_AUDIO_STEREOPATH_START_REQ *)(msg->SigP));
              }
              else
              {
                UWORD8 conf_index = 0;

                // use of a predefined configuration, we have to get parameters from the constant config
                tmode_audio_sp_conf_ptr = (T_TMODE_AUDIO_STEREOPATH_START_REQ *) tmode_audio_sp_conf[conf_index];

                while ((tmode_audio_sp_conf_ptr != NULL) && (conf_index < NB_MAX_STEREOPATH_CONFIG))
                {
                  if (tmode_audio_sp_conf_ptr->configuration == ((T_TMODE_AUDIO_STEREOPATH_START_REQ *)(msg->SigP))->configuration)
                    break;

                  tmode_audio_sp_conf_ptr = (T_TMODE_AUDIO_STEREOPATH_START_REQ *) tmode_audio_sp_conf[++conf_index];
                }
              }

              if (tmode_audio_sp_conf_ptr == NULL)
              {
                // unknow configuration identifier --> use message parameters
                tmode_audio_sp_conf_ptr =  ((T_TMODE_AUDIO_STEREOPATH_START_REQ *)(msg->SigP));
              }

              // Download the stereopath description in the l1a_l1s structure.
              l1a_l1s_com.stereopath_drv_task.parameters.sampling_frequency    =  tmode_audio_sp_conf_ptr->sampling_frequency;
              l1a_l1s_com.stereopath_drv_task.parameters.DMA_allocation        =  tmode_audio_sp_conf_ptr->DMA_allocation;
              l1a_l1s_com.stereopath_drv_task.parameters.DMA_channel_number    =  tmode_audio_sp_conf_ptr->DMA_channel_number;
              l1a_l1s_com.stereopath_drv_task.parameters.data_type             =  tmode_audio_sp_conf_ptr->data_type;
              l1a_l1s_com.stereopath_drv_task.parameters.source_port           =  tmode_audio_sp_conf_ptr->source_port;
              l1a_l1s_com.stereopath_drv_task.parameters.element_number        =  tmode_audio_sp_conf_ptr->element_number;
              l1a_l1s_com.stereopath_drv_task.parameters.frame_number          =  tmode_audio_sp_conf_ptr->frame_number;
              l1a_l1s_com.stereopath_drv_task.parameters.mono_stereo           =  tmode_audio_sp_conf_ptr->mono_stereo;
              l1a_l1s_com.stereopath_drv_task.parameters.feature_identifier    =  AUDIO_SP_TESTS_ID;

              /******************************************************************/
              /**************** CHECK ALLOCATION DSP/MCU ************************/
              /******************************************************************/

              if (tmode_audio_sp_conf_ptr->DMA_allocation == AUDIO_SP_DMA_ALLOC_MCU)
              {
                l1a_l1s_com.stereopath_drv_task.parameters.DMA_int_callback_fct = l1tm_stereopath_DMA_handler;
              }
              else // DMA_allocation == AUDIO_SP_DMA_ALLOC_DSP
              {
                // Update ndb
                stp_drv_ndb->d_cport_api_dma_install   = 1;
                stp_drv_ndb->d_cport_api_dma_channel   = l1a_l1s_com.stereopath_drv_task.parameters.DMA_channel_number;
                stp_drv_ndb->d_cport_api_dma_rootcause = 0;

                // start background task
                l1s_dsp_com.dsp_ndb_ptr->d_background_enable|=(API)(1<<C_BGD_STP_DRV);
                l1_trigger_api_interrupt();

                l1a_l1s_com.stereopath_drv_task.parameters.DMA_int_callback_fct = f_dma_default_call_back_it;
              }

              /******************************************************************/
              /**************** GENERATION OF THE PATTERN ***********************/
              /******************************************************************/

              // Reservation and generation of the pattern used to fill the buffer
              if (tmode_audio_sp_conf_ptr->pattern_identifier != AUDIO_SP_SILENCE_PATTERN)
              {
                l1tm.stereopath.stereopath_pattern = (WORD8 *)l1tm_stereopath_buffer;

                // if pattern has already been build with the same config (mp3,midi or ext audio) in the current scenario,
                // we don't do it again. This is to avoid to have a CPU overload during critical operation such as access or packet transfer
                if ((tmode_audio_sp_conf_ptr->configuration == 0) || (tmode_audio_sp_conf_ptr->configuration != previous_config))
                {
                  previous_config = tmode_audio_sp_conf_ptr->configuration;
                  l1tm.stereopath.stereopath_nb_samples = l1tm_stereopath_get_pattern(l1tm_stereopath_sampling_freqs[tmode_audio_sp_conf_ptr->sampling_frequency],
                                                                                      l1tm_stereopath_sin_freqs[tmode_audio_sp_conf_ptr->pattern_identifier][0],
                                                                                      l1tm_stereopath_sin_freqs[tmode_audio_sp_conf_ptr->pattern_identifier][1], 
                                                                                      tmode_audio_sp_conf_ptr->data_type);
                }
              }
              else
              {
                // Silence pattern, consider just 2 samples at the value 0
                l1tm.stereopath.stereopath_nb_samples = 2;

                l1tm.stereopath.stereopath_pattern = (WORD8 *)l1tm_stereopath_buffer;
                l1tm.stereopath.stereopath_pattern[0] = l1tm.stereopath.stereopath_pattern[1] = 
                l1tm.stereopath.stereopath_pattern[2] = l1tm.stereopath.stereopath_pattern[3] = 0x0000;
              }

              /******************************************************************/
              /**************** GET ADDRESS OF THE BUFFER ***********************/
              /******************************************************************/
#if (CHIPSET == 15)
             if (tmode_audio_sp_conf_ptr->source_port == AUDIO_SP_SOURCE_EMIF)
              {
                // get an address in internal RAM
                l1a_l1s_com.stereopath_drv_task.parameters.source_buffer_address = (WORD8*) TM_stereo_buf_ext_mem;   
              }


              if (tmode_audio_sp_conf_ptr->source_port == AUDIO_SP_SOURCE_IMIF)
              {
                // get an address in internal RAM
                l1a_l1s_com.stereopath_drv_task.parameters.source_buffer_address = (WORD8*) TM_stereo_buf;   
              }

	      if (tmode_audio_sp_conf_ptr->source_port == AUDIO_SP_SOURCE_API)
              {
                // Disable DSP trace
                l1s_dsp_com.dsp_ndb_ptr->d_debug_trace_type &= 0xfff0;
                l1s_dsp_com.dsp_ndb_ptr->d_debug_trace_type |= 0x8000;

                l1a_l1s_com.stereopath_drv_task.parameters.source_buffer_address = (WORD8*)API_address_dsp2mcu(C_STP_DRV_BUF_API_BASE_ADDRESS);
              }
#else
 	  if (tmode_audio_sp_conf_ptr->source_port == AUDIO_SP_SOURCE_IMIF)
              {
                // get an address in internal RAM
                l1a_l1s_com.stereopath_drv_task.parameters.source_buffer_address = (WORD8*) TM_stereo_buf;   
              }
              else // source_port == AUDIO_SP_SOURCE_API
              {
                // Disable DSP trace
                l1s_dsp_com.dsp_ndb_ptr->d_debug_trace_type &= 0xfff0;
                l1s_dsp_com.dsp_ndb_ptr->d_debug_trace_type |= 0x8000;

                l1a_l1s_com.stereopath_drv_task.parameters.source_buffer_address = (WORD8*)API_address_dsp2mcu(C_STP_DRV_BUF_API_BASE_ADDRESS);
              }
#endif

              /******************************************************************/
              /**************** FILL THE 2 FIRST BUFFERS ************************/
              /******************************************************************/

              l1tm_stereopath_fill_buffer((void*) l1a_l1s_com.stereopath_drv_task.parameters.source_buffer_address);
              l1tm_stereopath_fill_buffer((void*) l1a_l1s_com.stereopath_drv_task.parameters.source_buffer_address);

              // Start the L1S stereopath task
              l1a_l1s_com.stereopath_drv_task.command.start = TRUE;

              *state = WAIT_START_CON;
            }

            // End process
            end_process = 1;
          }
          break;

          case WAIT_START_CON:
          {
            if (SignalCode == L1_STEREOPATH_DRV_START_CON)
            {
              // Send the start confirmation message
              l1a_audio_send_confirmation(TMODE_AUDIO_STEREOPATH_START_CON);

              *state = WAIT_STOP;
            }

            // End process
            end_process = 1;
          }
          break;

          case WAIT_STOP:
          {
            if (SignalCode == TMODE_AUDIO_STEREOPATH_STOP_REQ)
            {
             if (l1a_l1s_com.stereopath_drv_task.parameters.DMA_allocation == AUDIO_SP_DMA_ALLOC_DSP)
             {
               // we first need to stop the DSP background task
               stp_drv_ndb->d_cport_api_dma_install   = 0xFFFF;
               l1_trigger_api_interrupt();

               *state = WAIT_DSP_STOP;
             }
             else
             {
               // Stop the L1S stereopath task
               l1a_l1s_com.stereopath_drv_task.command.stop = TRUE;

               // End process
               end_process = 1;
             }
            }
            else
            if (SignalCode == L1_STEREOPATH_DRV_STOP_CON)
            {
              // Reset TM msg flag
              // No new L1S result messages may be received before a new TM command
              l1tm.tm_msg_received = FALSE;

              free(l1tm.stereopath.stereopath_pattern);

              // Send the stop confirmation message
              l1a_audio_send_confirmation(TMODE_AUDIO_STEREOPATH_STOP_CON);

              *state = RESET;
            }
            else
            {
              // End process
              end_process = 1;
            }
          }
          break;

          case WAIT_DSP_STOP:
          {
            if (stp_drv_ndb->d_cport_api_dma_install == 0)
            {
              // stop the DSP background task
              l1s_dsp_com.dsp_ndb_ptr->d_background_enable&=(API)(~(1<<C_BGD_STP_DRV));
              l1_trigger_api_interrupt();

               // Stop the L1S stereopath task
               l1a_l1s_com.stereopath_drv_task.command.stop = TRUE;

              *state = WAIT_STOP;

              end_process = 1;
            }
          }
          break;
        } // switch
      } // while(!end_process)
#endif  // CODE_VERSION == NOT_SIMULATION
    }
  #endif //   #if ((L1_STEREOPATH == 1) && (OP_L1_STANDALONE == 1))
#endif

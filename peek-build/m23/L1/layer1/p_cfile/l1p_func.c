/************* Revision Controle System Header *************
 *                  GSM Layer 1 software
 * L1P_FUNC.C
 *
 *        Filename l1p_func.c
 *  Copyright 2003 (C) Texas Instruments
 *
 ************* Revision Controle System Header *************/

#define  L1P_FUNC_C

#include "l1_macro.h"
#include "l1_confg.h"

#if L1_GPRS

#if (CODE_VERSION == SIMULATION)
  #include "stddef.h"
#endif

#include "l1_types.h"
#include "sys_types.h"
#include "l1_const.h"


#if TESTMODE
  #include "l1tm_defty.h"
#endif
#if (AUDIO_TASK == 1)
  #include "l1audio_const.h"
  #include "l1audio_cust.h"
  #include "l1audio_defty.h"
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
#include "l1_varex.h"

#include "cust_os.h"
#include "l1_msgty.h"
#include "l1_time.h"

#include "l1p_cons.h"
#include "l1p_msgt.h"
#include "l1p_deft.h"
#include "l1p_vare.h"
#include "l1p_sign.h"

#if(RF_FAM == 61)
	#include "l1_rf61.h"
#endif
#if (CODE_VERSION == SIMULATION)
  #include "l1_rf2.h"
#endif

#include "tpudrv61.h"
/*-------------------------------------------------------*/
/* Prototypes of external functions used in this file.   */
/*-------------------------------------------------------*/

void  l1pddsp_meas_ctrl    (UWORD8 nbmeas, UWORD8 pm_pos);
void l1dtpu_meas           (UWORD16 radio_freq,WORD8 agc,UWORD8  lna_off,
                            UWORD16 win_id,UWORD16 tpu_synchro, UWORD8 adc_active
#if (RF_FAM == 61)
                           ,UWORD8 afc_mode
                            ,UWORD8 if_ctl
#endif
                                            );
WORD8 Cust_get_agc_from_IL (UWORD16 radio_freq, UWORD16 agc_index, UWORD8 table_id, UWORD8 lna_off_val);
void  l1ps_macs_init       (void);

/*-------------------------------------------------------*/
/* initialize_l1pvar()                                   */
/*-------------------------------------------------------*/
/* Parameters  :                                         */
/* -------------                                         */
/* Return      :                                         */
/* -------------                                         */
/* Description :                                         */
/* -------------                                         */
/* This routine is used to initialize the l1pa, l1ps and */
/* l1pa_l1ps_com global structures.                      */
/*-------------------------------------------------------*/
void initialize_l1pvar(void)
{
  UWORD8 i;
  
  //++++++++++++++++++++++++++++++++++++++++++
  //  Reset "l1ps" structure.
  //++++++++++++++++++++++++++++++++++++++++++

  l1ps.last_PR_good     = 0;
  l1ps.ILmin_beacon     = 255;
  l1ps.read_param.assignment_id = 0xFF; /* do not return non initialized value to RLC */

  for(i = 0; i < 8; i++)
    l1ps.ILmin_others[i] = l1_config.params.il_min;

  //++++++++++++++++++++++++++++++++++++++++++
  //  Reset "l1pa" structure.
  //++++++++++++++++++++++++++++++++++++++++++

  for(i=0;i<NBR_L1PA_PROCESSES;i++)
  {
    l1pa.state[i]        = 0;
    l1pa.l1pa_en_meas[i] = 0;
  }

  //++++++++++++++++++++++++++++++++++++++++++
  //  Reset "l1pa_l1ps_com" structure.
  //++++++++++++++++++++++++++++++++++++++++++

  // Initialize PC_MEAS_CHAN flag
  l1ps.pc_meas_chan_ctrl = FALSE;

  // Initialize active list used in Neighbour Measurement Transfer Process
  l1pa_l1ps_com.cres_freq_list.alist = &(l1pa_l1ps_com.cres_freq_list.list[0]);

  // Initialize parameters used in Neighbour Measurement Transfer Process
  l1pa_l1ps_com.cres_freq_list.alist->nb_carrier = 0;
  l1pa_l1ps_com.tcr_freq_list.new_list_present = FALSE;

  l1pa_l1ps_com.transfer.semaphore = TRUE;
  l1pa_l1ps_com.transfer.aset      = &(l1pa_l1ps_com.transfer.set[0]);
  l1pa_l1ps_com.transfer.fset[0]   = &(l1pa_l1ps_com.transfer.set[1]);
  l1pa_l1ps_com.transfer.fset[1]   = &(l1pa_l1ps_com.transfer.set[2]);

  // Initialize Downlink Power Control Struture. Set CRC to BAD, bcch_level
  // and burst_level[] to INVALID.
  l1pa_l1ps_com.transfer.dl_pwr_ctrl.crc_error  = TRUE;
  l1pa_l1ps_com.transfer.dl_pwr_ctrl.bcch_level = (WORD8)0x80;//omaps00090550

  for(i = 0; i < 4; i++)
  {
    l1pa_l1ps_com.transfer.dl_pwr_ctrl.burst_level[i] = (WORD8)0x80;//omaps00090550
  }

  l1pa_l1ps_com.transfer.set[0].ul_tbf_alloc = &(l1pa_l1ps_com.transfer.ul_tbf_alloc[0]);
  l1pa_l1ps_com.transfer.set[1].ul_tbf_alloc = &(l1pa_l1ps_com.transfer.ul_tbf_alloc[1]);
  l1pa_l1ps_com.transfer.set[2].ul_tbf_alloc = &(l1pa_l1ps_com.transfer.ul_tbf_alloc[2]);
  
  for(i=0;i<3;i++)
  {
    l1pa_l1ps_com.transfer.set[i].SignalCode                   = 0;
    l1pa_l1ps_com.transfer.set[i].dl_tbf_synchro_timeslot      = 0;
    l1pa_l1ps_com.transfer.set[i].dl_tbf_synchro_timeslot      = 0;
    l1pa_l1ps_com.transfer.set[i].transfer_synchro_timeslot    = 0;
    l1pa_l1ps_com.transfer.set[i].allocated_tbf                = NO_TBF;
    l1pa_l1ps_com.transfer.set[i].assignment_command           = NO_TBF;
    l1pa_l1ps_com.transfer.set[i].multislot_class              = 0;

    l1pa_l1ps_com.transfer.set[i].packet_ta.ta                 = 255;
    l1pa_l1ps_com.transfer.set[i].packet_ta.ta_index           = 255;
    l1pa_l1ps_com.transfer.set[i].packet_ta.ta_tn              = 255;

    l1pa_l1ps_com.transfer.set[i].tsc                          = 0;

    l1pa_l1ps_com.transfer.set[i].freq_param.chan_sel.h        = 0;
    l1pa_l1ps_com.transfer.set[i].freq_param.chan_sel.
                               rf_channel.single_rf.radio_freq = 0;

    l1pa_l1ps_com.transfer.set[i].tbf_sti.present              = FALSE;

    l1pa_l1ps_com.transfer.set[i].mac_mode                     = 0;

    l1pa_l1ps_com.transfer.set[i].ul_tbf_alloc->tfi            = 255;
    l1pa_l1ps_com.transfer.set[i].dl_tbf_alloc.tfi             = 255;

    l1pa_l1ps_com.transfer.set[i].dl_pwr_ctl.p0                = 255;
    l1pa_l1ps_com.transfer.set[i].dl_pwr_ctl.bts_pwr_ctl_mode  = 0;
    l1pa_l1ps_com.transfer.set[i].dl_pwr_ctl.pr_mode           = 0;
  }

  //++++++++++++++++++++++++++++++++++++++++++
  //  Reset "l1pa_macs_com" structure.
  //++++++++++++++++++++++++++++++++++++++++++

  l1ps_macs_com.fix_alloc_exhaust_flag = FALSE;
  l1ps_macs_com.rlc_downlink_call      = FALSE;
  #if FF_L1_IT_DSP_USF
    l1ps_macs_com.usf_status           = USF_AVAILABLE;
  #endif
  #if L1_EDA
    l1ps_macs_com.fb_sb_task_enabled   = FALSE;
    l1ps_macs_com.fb_sb_task_detect    = FALSE;
  #endif

  //++++++++++++++++++++++++++++++++++++++++++
  //  Reset MAC-S static structure.
  //++++++++++++++++++++++++++++++++++++++++++
  l1ps_macs_init();

  //++++++++++++++++++++++++++++++++++++++++++
  //  Reset packet transfer mode commands.
  //++++++++++++++++++++++++++++++++++++++++++

  l1pa_l1ps_com.transfer.ptcch.ta_update_cmd                 = FALSE;
  l1pa_l1ps_com.transfer.psi_param.psi_param_update_cmd      = FALSE;
  l1pa_l1ps_com.transfer.tbf_release_param.tbf_release_cmd   = FALSE;
  l1pa_l1ps_com.transfer.pdch_release_param.pdch_release_cmd = FALSE;
  l1pa_l1ps_com.transfer.repeat_alloc.repeat_allocation      = FALSE;
}

/*-------------------------------------------------------*/
/* l1ps_reset_db_mcu_to_dsp()                            */
/*-------------------------------------------------------*/
/* Parameters :                                          */
/* Return     :                                          */
/* Functionality :                                       */
/*-------------------------------------------------------*/
void l1ps_reset_db_mcu_to_dsp(T_DB_MCU_TO_DSP_GPRS *page_ptr)
{
  API i;
  API size = sizeof(T_DB_MCU_TO_DSP_GPRS) / sizeof(API);
  API *ptr = (API *)page_ptr;

  // Clear all locations.
  for(i=0; i<size; i++) *ptr++ = 0;
} 
 
/*-------------------------------------------------------*/
/* l1ps_reset_db_dsp_to_mcu()                            */
/*-------------------------------------------------------*/
/* Parameters :                                          */
/* Return     :                                          */
/* Functionality :                                       */
/*-------------------------------------------------------*/
void l1ps_reset_db_dsp_to_mcu(T_DB_DSP_TO_MCU *page_ptr)
{
  API i;
  API size = sizeof(T_DB_DSP_TO_MCU_GPRS) / sizeof(API);
  API *ptr = (API *)page_ptr;
 
  // Clear all locations.
  for(i=0; i<size; i++) *ptr++ = 0;
  
  // Set crc result as "SB not found".
  page_ptr->a_sch[0]  =  (1<<B_SCH_CRC);   // B_SCH_CRC =1, BLUD =0
}  

/*-------------------------------------------------------*/
/* l1ps_swap_iq_dl()                                     */
/*-------------------------------------------------------*/
/* Parameters :                                          */
/* Return     :                                          */
/* Functionality :                                       */
/*-------------------------------------------------------*/
BOOL l1ps_swap_iq_dl(UWORD16 radio_freq)
{ 
  UWORD8   swap_iq;
  BOOL     swap_flag=FALSE;  //omaps00090550;

#if (L1_FF_MULTIBAND == 0)
  
  if(((l1_config.std.id == DUAL) || (l1_config.std.id == DUALEXT) || (l1_config.std.id == DUAL_US)) && 
     (radio_freq >= l1_config.std.first_radio_freq_band2)) 
  {
    swap_iq = l1_config.std.swap_iq_band2;
  }
  else
  {  
    swap_iq = l1_config.std.swap_iq_band1;
  }
  
#else // L1_FF_MULTIBAND = 1 below

  UWORD16 physical_band_id;
  physical_band_id = 
    l1_multiband_radio_freq_convert_into_physical_band_id(radio_freq);
  swap_iq = rf_band[physical_band_id].swap_iq;
  
#endif // #if (L1_FF_MULTIBAND == 0) else
  

  switch(swap_iq)
  {
    case 0:  /* No swap at all. */
    case 2:  /* DL, no swap.    */
      swap_flag = FALSE;
    break;
    case 1:  /* DL I/Q swap.    */
    case 3:  /* DL I/Q swap.    */
      swap_flag = TRUE;
    break;
  }
  return(swap_flag);
}

/*-------------------------------------------------------*/
/* l1ps_swap_iq_ul()                                     */
/*-------------------------------------------------------*/
/* Parameters :                                          */
/* Return     :                                          */
/* Functionality :                                       */
/*-------------------------------------------------------*/
BOOL l1ps_swap_iq_ul(UWORD16 radio_freq)
{ 
  UWORD8   swap_iq;
  BOOL     swap_flag =FALSE;  //omaps00090550

#if (L1_FF_MULTIBAND == 0)

  if(((l1_config.std.id == DUAL) || (l1_config.std.id == DUALEXT) || (l1_config.std.id == DUAL_US)) && 
     (radio_freq >= l1_config.std.first_radio_freq_band2)) 
  {
    swap_iq = l1_config.std.swap_iq_band2;
  }
  else
  {
    swap_iq = l1_config.std.swap_iq_band1;
  }
  
#else // L1_FF_MULTIBAND = 1 below

 UWORD16 physical_band_id = 0;
  physical_band_id = 
    l1_multiband_radio_freq_convert_into_physical_band_id(radio_freq);
  swap_iq = rf_band[physical_band_id].swap_iq;
  
#endif // #if (L1_FF_MULTIBAND == 0) else
  
  switch(swap_iq)
  {
    case 0: /* No swap at all. */
    case 1: /* UL, no swap.    */
      swap_flag = FALSE;
    break;
    case 2: /* UL I/Q swap.    */
    case 3: /* UL I/Q swap.    */
      swap_flag = TRUE;
    break;
  }
  return(swap_flag);
}

/*-------------------------------------------------------*/
/* l1ps_tcr_ctrl()                                       */
/*-------------------------------------------------------*/
/* Parameters :                                          */
/* Return     :                                          */
/* Functionality :                                       */
/*-------------------------------------------------------*/
void l1ps_tcr_ctrl(UWORD8 pm_position)
{
  UWORD16  radio_freq_ctrl;
  UWORD8   lna_off;
  WORD8    agc;
  
#if(L1_FF_MULTIBAND == 1)
  UWORD16 operative_radio_freq;
#endif
  
  UWORD8   mode = PACKET_TRANSFER;
  UWORD8 input_level;
  #if (RF_FAM == 61)
    UWORD16 dco_algo_ctl_pw = 0;
    UWORD8 if_ctl = 0;
	UWORD8 if_threshold = C_IF_ZERO_LOW_THRESHOLD_GSM;
  #endif

  radio_freq_ctrl = l1pa_l1ps_com.cres_freq_list.alist->freq_list[l1pa_l1ps_com.tcr_freq_list.tcr_next_to_ctrl];

  // Get AGC according to the last known IL.
#if(L1_FF_MULTIBAND == 0)
  
  input_level = l1a_l1s_com.last_input_level[radio_freq_ctrl - l1_config.std.radio_freq_index_offset].input_level;
  lna_off = l1a_l1s_com.last_input_level[radio_freq_ctrl - l1_config.std.radio_freq_index_offset].lna_off;
  agc     = Cust_get_agc_from_IL(radio_freq_ctrl, input_level >> 1, PWR_ID, lna_off);

#else // L1_FF_MULTIBAND = 1 below

  operative_radio_freq = 
    l1_multiband_radio_freq_convert_into_operative_radio_freq(radio_freq_ctrl);
  input_level = 
    l1a_l1s_com.last_input_level[operative_radio_freq].input_level;
  lna_off = 
    l1a_l1s_com.last_input_level[operative_radio_freq].lna_off;
  agc     = 
    Cust_get_agc_from_IL(radio_freq_ctrl, input_level >> 1, PWR_ID, lna_off);

#endif // #if(L1_FF_MULTIBAND == 0) else 

  

   #if (RF_FAM == 61) // Locosto DCO
 #if (PWMEAS_IF_MODE_FORCE == 0)        
        cust_get_if_dco_ctl_algo(&dco_algo_ctl_pw, &if_ctl, (UWORD8) L1_IL_VALID , 
            input_level,
            l1pa_l1ps_com.p_idle_param.radio_freq, if_threshold);
      #else     
        if_ctl = IF_120KHZ_DSP;
        dco_algo_ctl_pw = DCO_IF_0KHZ;    
      #endif  
   	 

      l1ddsp_load_dco_ctl_algo_pw(dco_algo_ctl_pw);
      l1s.tcr_prog_done=1;
   #endif

#if (L1_FF_MULTIBAND == 0)

  // Memorize the IL and LNA used for AGC setting.
  l1pa_l1ps_com.tcr_freq_list.used_il_lna.il  = l1a_l1s_com.last_input_level[radio_freq_ctrl - l1_config.std.radio_freq_index_offset].input_level;
  l1pa_l1ps_com.tcr_freq_list.used_il_lna.lna = l1a_l1s_com.last_input_level[radio_freq_ctrl - l1_config.std.radio_freq_index_offset].lna_off;

#else // L1_FF_MULTIBAND = 1 below

  // Memorize the IL and LNA used for AGC setting.
  l1pa_l1ps_com.tcr_freq_list.used_il_lna.il  = 
    l1a_l1s_com.last_input_level[operative_radio_freq].input_level;
  l1pa_l1ps_com.tcr_freq_list.used_il_lna.lna = 
    l1a_l1s_com.last_input_level[operative_radio_freq].lna_off;

#endif // #if (L1_FF_MULTIBAND == 0) else

  // tpu pgm: 1 measurement only.
  l1dtpu_meas(radio_freq_ctrl,
              agc,
              lna_off,
              l1s.tpu_win,
              l1s.tpu_offset,INACTIVE
#if(RF_FAM == 61)
            ,L1_AFC_SCRIPT_MODE
            ,if_ctl
#endif
  );

  // Increment tpu window identifier.
  l1s.tpu_win += (l1_config.params.rx_synth_load_split + PWR_LOAD);

  // increment carrier counter for next measurement...
  if(++l1pa_l1ps_com.tcr_freq_list.tcr_next_to_ctrl >= l1pa_l1ps_com.cres_freq_list.alist->nb_carrier) 
    l1pa_l1ps_com.tcr_freq_list.tcr_next_to_ctrl = 0;

  // Program DSP, in order to performed 1 measure.
  // Second argument specifies PW position.
  l1pddsp_meas_ctrl(1, pm_position);

  #if (TRACE_TYPE!=0) 
    //trace_fct(CST_CTRL_TRANSFER_MEAS, radio_freq_ctrl);
  #endif

  // Update d_debug timer
  l1s_dsp_com.dsp_db_w_ptr->d_debug = (l1s.debug_time + 2) ;


  // Flag measurement control.
  // **************************

  // Set flag "ms_ctrl" to nb_meas_to_perform.
  // It will be used as 2 tdma delayed to trigger Read phase.
  l1pa_l1ps_com.tcr_freq_list.ms_ctrl = 1;

  // Flag DSP and TPU programmation.
  // ********************************

  // Set "CTRL_MS" flag in the controle flag register.
  l1s.tpu_ctrl_reg |= CTRL_MS;
  l1s.dsp_ctrl_reg |= CTRL_MS;

}

/*-------------------------------------------------------*/
/* l1ps_bcch_meas_ctrl()                                 */
/*-------------------------------------------------------*/
/* Parameters :                                          */
/* Return     :                                          */
/* Functionality :                                       */
/*-------------------------------------------------------*/
void l1ps_bcch_meas_ctrl(UWORD8 ts)
{
  UWORD8   lna_off;
  WORD8    agc;
#if(L1_FF_MULTIBAND == 1)
  UWORD16 operative_radio_freq = 0;
#endif
  
  UWORD8   mode = PACKET_TRANSFER;
  UWORD8 input_level;
  #if (RF_FAM == 61)
    UWORD16 dco_algo_ctl_pw =0;
    UWORD8 if_ctl=0;
	UWORD8 if_threshold = C_IF_ZERO_LOW_THRESHOLD_GSM;
  #endif
 

  if ((l1s.dsp_ctrl_reg & CTRL_ABORT) == 0)
  {
    #define radio_freq_ctrl l1a_l1s_com.Scell_info.radio_freq

#if(L1_FF_MULTIBAND == 0)

    // Get AGC according to the last known IL.
    input_level = l1a_l1s_com.last_input_level[radio_freq_ctrl - l1_config.std.radio_freq_index_offset].input_level;
    lna_off = l1a_l1s_com.last_input_level[radio_freq_ctrl - l1_config.std.radio_freq_index_offset].lna_off;
    agc     = Cust_get_agc_from_IL(radio_freq_ctrl, input_level >> 1, PWR_ID, lna_off);
    // Memorize the IL and LNA used for AGC setting.
    // Note: the same structure as for TCR meas is used for PC_MEAS_CHAN measurements
    l1pa_l1ps_com.tcr_freq_list.used_il_lna.il  =  input_level;
    l1pa_l1ps_com.tcr_freq_list.used_il_lna.lna = l1a_l1s_com.last_input_level[radio_freq_ctrl - l1_config.std.radio_freq_index_offset].lna_off;

#else // L1_FF_MULTIBAND = 1 below

    operative_radio_freq = 
      l1_multiband_radio_freq_convert_into_operative_radio_freq(radio_freq_ctrl);

     // Get AGC according to the last known IL.
    input_level = 
      l1a_l1s_com.last_input_level[operative_radio_freq].input_level;
    lna_off = 
      l1a_l1s_com.last_input_level[operative_radio_freq].lna_off;
    agc     = 
      Cust_get_agc_from_IL(radio_freq_ctrl, input_level >> 1, PWR_ID, lna_off);

    // Memorize the IL and LNA used for AGC setting.
    // Note: the same structure as for TCR meas is used for PC_MEAS_CHAN measurements
    l1pa_l1ps_com.tcr_freq_list.used_il_lna.il  =  input_level;
    l1pa_l1ps_com.tcr_freq_list.used_il_lna.lna = 
        l1a_l1s_com.last_input_level[operative_radio_freq].lna_off;

#endif // #if(L1_FF_MULTIBAND == 0) else
   

   #if (RF_FAM == 61) // Locosto DCO
      cust_get_if_dco_ctl_algo(&dco_algo_ctl_pw, &if_ctl, (UWORD8) L1_IL_VALID, 
                                             input_level,
                                             radio_freq_ctrl,if_threshold);
   
      l1ddsp_load_dco_ctl_algo_pw(dco_algo_ctl_pw);
   #endif


    // tpu pgm: 1 measurement only.
    l1dtpu_meas(radio_freq_ctrl,
                agc,
                lna_off,
                l1s.tpu_win,
                l1s.tpu_offset,INACTIVE
#if(RF_FAM == 61)
                  ,L1_AFC_SCRIPT_MODE
                  ,if_ctl
#endif
	                );

    // Increment tpu window identifier.
    l1s.tpu_win += (l1_config.params.rx_synth_load_split + PWR_LOAD);

    // Program DSP, in order to performed 1 measure.
    // Second argument specifies PW position.
    l1pddsp_meas_ctrl(1, (UWORD8)ts);

    #if (TRACE_TYPE!=0) && (TRACE_TYPE!=5)
      //trace_fct(CST_CTRL_SCELL_TRANSFER_MEAS, radio_freq_ctrl);
    #endif

    // Update d_debug timer
    l1s_dsp_com.dsp_db_w_ptr->d_debug = (l1s.debug_time + 2) ;


    // Flag measurement control.
    // **************************

    l1ps.pc_meas_chan_ctrl = TRUE;

    // Flag DSP and TPU programmation.
    // ********************************

    // Set "CTRL_MS" flag in the controle flag register.
    l1s.tpu_ctrl_reg |= CTRL_MS;
    l1s.dsp_ctrl_reg |= CTRL_MS;
  }
}

/*-------------------------------------------------------*/
/* l1ps_update_read_set_parameters()                     */
/*-------------------------------------------------------*/
/* Parameters :                                          */
/* Return     :                                          */
/* Functionality : Updating of the "Read_param" structure*/
/*   usefull in case the aset structure has been updated */
/*   before the last read of the current block           */
/*-------------------------------------------------------*/
void l1ps_update_read_set_parameters(void)
{
  #define READ_PARAM l1ps.read_param
  #define ASET       l1pa_l1ps_com.transfer.aset 

  // Copy of the "aset" parameters in the "read_param" structure
  READ_PARAM.dl_tn         = l1a_l1s_com.dl_tn;
  READ_PARAM.new_set       = 0;
  READ_PARAM.assignment_id = ASET->assignment_id;
  READ_PARAM.allocated_tbf = ASET->allocated_tbf;
  READ_PARAM.dl_tfi        = ASET->dl_tbf_alloc.tfi;
  READ_PARAM.ul_tfi        = ASET->ul_tbf_alloc->tfi;
  READ_PARAM.dl_pwr_ctl    = ASET->dl_pwr_ctl;
  READ_PARAM.pc_meas_chan  = ASET->pc_meas_chan;

  // We need to know on which frequency band we work for LNA state processing
  if (!l1pa_l1ps_com.transfer.aset->freq_param.chan_sel.h)
  {
    // Single frequency
    READ_PARAM.radio_freq_for_lna = l1pa_l1ps_com.transfer.aset->freq_param.chan_sel.rf_channel.single_rf.radio_freq;
  }
  else
  {
    // Frequency hopping: all frequencies of the frequency list are on the same band
    // We take the first frequency of the list
    READ_PARAM.radio_freq_for_lna = l1pa_l1ps_com.transfer.aset->freq_param.freq_list.rf_chan_no.A[0];
  }
}
#endif

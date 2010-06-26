/************* Revision Controle System Header *************
 *                  GSM Layer 1 software
 * L1P_CMPL.C
 *
 *        Filename l1p_cmpl.c
 *  Copyright 2003 (C) Texas Instruments
 *
 ************* Revision Controle System Header *************/

#define  L1P_CMPL_C

//#pragma DUPLICATE_FOR_INTERNAL_RAM_START

#include "l1_macro.h"
#include "l1_confg.h"

#if L1_GPRS
#if (CODE_VERSION == SIMULATION)
  #include <string.h>
  #include "l1_types.h"
  #include "sys_types.h"
  #include "l1_const.h"
  #include "l1_time.h"
  #include "l1_signa.h"
  #if TESTMODE
    #include "l1tm_defty.h"
  #endif
  #if (AUDIO_TASK == 1)
    #include "l1audio_const.h"
    #include "l1audio_cust.h"
    #include "l1audio_signa.h"
    #include "l1audio_defty.h"
    #include "l1audio_msgty.h"
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
  #include "cust_os.h"
  #include "l1_msgty.h"
  #include "l1_varex.h"
  #include "l1_proto.h"
  #include "l1_tabs.h"
  #include "l1_trace.h"
  #if L2_L3_SIMUL
    #include "l2_l3.h"
    #include "hw_debug.h"
  #endif

  #include "l1p_cons.h"
  #include "l1p_msgt.h"
  #include "l1p_deft.h"
  #include "l1p_vare.h"
  #include "l1p_sign.h"

  #if TESTMODE
    #include "l1tm_msgty.h"
    #include "l1tm_signa.h"
  #endif

  #include "macs_def.h"
  #include "macs_cst.h"

  #include "sim_cons.h"
  #include "sim_def.h"
  extern T_hw FAR hw;

#else
  #include <string.h>
  #include "l1_types.h"
  #include "sys_types.h"
  #include "l1_const.h"
  #include "l1_time.h"
  #include "l1_signa.h"
  #if TESTMODE
    #include "l1tm_defty.h"
  #endif
  #if (AUDIO_TASK == 1)
    #include "l1audio_const.h"
    #include "l1audio_cust.h"
    #include "l1audio_signa.h"
    #include "l1audio_defty.h"
    #include "l1audio_msgty.h"
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
  #include "cust_os.h"
  #include "l1_msgty.h"
  #include "l1_varex.h"
  #include "l1_proto.h"
  #include "l1_tabs.h"
  #include "l1_trace.h"

  #if L2_L3_SIMUL
    #include "l2_l3.h"
    #include "hw_debug.h"
  #endif

  #include "l1p_cons.h"
  #include "l1p_msgt.h"
  #include "l1p_deft.h"
  #include "l1p_vare.h"
  #include "l1p_sign.h"

  #if TESTMODE
    #include "l1tm_msgty.h"
    #include "l1tm_signa.h"
  #endif

  #include "macs_def.h"
  #include "macs_cst.h"
#endif

#if(RF_FAM == 61)
	#include "l1_rf61.h"
#endif

#include "l1_ctl.h"
#include "tpudrv61.h"

/*-------------------------------------------------------*/
/* Prototypes of external functions used in this file.   */
/*-------------------------------------------------------*/
void l1dmacro_synchro             (UWORD32 when, UWORD32 value);
void l1dmacro_offset              (UWORD32 offset_value, WORD32 relative_time);
void l1dmacro_rx_synth            (UWORD16 arfcn);
void l1dmacro_agc                 (UWORD16 arfcn,WORD8 gain, UWORD8 lna
                                                 	#if (RF_FAM == 61)
                                                 	   ,UWORD8 if_ctl
                                                 	#endif
													);
#if (L1_MADC_ON == 1)
#if (RF_FAM == 61)
void l1dmacro_rx_nb               (UWORD16 arfcn, UWORD8 adc_active, UWORD8 csf_filter_choice
#if (NEW_SNR_THRESHOLD == 1)
    ,UWORD8 saic_flag
#endif /* NEW_SNR_THRESHOLD */
    );
#endif /* RF_FAM == 61*/
#else  /* L1_MADC_ON == 1*/
void l1dmacro_rx_nb               (UWORD16 arfcn, UWORD8 csf_filter_choice);
#endif /* L1_MADC_ON == 1*/

void l1dmacro_afc                 (UWORD16 afc_value, UWORD8 win_id);
#if (RF_FAM == 61)
  void l1dtpu_serv_rx_nb            (UWORD16 radio_freq, WORD8 agc, UWORD8 lna_off,
                                     UWORD32 synchro_serv,UWORD32 new_offset,BOOL change_offset,
                                     UWORD8 adc_active, UWORD8 csf_filter_choice,
                                     UWORD8 if_ctl
#if (NEW_SNR_THRESHOLD == 1)
                                     ,UWORD8 saic_flag
#endif /* NEW_SNR_THRESHOLD */
                                     );

  void l1pdtpu_serv_rx_nb           (UWORD16 radio_freq, WORD8 agc, BOOL lna_off,
                                     UWORD8 rx_id, UWORD32 offset_serv, UWORD8 num_rx,
                                     UWORD8 rx_group_id, BOOL rx_done_flag,
                                     UWORD8 adc_active, UWORD8 csf_filter_choice,
                                     UWORD8 if_ctl
#if (NEW_SNR_THRESHOLD == 1)
                                    ,UWORD8 saic_flag
#endif /* NEW_SNR_THRESHOLD */
                                     );
#endif /* RF_FAM == 61*/
#if(RF_FAM != 61)
void l1dtpu_serv_rx_nb            (UWORD16 radio_freq, WORD8 agc, UWORD8 lna_off,
                                   UWORD32 synchro_serv,UWORD32 new_offset,BOOL change_offset, UWORD8 adc_active);

void l1pdtpu_serv_rx_nb           (UWORD16 radio_freq, WORD8 agc, BOOL lna_off,
                                   UWORD8 rx_id, UWORD32 offset_serv, UWORD8 num_rx,
                                   UWORD8 rx_group_id, BOOL rx_done_flag,UWORD8 adc_active);
#endif
void l1pdtpu_serv_tx              (UWORD16 radio_freq, UWORD8 timing_advance,
                                   UWORD32 offset_serv, UWORD8 tx_id, UWORD8 num_tx,
                                   UWORD8 tx_group_id, UWORD8 switch_flag, BOOL burst_type,
                                   BOOL rx_flag,UWORD8 adc_active);
UWORD8 l1pdtpu_interf_meas        (UWORD16 radio_freq,
                                   WORD8  agc,
                                   UWORD8  lna_off,
                                   UWORD8  meas_bitmap,
                                   UWORD32 offset_serv,
                                   UWORD16 win_id,
                                   UWORD8  synchro_ts
                                #if(RF_FAM == 61)
                                   ,UWORD8 if_ctl
                                #endif
                                );

void l1s_read_l3frm               (UWORD8 pwr_level, API *info_address, UWORD32 task_rx);
void l1ps_macs_read               (UWORD8 pr_table[8]);
void l1ps_macs_ctrl               (void);
#if TESTMODE
  void l1ps_tmode_macs_ctrl               (void);
#endif
void l1pddsp_transfer_mslot_power (UWORD8 *txpwr, UWORD16 radio_freq, UWORD8 ul_bitmap);

void l1pddsp_single_tx_block      (UWORD8 burst_nb, UWORD8 *data, UWORD8  tsc,
                                   UWORD16 radio_freq);
void l1pddsp_idle_prach_data      (BOOL    polling, UWORD8  cs_type, UWORD16  channel_request_data,
                                   UWORD8  bsic, UWORD16 radio_freq);
void l1pddsp_idle_prach_power     (UWORD8 txpwr, UWORD16  radio_freq, UWORD8 ts);
#if FF_L1_IT_DSP_USF
void l1pddsp_idle_rx_nb           (UWORD8 burst_nb, UWORD8 tsq, UWORD16 radio_freq,
                                   UWORD8 timeslot_no, BOOL ptcch_dl, BOOL usf_interrupt);
#else
void l1pddsp_idle_rx_nb           (UWORD8 burst_nb, UWORD8 tsq, UWORD16 radio_freq,
                                   UWORD8 timeslot_no, BOOL ptcch_dl);
#endif

void l1pddsp_ul_ptcch_data        (UWORD8 cs_type, UWORD16  channel_request_data, UWORD8  bsic, UWORD16 radio_freq, UWORD8 timeslot_no);
void l1ps_tcr_ctrl                (UWORD8 pm_position);
void l1pd_afc                     (void);
void l1pddsp_interf_meas_ctrl     (UWORD8  nb_meas_req);
void l1pddsp_meas_ctrl            (UWORD8 nbmeas, UWORD8 pm_pos);
void maca_power_control           (UWORD8 assignment_id, BOOL crc_error, WORD8 bcch_level, UWORD16 *radio_freq, WORD8 *burst_level, UWORD8 *pch);
WORD16 l1s_encode_rxlev           (UWORD8 inlevel);

void   l1pctl_pagc_ctrl           (WORD8 *agc, UWORD8 *lna_off, UWORD16 radio_freq,UWORD8 serving_cell);
UWORD8 l1pctl_pagc_read           (UWORD8 pm, UWORD16 radio_freq);
void   l1pctl_transfer_agc_ctrl   (WORD8 *agc, UWORD8 *lna_off, UWORD16 radio_freq);
void   l1pctl_npc_agc_read        (UWORD8 calibrated_IL[8],
                                   T_DB_DSP_TO_MCU_GPRS *pdsp_db_r_ptr,
                                   T_NDB_MCU_DSP_GPRS   *pdsp_ndb_ptr);
void   l1pctl_dpcma_agc_read      (UWORD8 calibrated_IL[8],
                                   T_DB_DSP_TO_MCU_GPRS *pdsp_db_r_ptr,
                                   T_NDB_MCU_DSP_GPRS *pdsp_ndb_ptr,
                                   UWORD8 pr_table[8]);
void   l1pctl_dpcmb_agc_read      (UWORD8 calibrated_IL[8],
                                   T_DB_DSP_TO_MCU_GPRS *pdsp_db_r_ptr,
                                   T_NDB_MCU_DSP_GPRS *pdsp_ndb_ptr,
                                   UWORD8 pr_table[8]);
void l1ps_macs_header_decoding    (UWORD8 rx_no, UWORD8 *tfi_result, UWORD8 *pr);
void l1ps_update_read_set_parameters(void);
void l1ps_bcch_meas_ctrl          (UWORD8 ts);

//#pragma DUPLICATE_FOR_INTERNAL_RAM_END


/*-------------------------------------------------------*/
/* l1ps_ctrl_single()                                    */
/*-------------------------------------------------------*/
/*                                                       */
/* Description:                                          */
/* ------------                                          */
/*                                                       */
/* Input parameters:                                     */
/* -----------------                                     */
/*                                                       */
/* Input parameters from globals:                        */
/* ------------------------------                        */
/*                                                       */
/* Modified parameters from globals:                     */
/* ---------------------------------                     */
/*                                                       */
/*-------------------------------------------------------*/
void l1ps_ctrl_single(UWORD8 task, UWORD8 burst_id)
{
  UWORD16        radio_freq;
  T_INPUT_LEVEL *IL_info_ptr;
  #if (RF_FAM == 61)
    UWORD16 dco_algo_ctl_nb = 0;
    UWORD8 if_ctl = 0;
    UWORD8 if_threshold = C_IF_ZERO_LOW_THRESHOLD_GPRS;
  #endif
  // By default we choose the hardware filter
  UWORD8 csf_filter_choice = L1_SAIC_HARDWARE_FILTER;
  #if (NEW_SNR_THRESHOLD == 1)
  UWORD8 saic_flag=0;
  #endif /* NEW_SNR_THRESHOLD */
  // needs to be defined for maca_power_control() function call
  #define DL_pwr_ctrl   l1pa_l1ps_com.transfer.dl_pwr_ctrl

  if((l1a_l1s_com.l1s_en_task[task] == TASK_ENABLED) &&
    !(l1a_l1s_com.task_param[task] == SEMAPHORE_SET))
  // Check the task semaphore. The control body is executed only
  // when the task semaphore is 0. This semaphore can be set to
  // 1 whenever L1A makes some changes to the task parameters.
  {
    // Catch ARFCN.
    // *************

    // Get ARFCN to be used for current control.
    radio_freq = l1a_l1s_com.dedic_set.radio_freq;

    // Traces and debug.
    // ******************

    #if (TRACE_TYPE==5) && FLOWCHART
      trace_flowchart_dsp_tpu(dltsk_trace[task].name);
      if(burst_id == BURST_1) trace_flowchart_dsptx(dltsk_trace[task].name);
    #endif

    #if (TRACE_TYPE!=0)
      if (l1pa_l1ps_com.transfer.single_block.activity & (SINGLE_DL | SINGLE_UL)) // trace only if a window is programmed
        trace_fct(CST_L1PS_CTRL_SINGLE, radio_freq);
    #endif

    l1s_dsp_com.dsp_db_w_ptr->d_debug = (l1s.debug_time + 2) ;

    /**************************************************************************/
    /* Program DSP for mulstislot operation...                                */
    /**************************************************************************/
    /*===============*/
    /* Downlink      */
    /*===============*/
    if(l1pa_l1ps_com.transfer.single_block.activity & SINGLE_DL)
    {
      // Programs DSP.
    #if FF_L1_IT_DSP_USF
      l1pddsp_idle_rx_nb(burst_id,
                         l1pa_l1ps_com.transfer.aset->tsc,
                         radio_freq,
                         0,
                         FALSE,
                         FALSE);
    #else
      l1pddsp_idle_rx_nb(burst_id,
                         l1pa_l1ps_com.transfer.aset->tsc,
                         radio_freq,
                         0,
                         FALSE);
    #endif

      // Flag DSP programmation.
      // Set "CTRL_RX" flag in the controle flag registers.
      l1s.dsp_ctrl_reg |= CTRL_RX;
    }

    /*===============*/
    /* Uplink        */
    /*===============*/
    if(l1pa_l1ps_com.transfer.single_block.activity & SINGLE_UL)
    {
      // Pgme DSP for Single block TX on TS=3.
      l1pddsp_single_tx_block (burst_id,
                               l1pa_l1ps_com.transfer.single_block.data_array,
                               l1pa_l1ps_com.transfer.aset->tsc,
                               radio_freq);

      // TXPWR control needs to take into account ALPHA, GAMMA and C values, not only TXPWR MAX
      // => maca_power_control() needs to be called
      // Initialization of txpwr control values for all time slots
      {
        UWORD8   txpwr[8];
        UWORD8   i;

        // Call Uplink Transmit Power level algorithm
        maca_power_control(l1pa_l1ps_com.transfer.aset->assignment_id,
                           DL_pwr_ctrl.crc_error,
                           DL_pwr_ctrl.bcch_level,
                           DL_pwr_ctrl.radio_freq_tbl,
                           DL_pwr_ctrl.burst_level,
                           txpwr);

        for(i = 0; i < 8; i++)
        {
          l1pa_l1ps_com.transfer.dl_pwr_ctrl.txpwr[i] = txpwr[i];
        }
      }

      // Pgme DSP for Transmit power on TS=3.
      l1pddsp_transfer_mslot_power(l1pa_l1ps_com.transfer.dl_pwr_ctrl.txpwr, radio_freq, 0x80 >> 3);

      // Flag DSP programmation.
      // Set "CTRL_TX" flag in the controle flag registers.
      l1s.dsp_ctrl_reg |= CTRL_TX;
    }

    /**************************************************************************/
    /* Program TPU for single slot operation...                               */
    /**************************************************************************/

    /*===============*/
    /* Downlink      */
    /*===============*/
    if(l1pa_l1ps_com.transfer.single_block.activity & SINGLE_DL)
    {
      WORD8  agc =0; //omaps00090550
      UWORD8  lna_off=0; //omaps00090550;

      // Update AGC
      l1pctl_transfer_agc_ctrl(&agc, &lna_off, radio_freq);


      #if (RF_FAM == 61)
          // Locosto DCO

         cust_get_if_dco_ctl_algo(&dco_algo_ctl_nb, &if_ctl, (UWORD8) L1_IL_VALID,
                                                  l1a_l1s_com.Scell_used_IL.input_level ,
                                                  radio_freq, if_threshold);
         l1ddsp_load_dco_ctl_algo_nb(dco_algo_ctl_nb);

      #if (L1_SAIC != 0)
        // If SAIC is enabled, call the low level SAIC control function
        // NOTE: l1a_l1s_com.Scell_used_IL.input_level is updated within
        //       the l1pctl_transfer_agc_ctrl above
        csf_filter_choice = l1ctl_saic(l1a_l1s_com.Scell_used_IL.input_level,l1a_l1s_com.mode
        #if (NEW_SNR_THRESHOLD == 1)
            ,task
            ,&saic_flag
        #endif
        );
      #endif

         // tpu pgm...
        l1pdtpu_serv_rx_nb(radio_freq,
                           agc,
                           lna_off,
                           0,
                           l1s.tpu_offset,
                           1,
                           1,
                           TRUE,
                           INACTIVE,
                           csf_filter_choice,
                           if_ctl
      #if (NEW_SNR_THRESHOLD == 1)
                            ,saic_flag
      #endif /* NEW_SNR_THRESHOLD */
                           );
      #endif /* RF_FAM == 61*/
      #if (RF_FAM != 61)
        // tpu pgm...
         #if (L1_SAIC != 0)
           // If SAIC is enabled, call the low level SAIC control function
           // NOTE: l1a_l1s_com.Scell_used_IL.input_level is updated within
           //       the l1pctl_transfer_agc_ctrl above
           l1ctl_saic(l1a_l1s_com.Scell_used_IL.input_level,l1a_l1s_com.mode
          #if (NEW_SNR_THRESHOLD == 1)
               ,task
          #endif
          );
         #endif


        l1pdtpu_serv_rx_nb(radio_freq,
                           agc,
                           lna_off,
                           0,
                           l1s.tpu_offset,
                           1,
                           1,
                           TRUE,
                           INACTIVE);
      #endif

      // Set tpu window identifier for Synthesizer and
      // according to last RX group position.
      l1s.tpu_win = l1_config.params.rx_synth_load_split + RX_SPLIT_TABLE[0];

      // Flag TPU programmation.
      // Set "CTRL_RX" flag in the controle flag registers.
      l1s.tpu_ctrl_reg |= CTRL_RX;
    }

    /*===============*/
    /* Uplink        */
    /*===============*/
    if(l1pa_l1ps_com.transfer.single_block.activity & SINGLE_UL)
    {
      // Program single block UL slot.
      // ******************************
      l1pdtpu_serv_tx(radio_freq,
                      l1pa_l1ps_com.transfer.aset->packet_ta.ta,
                      l1s.tpu_offset,
                      3,
                      1,
                      1,
                      0,
                      TX_NB_BURST,
                      l1pa_l1ps_com.transfer.single_block.activity & SINGLE_DL,
                      INACTIVE);

      // Set tpu window identifier for Synthesizer and
      // according to last TX group position.
      l1s.tpu_win =(UWORD16)( (l1_config.params.rx_synth_load_split) +
                    ((UWORD16)3 * BP_SPLIT) + l1_config.params.tx_nb_load_split);

      // Flag TPU programmation.
      // Set "CTRL_TX" flag in the controle flag registers.
      l1s.tpu_ctrl_reg |= CTRL_TX;

      #if (TRACE_TYPE == 1) || (TRACE_TYPE == 4)
        RTTL1_FILL_UL_NB(task, l1pa_l1ps_com.transfer.aset->packet_ta.ta, l1s.applied_txpwr)
      #endif
    }

    /*===============*/
    /* General       */
    /*===============*/
    if(burst_id == BURST_4)
    {
      // Single block UL is now complete, reset its activity flag.
      l1pa_l1ps_com.transfer.single_block.activity &= SINGLE_UL_MASK;

      if(l1pa_l1ps_com.transfer.aset->allocated_tbf == SINGLE_BLOCK_DL)
      {
        // Single block UL is now complete.
        l1pa_l1ps_com.transfer.single_block.activity &= SINGLE_DL_MASK;
      }
    }
  } // End if(task enabled and semaphore false)

  else
  // When the task is aborted, we must continue to make dummy
  // DSP programming to avoid communication mismatch due
  // to C/W/R pipelining.
  {
    // Flag dummy DSP programmation.
    // Set "CTRL_TX" flag in the controle flag registers.
    l1s.dsp_ctrl_reg |= CTRL_TX;
  }
}

/*-------------------------------------------------------*/
/* l1ps_ctrl_prach()                                     */
/*-------------------------------------------------------*/
/*                                                       */
/* Description:                                          */
/* ------------                                          */
/*                                                       */
/* Input parameters:                                     */
/* -----------------                                     */
/*                                                       */
/* Input parameters from globals:                        */
/* ------------------------------                        */
/*                                                       */
/* Modified parameters from globals:                     */
/* ---------------------------------                     */
/*                                                       */
/*-------------------------------------------------------*/
void l1ps_ctrl_prach(UWORD8 task, UWORD8 burst_id)
{
  UWORD16  radio_freq;
  UWORD8   txpwr;
  UWORD8   adc_active = INACTIVE;

  // Get ARFCN to be used for current control.
  // ******************************************

  radio_freq = l1pa_l1ps_com.p_idle_param.radio_freq;

  // Get TXPWR value
  txpwr = l1s.applied_txpwr;

  // Traces and debug.
  // ******************

  #if (TRACE_TYPE!=0)
    trace_fct(CST_L1S_CTRL_RACH, radio_freq);
  #endif

  #if (TRACE_TYPE==5) && FLOWCHART
    trace_flowchart_dsp_tpu(dltsk_trace[task].name);
  #endif

  #if FF_L1_IT_DSP_USF
    // Whenever the USF status is unknown then the PRACH control execution
    // has to be postponed until DSP USF interrupt fires.
    if (l1ps_macs_com.usf_status != USF_AWAITED)
    {
  #endif // FF_L1_IT_DSP_USF

  l1s_dsp_com.dsp_db_w_ptr->d_debug = (l1s.debug_time + 2) ;

      #if FF_L1_IT_DSP_USF
        // TPU and DSP have to be programmed for transmission only if the USF is
        // good otherwise this is not a valid opportunity. If there was no USF
        // uncertainty then the test is void.
        if ( (l1ps_macs_com.usf_status != USF_IT_DSP)
             || (((l1ps_dsp_com.pdsp_ndb_ptr->d_usf_updated_gprs >> ((7-0)*2)) & 0x0003) == USF_GOOD))
        {
          // Flags PRACH burst was controlled to TPU/DSP (not cancelled due to USF)
          l1pa_l1ps_com.pra_info.prach_controlled = TRUE;
      #endif // FF_L1_IT_DSP_USF

  #if (CODE_VERSION!=SIMULATION)
    #if (TRACE_TYPE==2 ) || (TRACE_TYPE==3)
      L1_trace_string("PRA");
    #endif
  #endif

  // Programs DSP for required task.
  // ********************************
  {
    UWORD8  cs_type;

    if (l1pa_l1ps_com.access_burst_type == ACC_BURST_8)
      cs_type = CS_PAB8_TYPE;
    else
      cs_type = CS_PAB11_TYPE;

    // ACCESS PRACH dsp control.
    l1pddsp_idle_prach_data(FALSE,
                            cs_type,
                            l1pa_l1ps_com.pra_info.channel_request_data,
                            l1a_l1s_com.Scell_info.bsic,
                            radio_freq);
  }

  l1pddsp_idle_prach_power(txpwr,
                           radio_freq,
                           3);

  // ADC measurement
  // ***************

  // check if during the RACH an ADC measurement must be performed
  if (l1a_l1s_com.adc_mode & ADC_EACH_RACH)  // perform ADC on each burst
     adc_active = ACTIVE;

  // Programs TPU for required task.
  // ********************************

  // tpu pgm...
  l1dtpu_serv_tx_ra(radio_freq, l1s.tpu_offset, txpwr, adc_active);

  // Set tpu window identifier for Power meas if any.
  l1s.tpu_win = (3 * BP_SPLIT) + l1_config.params.tx_ra_load_split + l1_config.params.rx_synth_load_split;

  // Store frame number to report to L3
  l1pa_l1ps_com.pra_info.fn_to_report = l1s.next_time.fn;

  #if (TRACE_TYPE == 1) || (TRACE_TYPE == 4)
    RTTL1_FILL_UL_AB(task,txpwr)
  #endif

  // Flag DSP and TPU programmation.
  // ********************************

  // Set "CTRL_TX" flag in the controle flag register.
  l1s.tpu_ctrl_reg |= CTRL_PRACH;
  l1s.dsp_ctrl_reg |= CTRL_TX;

      #if FF_L1_IT_DSP_USF
        }// if ((l1ps_macs_com.usf_status != USF_IT_DSP) || USF_GOOD)
        else
        {
          // PRACH has been cancelled because USF not FREE. Hence Read is skipped.
          l1pa_l1ps_com.pra_info.prach_controlled = FALSE;
        }
      #endif

  #if FF_L1_IT_DSP_USF
    } // if (l1ps_macs_com.usf_status != USF_AWAITED)
  #endif
}

/*-------------------------------------------------------*/
/* l1ps_ctrl_poll()                                      */
/*-------------------------------------------------------*/
/*                                                       */
/* Description:                                          */
/* ------------                                          */
/*                                                       */
/* Input parameters:                                     */
/* -----------------                                     */
/*                                                       */
/* Input parameters from globals:                        */
/* ------------------------------                        */
/*                                                       */
/* Modified parameters from globals:                     */
/* ---------------------------------                     */
/*                                                       */
/*-------------------------------------------------------*/
void l1ps_ctrl_poll(UWORD8 task, UWORD8 burst_id)
{
  UWORD16  radio_freq;

  // Get ARFCN to be used for current control.
  // ******************************************
  if(l1a_l1s_com.l1s_en_task[SINGLE] == TASK_ENABLED)
    radio_freq = l1a_l1s_com.dedic_set.radio_freq;
  else
    radio_freq = l1pa_l1ps_com.p_idle_param.radio_freq;

  // Traces and debug.
  // ******************

  #if (TRACE_TYPE!=0)
    trace_fct(CST_L1PS_CTRL_POLL, radio_freq);
  #endif

  #if (TRACE_TYPE==5) && FLOWCHART
    trace_flowchart_dsp_tpu(dltsk_trace[task].name);
  #endif

  l1s_dsp_com.dsp_db_w_ptr->d_debug = (l1s.debug_time + 2) ;

  #if (CODE_VERSION!=SIMULATION)
    #if (TRACE_TYPE==2 ) || (TRACE_TYPE==3)
      L1_trace_string("PRA");
    #endif
  #endif

  // Programs DSP for required task.
  // ********************************
  {
    UWORD8  cs_type;

    cs_type = l1pa_l1ps_com.poll_info.pol_resp_type;

    if ((cs_type == CS_PAB8_TYPE) || (cs_type == CS_PAB11_TYPE))
    {
      // IDLE POLLING PRACH dsp control.
      l1pddsp_idle_prach_data(TRUE,
                              cs_type,
                              l1pa_l1ps_com.poll_info.chan_req.prach_data[0],
                              l1a_l1s_com.Scell_info.bsic,
                              radio_freq);

      l1pddsp_idle_prach_power(l1s.applied_txpwr,
                               radio_freq,
                               3);

      // Programs TPU for required task.
      // ********************************
      // tpu pgm...
      l1dtpu_serv_tx_ra(radio_freq, l1s.tpu_offset, l1s.applied_txpwr,INACTIVE);

      #if (TRACE_TYPE == 1) || (TRACE_TYPE == 4)
        RTTL1_FILL_UL_AB(task,l1s.applied_txpwr)
      #endif
    }
    else
    {
      UWORD8   tsc;

      if(l1a_l1s_com.l1s_en_task[SINGLE] == TASK_ENABLED)
        tsc = l1pa_l1ps_com.transfer.aset->tsc;
      else
        tsc = l1pa_l1ps_com.pccch.packet_chn_desc.tsc;

      // Pgm DSP for Poll Response TX NB on TS=3.
      l1pddsp_single_tx_block (burst_id,
                              l1pa_l1ps_com.poll_info.chan_req.cs1_data,
                              tsc,
                              radio_freq);

      // Pgm DSP for Transmit power.
      l1pddsp_transfer_mslot_power(l1pa_l1ps_com.transfer.dl_pwr_ctrl.txpwr, radio_freq, 0x80>>3);

      // Programs TPU for required task.
      // ********************************
      // tpu pgm...
      l1dtpu_serv_tx_nb(radio_freq,
                         l1pa_l1ps_com.poll_info.timing_advance,
                         l1s.tpu_offset,
                         l1pa_l1ps_com.transfer.dl_pwr_ctrl.txpwr[3],INACTIVE);

      #if (TRACE_TYPE == 1) || (TRACE_TYPE == 4)
        RTTL1_FILL_UL_NB(task, l1pa_l1ps_com.poll_info.timing_advance, l1pa_l1ps_com.transfer.dl_pwr_ctrl.txpwr[3])
      #endif
    }
  }


  // Set tpu window identifier for Power meas if any.
  l1s.tpu_win = (3 * BP_SPLIT) + l1_config.params.tx_nb_load_split + l1_config.params.rx_synth_load_split;

  // Store frame number to report to L3
  if (burst_id == BURST_4)
    l1pa_l1ps_com.poll_info.fn_to_report = l1s.next_time.fn;

  // Flag DSP and TPU programmation.
  // ********************************

  // Set "CTRL_TX" flag in the controle flag register.
  l1s.tpu_ctrl_reg |= CTRL_TX;
  l1s.dsp_ctrl_reg |= CTRL_TX;
}


#if (MOVE_IN_INTERNAL_RAM == 0) // Must be followed by the pragma used to duplicate the funtion in internal RAM
//#pragma DUPLICATE_FOR_INTERNAL_RAM_START

/*-------------------------------------------------------*/
/* l1s_ctrl_pdtch()                                      */
/*-------------------------------------------------------*/
/*                                                       */
/* Description:  This is the control function for Multislot Rx/Tx                */
/* ------------                                          */
/*                                                       */
/* Input parameters:                                     */
/* -----------------                                     */
/*                                                       */
/* Input parameters from globals:                        */
/* ------------------------------                        */
/*                                                       */
/* Modified parameters from globals:                     */

/*-------------------------------------------------------*/
void l1ps_ctrl_pdtch(UWORD8 task, UWORD8 burst_id)
{
  if((l1a_l1s_com.l1s_en_task[task] == TASK_ENABLED) &&
    !(l1a_l1s_com.task_param[task] == SEMAPHORE_SET))
  // Check the task semaphore. The control body is executed only
  // when the task semaphore is 0. This semaphore can be set to
  // 1 whenever L1A makes some changes to the task parameters.
  {
    UWORD16        radio_freq;
    // By default we choose the hardware filter
    UWORD8 csf_filter_choice = L1_SAIC_HARDWARE_FILTER;
    #if (NEW_SNR_THRESHOLD == 1)
    UWORD8 saic_flag=0;
    #endif  /* NEW_SNR_THRESHOLD */
    // Traces and debug.
    // ******************

    #if (TRACE_TYPE==5) && FLOWCHART
      trace_flowchart_dsp_tpu(dltsk_trace[PDTCH].name);
    #endif

    l1s_dsp_com.dsp_db_w_ptr->d_debug = (l1s.debug_time + 2) ;

    // Catch channel description and ARFCN.
    // *************************************

    // Get ARFCN to be used for current control. This  ARFCN comes from
    // the HOPPING algorithm called just before calling this function.
    radio_freq = l1a_l1s_com.dedic_set.radio_freq;

    #if (TRACE_TYPE==5) // in simulation trace only one time by frame
      trace_fct(CST_L1PS_CTRL_PDTCH, radio_freq);
    #endif

    /**************************************************************************/
    /* Program DSP for mulstislot operation...                                */
    /**************************************************************************/
    {
      UWORD8  tx_allocation;

      #if TESTMODE
      if (l1_config.TestMode && (l1_config.tmode.tx_params.burst_data == 11))
        // Call dummy MACS for CMU200 loopback mode
        l1ps_tmode_macs_ctrl();
      else
      #endif
      {
        // Call MACS for Medium control, DATA control and RLC-MACS management
        l1ps_macs_ctrl();
      }

      #if FF_L1_IT_DSP_USF
        if (l1ps_macs_com.usf_status != USF_AWAITED)
        {
      #endif
      // Compute tx_allocation mixing NB and RA allocations.
      tx_allocation = l1ps_macs_com.tx_nb_allocation | l1ps_macs_com.tx_prach_allocation;

      // Pgme TXPWR only if any UL.
      if(tx_allocation)
      {
        // Pgme DSP for Transmit power.
        // !!! Warning: This function must be called before l1pdtpu_serv_tx()
        // !!! a_ctrl_abb_gprs is partly overwritten by l1pdtpu_serv_tx()
        l1pddsp_transfer_mslot_power(l1pa_l1ps_com.transfer.dl_pwr_ctrl.txpwr, radio_freq, tx_allocation);
      }
      #if FF_L1_IT_DSP_USF
        }
      #endif
    }


    /**************************************************************************/
    /* Program TPU for mulstislot operation...                                */
    /**************************************************************************/
    {
      WORD8  ts             = 0;
      UWORD8 rx_group_id    = 0;
      UWORD8 tx_group_id    = 0;
      BOOL   pwr_programmed = FALSE;
      UWORD8 bit_mask       = 0x80;
      WORD8  agc =0;  //omaps00090550
      UWORD8 lna_off=0;  //omaps00090550;
      BOOL   rx_done_flag;
      BOOL   adc_done = FALSE;
      UWORD8 adc_active = INACTIVE;

     #if (RF_FAM == 61)
       UWORD16 dco_algo_ctl_nb = 0;
       UWORD16 dco_algo_ctl_pw = 0;
       UWORD8 if_ctl =0;  //omaps00090550;
       UWORD8 tot_num_rx = 0;
	   UWORD8 if_threshold = C_IF_ZERO_LOW_THRESHOLD_GPRS;
     #endif

      // AGC and LNA_OFF processing
      //---------------------------

      #if FF_L1_IT_DSP_USF
        #if L1_EDA
          if (l1ps_macs_com.usf_status != USF_AWAITED)
        #else
         if (l1ps_macs_com.usf_status != USF_IT_DSP)
      #endif
      #endif
      // Same AGC is used for all timeslots
      l1pctl_transfer_agc_ctrl(&agc, &lna_off, radio_freq);

      #if (L1_SAIC != 0)
        // If SAIC is enabled, call the low level SAIC control function
        // NOTE: l1a_l1s_com.Scell_used_IL.input_level is updated within
        //       the l1pctl_transfer_agc_ctrl above
        csf_filter_choice = l1ctl_saic(l1a_l1s_com.Scell_used_IL.input_level,l1a_l1s_com.mode
        #if (NEW_SNR_THRESHOLD == 1)
            ,task
            ,&saic_flag
        #endif
            );
      #endif

      while(ts < 8)
      {

        #if FF_L1_IT_DSP_USF
          #if L1_EDA
            //Depending on USF status got from DSP, RX allocation may change after
            //receiving the USF interrupt due to USF status updates
            if ((l1ps_macs_com.rx_allocation & bit_mask)
                && (l1ps_macs_com.usf_status != USF_AWAITED))
          #else
               if ((l1ps_macs_com.rx_allocation & bit_mask)
                && (l1ps_macs_com.usf_status != USF_IT_DSP))
          #endif
        #else
        if(l1ps_macs_com.rx_allocation & bit_mask)
        #endif
        {
          // We have detected the 1st RX slot number for a new RX group.

          UWORD8  rx_id  = ts;   // Save 1st RX timeslot number.
          UWORD8  num_rx = 1;    // 1 RX in the RX group for the moment.

          // Increment the RX group ID.
          rx_group_id++;

          #if (RF_FAM == 61)
          //Increment the total number of RX slots
          tot_num_rx++;
          #endif

          // Increment TS.
          ts++;

          // Jump on next timeslot to keep looking for contiguous RX slots.
          bit_mask >>= 1;

          // Look for more contiguous RX slots.
          while((l1ps_macs_com.rx_allocation & bit_mask) && (ts < 8))
          {
            ts++;
            num_rx++;
            #if (RF_FAM == 61)
            tot_num_rx++;
            #endif
            bit_mask >>= 1;
          }

          // Check if more RX bursts follow
          if((l1ps_macs_com.rx_allocation << (ts+1)) & 0xFF)
            rx_done_flag = FALSE;
          else
            rx_done_flag = TRUE;

          #if (RF_FAM == 61) // Locosto DCO
           if(rx_group_id == 1)
           {
             cust_get_if_dco_ctl_algo(&dco_algo_ctl_nb, &if_ctl, (UWORD8) L1_IL_VALID,
                  l1a_l1s_com.Scell_used_IL.input_level, radio_freq,if_threshold);
           }

           if(rx_done_flag == TRUE)
           {
             //dco_algo_ctl has 0000 00ZL
             dco_algo_ctl_nb *= 0x55;   // replicate 0000 00zL as ZLZL ZLZL
             // ZLZLZLZL >> 2*(4-tot_num_rx) where i is the tot_num_rx would produce the
             // desired dco_algo_ctl_nb For Eg if tot_num_rx is 2 the desired pattern is
             // 0000 ZLZL
             dco_algo_ctl_nb = dco_algo_ctl_nb >> (2*( 4 - tot_num_rx));
             l1ddsp_load_dco_ctl_algo_nb(dco_algo_ctl_nb);
           }
          #endif
          // ADC measurement
          // ***************
          if (adc_done == FALSE)
          {
            // check if during the PDTCH burst an ADC measurement must be performed
             if (l1a_l1s_com.adc_mode & ADC_NEXT_TRAFFIC_DL)  // perform ADC only one time
             {
                adc_active = ACTIVE;
                adc_done   = TRUE;
                l1a_l1s_com.adc_mode &= ADC_MASK_RESET_TRAFFIC; // reset in order to have only one ADC measurement in Traffic
             }
             else
               if (l1a_l1s_com.adc_mode & ADC_EACH_TRAFFIC_DL) // perform ADC on each period bloc
                 if (l1s.actual_time.fn_mod104 == 10) //periodic with each PDTCH burst in frame 11 (frame with the lowest CPU load)
                   if ((++l1a_l1s_com.adc_cpt)>=l1a_l1s_com.adc_traffic_period) // wait for the period
                   {
                     adc_active = ACTIVE;
                     adc_done   = TRUE;
                     l1a_l1s_com.adc_cpt = 0;
                   }
           }

          // update the TPU with the new TOA if necessary
          if (rx_group_id == 1) // only if synchro performed
            l1ctl_update_TPU_with_toa();

          #if (TRACE_TYPE!=0) && (TRACE_TYPE!=5)
            trace_fct(CST_L1PS_CTRL_PDTCH_DL_BURST0 + burst_id, radio_freq);
          #endif

          // Program RX scenario.
          l1pdtpu_serv_rx_nb(radio_freq,
                             agc,
                             lna_off,
                             rx_id,
                             l1s.tpu_offset,
                             num_rx,
                             rx_group_id,
                             rx_done_flag,adc_active,
                             csf_filter_choice
                          #if (RF_FAM == 61)
                             ,if_ctl
                          #endif
                          #if (NEW_SNR_THRESHOLD == 1)
                             ,saic_flag
                          #endif
		                              );

          adc_active = INACTIVE;  // ADC performed only on the first RX burst

          // Set tpu window identifier for Synthesizer and
          // according to last RX group position.
          l1s.tpu_win = (l1_config.params.rx_synth_load_split) +
                        ((UWORD16)rx_id * BP_SPLIT) + RX_SPLIT_TABLE[num_rx-1];


          // Set "CTRL_RX" flag in the controle flag registers.
          l1s.tpu_ctrl_reg |= CTRL_RX;
          l1s.dsp_ctrl_reg |= CTRL_RX;
        }

        else
        #if FF_L1_IT_DSP_USF
          if ((l1ps_macs_com.tx_nb_allocation & bit_mask)
                  && (l1ps_macs_com.usf_status != USF_AWAITED))
        #else
        if(l1ps_macs_com.tx_nb_allocation & bit_mask)
        #endif
        {
          // We have detected the 1st TX NB slot number for a new TX group.

          UWORD8  tx_id       = ts; // Save 1st TX timeslot number.
          UWORD8  num_tx      = 1;  // 1 RX in the TX group for the moment.
          UWORD8  switch_flag;

          // Increment the TX group ID.
          tx_group_id++;

          // Increment TS.
          ts++;

          // Jump on next timeslot to keep looking for contiguous TX slots.
          bit_mask >>= 1;

          // Look for more contiguous TX slots.
          while((l1ps_macs_com.tx_nb_allocation & bit_mask) && (ts < 8))
          {
            ts++;
            num_tx++;
            bit_mask >>= 1;
          }

          // Detect special case: TX NB followed by PRACH.
          if(l1ps_macs_com.tx_prach_allocation & bit_mask)
          {
            switch_flag = 1;
          }
          else
          {
            switch_flag = 0;
          }

          #if (TRACE_TYPE!=0) && (TRACE_TYPE!=5)
            trace_fct(CST_L1PS_CTRL_PDTCH_UL, radio_freq);
          #endif

          // Program TN NB scenario.
          l1pdtpu_serv_tx(radio_freq,
                          l1pa_l1ps_com.transfer.aset->packet_ta.ta,
                          l1s.tpu_offset,
                          tx_id,
                          num_tx,
                          tx_group_id,
                          switch_flag,
                          0,             // Driver called for Normal Burst.
                          TRUE,INACTIVE);// Flag RX in same frame as TX

          // Set tpu window identifier for Synthesizer and
          // according to last TX group position.
          // TX offset = tx_id * BP_SPLIT
          // TX load = (num_tx-1) * BP_SPLIT + l1_config.params.tx_load_split
          l1s.tpu_win = (l1_config.params.rx_synth_load_split) +
                        ((UWORD16)tx_id * BP_SPLIT) +
                        ((UWORD16)(num_tx -1) * BP_SPLIT) +
                        l1_config.params.tx_nb_load_split;

          // Set "CTRL_TX" flag in the controle flag registers.
          l1s.tpu_ctrl_reg |= CTRL_TX;
          l1s.dsp_ctrl_reg |= CTRL_TX;
        }

        else
            #if FF_L1_IT_DSP_USF
              if ((l1ps_macs_com.tx_prach_allocation & bit_mask)
                  && (l1ps_macs_com.usf_status != USF_AWAITED))
            #else
             if(l1ps_macs_com.tx_prach_allocation & bit_mask)
            #endif
        {
          // We have detected a TX RA.

          UWORD8 switch_flag;

          // Increment the TX group ID.
          tx_group_id++;

          // Jump on next timeslot.
          bit_mask >>= 1;

          // Detect special case: PRACH followed by TX NB.
          if(l1ps_macs_com.tx_nb_allocation & bit_mask)
          {
            switch_flag = 1;
          }
          // Detect special case: PRACH followed by PRACH
          else if (l1ps_macs_com.tx_prach_allocation & bit_mask)
          {
            // Solution with DSP patch supporting PRACH|PRACH
            switch_flag = 2;
          }
          else
          {
            switch_flag = 0;
          }

          #if (TRACE_TYPE!=0) && (TRACE_TYPE!=5)
            trace_fct(CST_L1PS_CTRL_PDTCH_RA, radio_freq);
          #endif

          // Program TX RA scenario.
          l1pdtpu_serv_tx(radio_freq,
                          l1pa_l1ps_com.transfer.aset->packet_ta.ta,
                          l1s.tpu_offset,
                          ts,
                          1,               // Driver is called for each PRACH.
                          tx_group_id,
                          switch_flag,
                          TX_RA_BURST,     // Driver called for PRACH Burst.
                          TRUE,INACTIVE);  // Flag RX in same frame as TX

          // Set tpu window identifier for Synthesizer and
          // according to last TX group position.
          // TX offset = ts * BP_SPLIT
          // TX load = l1_config.params.tx_nb_load_split
          // original value of TX load (RACH) replaced by TX NB to take into account TX_NB|PRACH with max. TA
          l1s.tpu_win = (l1_config.params.rx_synth_load_split) +
                        (ts * BP_SPLIT) +
                        l1_config.params.tx_nb_load_split;


          // Increment TS.
          ts++;

          // Set "CTRL_TX" flag in the controle flag registers.
          l1s.tpu_ctrl_reg |= CTRL_TX;
          l1s.dsp_ctrl_reg |= CTRL_TX;
        }

        else
        #if FF_L1_IT_DSP_USF
          if ((l1ps_macs_com.pwr_allocation & bit_mask)
              && (pwr_programmed == 0)
              && (l1ps_macs_com.usf_status != USF_AWAITED))
        #else
        if((l1ps_macs_com.pwr_allocation & bit_mask) && (pwr_programmed == 0))
        #endif
        {
          // We have detected a PWR allocation and no PWR programmed in current frame.
          if((l1pa_l1ps_com.transfer.aset->pc_meas_chan == FALSE) &&
              ((l1s.actual_time.t2 == 1) || (l1s.actual_time.t2 == 9)
               || (l1s.actual_time.t2 == 18)))
          {
            #if (TRACE_TYPE!=0) && (TRACE_TYPE!=5)
              trace_fct(CST_CTRL_PC_MEAS_CHAN, (UWORD32)(-1));
            #endif

            // Measurement on the beacon (PC_MEAS_CHAN = 0)
            l1ps_bcch_meas_ctrl(ts);
          }
          else
          if(l1pa_l1ps_com.l1ps_en_meas & P_TCRMS_MEAS)
          {
            // Neighbour Measurement CTRL Phase

            // Note: Test on l1s.forbid_meas can't be done from the fact that at this
            // level, l1s.forbid_meas is not set.
            // l1ps_ctrl_pdtch is called before CTRL of FB26/SB26/SBCNF26.
            if(!((l1s.actual_time.t2 == 23) &&
                ((l1s.task_status[FB26].current_status    != INACTIVE) ||
                 (l1s.task_status[SB26].current_status    != INACTIVE) ||
                 (l1s.task_status[SBCNF26].current_status != INACTIVE)))&&
               !(l1pa_l1ps_com.tcr_freq_list.new_list_present &&
                 ((l1pa_l1ps_com.tcr_freq_list.cres_meas_report == 0) ||
                  (l1pa_l1ps_com.tcr_freq_list.cres_meas_report == 103))))
            {
              if(!(l1pa_l1ps_com.meas_param & P_TCRMS_MEAS))
              {
                #if (TRACE_TYPE!=0) && (TRACE_TYPE!=5)
                  trace_fct(CST_CTRL_TCR_MEAS_1, (UWORD32)(-1));
                #endif

                l1ps_tcr_ctrl((UWORD8)ts);
              }
            }
          } // End of CTRL PDTCH phase

          // Increment TS.
          ts++;

          // Flag that a PWR as been programmed for current frame.
          pwr_programmed = TRUE;

          // Jump on next timeslot.
          bit_mask >>= 1;

          // Call PWR control function

        }

        else
        {
          // Increment TS.
          ts++;

          // Jump on next timeslot.
          bit_mask >>= 1;
        }

      } // End of "while(ts < 8)"

      #if FF_L1_IT_DSP_USF
        if (l1ps_macs_com.usf_status != USF_AWAITED)
      #endif
      // Update of AFC
      l1pd_afc();
    }
  } // End if(task enabled and semaphore false)

  else
  // When the task is aborted, we must continue to make dummy
  // DSP programming to avoid communication mismatch due
  // to C/W/R pipelining.
  {
    #if (TRACE_TYPE!=0) && (TRACE_TYPE!=5)
      trace_fct(CST_L1PS_CTRL_PDTCH_DUMMY, (UWORD32)(-1));
    #endif

    // Flag dummy DSP programmation.
    // Set "CTRL_TX" flag in the controle flag registers.
    l1s.dsp_ctrl_reg |= CTRL_TX;
  }

}
//#pragma DUPLICATE_FOR_INTERNAL_RAM_END
#endif // MOVE_IN_INTERNAL_RAM


/*-------------------------------------------------------*/
/* l1s_read_single()                                     */
/*-------------------------------------------------------*/
/*                                                       */
/* Description:                                          */
/* ------------                                          */
/*                                                       */
/* Input parameters:                                     */
/* -----------------                                     */
/*                                                       */
/* Input parameters from globals:                        */
/* ------------------------------                        */
/*                                                       */
/* Modified parameters from globals:                     */
/* ---------------------------------                     */
/*                                                       */
/*-------------------------------------------------------*/
void l1ps_read_single(UWORD8 task, UWORD8 burst_id)
{
  if((l1a_l1s_com.l1s_en_task[task] == TASK_ENABLED) &&
    !(l1a_l1s_com.task_param[task] == SEMAPHORE_SET))
  // Check the task semaphore. The control body is executed only
  // when the task semaphore is 0. This semaphore can be set to
  // 1 whenever L1A makes some changes to the task parameters.
  {
    // Read param updating...
    if (l1ps.read_param.new_set == TRUE)
    {
      // If it's the first Read phase of the block (first of the new TBF)
      if (burst_id == BURST_1)
      {
        // Update the "read_param" structure
        l1ps_update_read_set_parameters();
      }
    }

    /*--------------------------------------------------------*/
    /* READ TRANSMIT TASK RESULTS...                          */
    /*--------------------------------------------------------*/

    l1_check_com_mismatch(task);

     // check PM error only in case of downlink single block
	if(l1ps.read_param.allocated_tbf == SINGLE_BLOCK_DL)
    {
       UWORD32   pm;
       pm = (l1ps_dsp_com.pdsp_db_r_ptr->a_burst_pm_gprs[0] & 0xffff);
       l1_check_pm_error(pm,task);
       pm = pm >> 5;
    }


    // Two phase access: downlink PDCH reading
    if((l1ps.read_param.allocated_tbf == TWO_PHASE_ACCESS)&&
       (l1ps_dsp_com.pdsp_db_r_ptr->d_task_d_gprs & (0x80 >> 0)))
    {
      UWORD8 IL_for_rxlev[8];
      UWORD8 pr_table[8];

      if (burst_id == BURST_4)
        l1ps_macs_header_decoding(0, &(IL_for_rxlev[0]), &(pr_table[0]));

      // Update AGC and extract IL for RXLEV
      //------------------------------------
      if (l1ps.read_param.dl_pwr_ctl.p0 == 255)
      {
        // No power control mode AGC algorithm
        l1pctl_npc_agc_read(IL_for_rxlev, l1ps_dsp_com.pdsp_db_r_ptr, l1ps_dsp_com.pdsp_ndb_ptr);
      }
      else
      {
        // Downlink power control AGC algorithms
        if (l1pa_l1ps_com.transfer.aset->dl_pwr_ctl.bts_pwr_ctl_mode == 0)
        {
          // BTS Power control mode A
          l1pctl_dpcma_agc_read(IL_for_rxlev, l1ps_dsp_com.pdsp_db_r_ptr, l1ps_dsp_com.pdsp_ndb_ptr, pr_table);
        }
        else
        {
          // BTS power control mode B
          l1pctl_dpcmb_agc_read(IL_for_rxlev, l1ps_dsp_com.pdsp_db_r_ptr, l1ps_dsp_com.pdsp_ndb_ptr, pr_table);
        }
      } // End of "AGC algorithm"
    }

    #if (TRACE_TYPE!=0) && (TRACE_TYPE !=5)
      trace_fct(CST_L1PS_READ_SINGLE, l1a_l1s_com.Scell_info.radio_freq);
    #endif

    // Read downlink DATA block from MCU/DSP interface.
    // *************************************************
    if(burst_id == BURST_4)
    {
      xSignalHeaderRec *msg;

      #if (TRACE_TYPE==5) // in simulation trace only the 4th burst
        trace_fct(CST_L1PS_READ_SINGLE, l1a_l1s_com.Scell_info.radio_freq);
      #endif

      if(l1ps.read_param.allocated_tbf == TWO_PHASE_ACCESS)
      // 2 phase ACCESS.
      {
        if((l1ps_dsp_com.pdsp_db_r_ptr->d_task_u_gprs & (0x80 >> 3)) &&
           (l1s.task_status[POLL].current_status == INACTIVE))
        // UL block sent...
        {
          // Send confirmation msg to L3/MACA.
          msg = os_alloc_sig(sizeof(T_MPHP_SINGLE_BLOCK_CON));
          DEBUGMSG(status,NU_ALLOC_ERR)

          // Return "Single block" purpose.
          ((T_MPHP_SINGLE_BLOCK_CON *)(msg->SigP))->purpose       =
            l1pa_l1ps_com.transfer.aset->assignment_command;
          ((T_MPHP_SINGLE_BLOCK_CON *)(msg->SigP))->assignment_id =
            l1pa_l1ps_com.transfer.aset->assignment_id;

          // Return status and CRC error (CRC error not applicable).
          ((T_MPHP_SINGLE_BLOCK_CON *)(msg->SigP))->status        = SINGLE_UL_DONE;
          ((T_MPHP_SINGLE_BLOCK_CON *)(msg->SigP))->dl_error_flag = 0;

          // MSG is sent to L1A to stop PCCCH or CCCH/BCCH reading.
          msg->SignalCode = L1P_SINGLE_BLOCK_CON;
          os_send_sig(msg, L1C1_QUEUE);
          DEBUGMSG(status,NU_SEND_QUEUE_ERR)
        }

        if(l1ps_dsp_com.pdsp_db_r_ptr->d_task_d_gprs & (0x80 >> 0))
        // DL block received...
        {
          UWORD16 pwr_level;
          
#if (L1_FF_MULTIBAND == 1)
          UWORD16 operative_radio_freq;
#endif /*(L1_FF_MULTIBAND == 1)*/

          // this bloc doesn't compute the burst input level, so the last_input_level is used.
          
#if (L1_FF_MULTIBAND == 0)
          
          pwr_level = l1a_l1s_com.last_input_level[l1a_l1s_com.Scell_info.radio_freq].input_level;

#else // L1_FF_MULTIBAND = 1 below

          operative_radio_freq = 
            l1_multiband_radio_freq_convert_into_operative_radio_freq(l1a_l1s_com.Scell_info.radio_freq);
          pwr_level = l1a_l1s_com.last_input_level[operative_radio_freq].input_level; 

#endif // #if (L1_FF_MULTIBAND == 0) else


          // Read L3 frame block and send msg to L1A.
          l1s_read_l3frm(pwr_level, &(l1ps_dsp_com.pdsp_ndb_ptr->a_dd_gprs[0][0]), task);
        }
      }
      else
      {
        // SYNCHRO task is not schedule if we are in the specific case:
        // L1A is touching SYNCHRO parameters (tn_difference, dl_tn and dsp_scheduler_mode)
        // and leave L1A to go in HISR (L1S) in middle of the update (cf. BUG1339)
        if(l1a_l1s_com.task_param[SYNCHRO] == SEMAPHORE_RESET)
        {
          //----------------
          // Synchro back
          //----------------
          // Save the "timeslot difference" between new and old configuration
          // in "tn_difference".
          //   tn_difference -> loaded with the number of timeslot to shift.
          //   dl_tn         -> loaded with the new timeslot.
          l1a_l1s_com.tn_difference += l1pa_l1ps_com.transfer.single_block.dl_tn_to_restore -
                                      l1a_l1s_com.dl_tn;
          l1a_l1s_com.dl_tn         = l1pa_l1ps_com.transfer.single_block.dl_tn_to_restore;

          // Select DSP Scheduler used in packet Idle (can be CCCH or PCCCH).
          if((l1a_l1s_com.l1s_en_task[ALLC] == TASK_ENABLED)
            || (l1a_l1s_com.l1s_en_task[NP] == TASK_ENABLED)
            || (l1a_l1s_com.l1s_en_task[EP] == TASK_ENABLED))
          {
            // We are in CS Idle on CCCH.
            l1a_l1s_com.dsp_scheduler_mode = GSM_SCHEDULER;
          }

          // Enable SYNCHRO task.
          l1a_l1s_com.l1s_en_task[SYNCHRO] = TASK_ENABLED;
        }

        //------------------
        // Confirmation msg
        //------------------
        // Common part...
        {
          // Send confirmation msg to L3/MACA.
          msg = os_alloc_sig(sizeof(T_MPHP_SINGLE_BLOCK_CON));
          DEBUGMSG(status,NU_ALLOC_ERR)


          ((T_MPHP_SINGLE_BLOCK_CON *)(msg->SigP))->purpose       =
            l1pa_l1ps_com.transfer.aset->assignment_command;
          ((T_MPHP_SINGLE_BLOCK_CON *)(msg->SigP))->assignment_id =
            l1pa_l1ps_com.transfer.aset->assignment_id;

          msg->SignalCode = L1P_SINGLE_BLOCK_CON;

          // Disable SINGLE task.
          l1s.task_status[task].current_status = INACTIVE;
          l1a_l1s_com.l1s_en_task[task]        = TASK_DISABLED;
        }

        // Differentiated part...

        if(l1pa_l1ps_com.transfer.aset->allocated_tbf == SINGLE_BLOCK_UL)
        // SINGLE UL task is complete.
        {
          // Return status and CRC error (CRC error not applicable).
          ((T_MPHP_SINGLE_BLOCK_CON *)(msg->SigP))->status        = SINGLE_UL_DONE;
          ((T_MPHP_SINGLE_BLOCK_CON *)(msg->SigP))->dl_error_flag = 0;
        }
        else
        if(l1ps.read_param.allocated_tbf == SINGLE_BLOCK_DL)
        // SINGLE DL task is complete.
        {
          API     *info_address;
          UWORD32  i,j;
          UWORD32  word32;

          info_address = &(l1ps_dsp_com.pdsp_ndb_ptr->a_dd_gprs[0][0]);

          // Return status and CRC error
          ((T_MPHP_SINGLE_BLOCK_CON *)(msg->SigP))->status        = SINGLE_DL_DONE;
          ((T_MPHP_SINGLE_BLOCK_CON *)(msg->SigP))->dl_error_flag = ((*info_address & 0x0100) >> 8);

          // Download data from API to message.

          // Get 24 bytes info. from DSP: CS1 meaningful block is of size 12 UWORD16 data.
          // !!! WARNING: word32 type is for compatibility with chipset == 0.
          // Can be word16 if only chipset == 2 is used.
          for (j=0, i=0; i<12; i++)
          {
            word32 = info_address[4 + i]; // Get info word, rem: skip info. header.
            ((T_MPHP_SINGLE_BLOCK_CON *)(msg->SigP))->data_array[j++] = (word32 & 0x000000ff);
            ((T_MPHP_SINGLE_BLOCK_CON *)(msg->SigP))->data_array[j++] = (word32 & 0x0000ff00) >> 8;
          }
        }

        // send message...
        os_send_sig(msg, L1C1_QUEUE);
        DEBUGMSG(status,NU_SEND_QUEUE_ERR)

      } // End else (!TWO_PHASE_ACCESS)
    }

    // Set flag used to change the read page at the end of "l1_synch".
    l1s_dsp_com.dsp_r_page_used = TRUE;

  } // End if(task enabled and semaphore false)

  else
  // When the task is aborted, we must continue to make dummy
  // DSP/TPU programming to avoid communication mismatch due
  // to C/W/R pipelining. Dummy MCU/DSP reading is also done.
  {
    // Set flag used to change the read page at the end of "l1_synch".
    l1s_dsp_com.dsp_r_page_used = TRUE;
  }
}

#if (MOVE_IN_INTERNAL_RAM == 0) // Must be followed by the pragma used to duplicate the funtion in internal RAM
//#pragma DUPLICATE_FOR_INTERNAL_RAM_START

/*-------------------------------------------------------*/
/* l1ps_read_pdtch()                                     */
/*-------------------------------------------------------*/
/*                                                       */
/* Description:                                          */
/* ------------                                          */
/*                                                       */
/* Input parameters:                                     */
/* -----------------                                     */
/*                                                       */
/* Input parameters from globals:                        */
/* ------------------------------                        */
/*                                                       */
/* Modified parameters from globals:                     */
/* ---------------------------------                     */
/*                                                       */
/*-------------------------------------------------------*/
void l1ps_read_pdtch(UWORD8 task, UWORD8 burst_id)
{
  if((l1a_l1s_com.l1s_en_task[task] == TASK_ENABLED) &&
    !(l1a_l1s_com.task_param[task] == SEMAPHORE_SET))
  // Check the task semaphore. The control body is executed only
  // when the task semaphore is 0. This semaphore can be set to
  // 1 whenever L1A makes some changes to the task parameters.
  {
    BOOL            beacon;
    T_INPUT_LEVEL  *IL_info_ptr;
    UWORD16         radio_freq;
    WORD8           ts       = 0;
    UWORD8          bit_mask = 0x80;
    WORD8           bcch_level;
    UWORD8          txpwr[8];
    UWORD8          rx_no = 0;
    BOOL            first_valid_block = TRUE;
    BOOL            crc_error =  TRUE;
    UWORD8          i;
    UWORD32         best_snr = 0;
    UWORD32         best_angle = 0;
    UWORD32         best_pm = 0;
    UWORD8          IL_for_rxlev[8], pr_table[8];
    WORD16          best_rxlev_accu = 0;


    static  BOOL    crc_error_tbl[8] = {TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE};
    static  WORD16  rxlev_accu[8]    = {0, 0, 0, 0, 0, 0, 0, 0};
    static  WORD8   burst_level[4] = {(WORD8)0x80, (WORD8)0x80, (WORD8)0x80, (WORD8)0x80};
    static  UWORD16 radio_freq_tbl[4];
    static  UWORD32 toa_val[4] = {0, 0, 0, 0};
    static  UWORD32 snr_val[4] = {0, 0, 0, 0};

    #if TESTMODE
      xSignalHeaderRec *msg;
      static UWORD32 tm_pm_fullres = 0;
      static UWORD32 tm_snr        = 0;
      static UWORD32 tm_toa        = 0;
      static WORD16  tm_angle      = 0;
    #endif


    #define burst_number  l1ps_dsp_com.pdsp_db_r_ptr->d_burst_nb_gprs
    #define DL_pwr_ctrl   l1pa_l1ps_com.transfer.dl_pwr_ctrl

    // Read parameters updating
    // *************************

    // ***********
    // * WARNING *
    // ***********

    // Because of the STI implementation, the parameters under the "l1pa_l1ps_com.transfer.aset"
    // structure mustn't be used in the Read PDTCH functions
    // This is due to the following case that may happen:
    //
    // C|W R    |
    //  |C W R  |
    //  |  C W R|       TBF 1
    //  |    C W|R <--------------------------- (2) This "read phase" must use the TBF 1 parameters while "aset" has already been updated
    //----------------------                        so the read_param structure is used
    //  |      C|W R <------------------------- (3) The read_param is updated with TBF 2 parameters
    //  |       |C W R                              at the beguining of the first Read of TBF 2
    //  |       |  C W R       TBF 2                so when (l1ps.read_param.new_set = TRUE and burst_id = BURST_1)
    //  |       |    C W R
    //         ^
    //        (1) TBF 2 starting time detected on this frame (no SYNCHRO change needed between TBF1 and TBF 2)
    //         --> aset parameters updated to TBF 2
    //         --> l1ps.read_param.new_set set to TRUE

    // If a new TBF has been enabled...
    if (l1ps.read_param.new_set == TRUE)
    {
      // If it's the first Read phase of the block (first of the new TBF)
      if (burst_id == BURST_1)
      {
        // Update the "read_param" structure
        l1ps_update_read_set_parameters();
      }
    }

    // Traces and debug.
    // ******************

    l1_check_com_mismatch(task);

    radio_freq = l1a_l1s_com.dedic_set.radio_freq_dd;

    if(l1ps.read_param.pc_meas_chan)
    {
      radio_freq_tbl[burst_number] =  radio_freq;
    }
    else
    {
      radio_freq_tbl[burst_number] =  l1a_l1s_com.Scell_info.radio_freq;
    }

    if (radio_freq == l1a_l1s_com.Scell_info.radio_freq)
    {
      beacon=1;
      IL_info_ptr = &l1a_l1s_com.Scell_info.traffic_meas_beacon;
    }
    else
    {
      beacon=0;
      IL_info_ptr = &l1a_l1s_com.Scell_info.traffic_meas;
    }

    // Call maca_power_control() in order to get TXPWR value
    if(l1ps_dsp_com.pdsp_db_r_ptr->d_burst_nb_gprs == 2)
    {
      UWORD8   txpwr[8];
      UWORD8   i;

      // Due to the CWR pipeleine, maca_power_control() has to be called before the
      // CTRL of the first PDTCH i.e. in l1ps_ctrl_pdtch(). It means that crc_error,
      // radio_freq_tbl[], burst_level[] and bcch_level information are stored on
      // burst4 of READ phase ("l1ps_ctrl_pdtch()") to be used on burst4 of CTRL phase.

      // Call Uplink Transmit Power level algorithm
      maca_power_control(l1ps.read_param.assignment_id,
                         DL_pwr_ctrl.crc_error,
                         DL_pwr_ctrl.bcch_level,
                         DL_pwr_ctrl.radio_freq_tbl,
                         DL_pwr_ctrl.burst_level,
                         txpwr);

      #if TESTMODE
        if(!l1_config.TestMode)
      #endif
        {
          for(i = 0; i < 8; i++)
          {
            l1pa_l1ps_com.transfer.dl_pwr_ctrl.txpwr[i] = txpwr[i];
          }
        }
      }


    if(l1ps_dsp_com.pdsp_db_r_ptr->d_burst_nb_gprs == 3)
    /*---------------------------------------------------*/
    /* Complete PDTCH DL block has been processed by DSP */
    /*---------------------------------------------------*/
    {
      #if (TRACE_TYPE == 5) // in simulation trace only the latest burst
        trace_fct(CST_L1PS_READ_PDTCH, radio_freq);
      #endif

      // Call MACS...
      l1ps_macs_read(pr_table);
    }

    // Update AGC and extract IL for RXLEV
    //------------------------------------
    if (l1ps.read_param.dl_pwr_ctl.p0 == 255)
    {
      // No power control mode AGC algorithm
      l1pctl_npc_agc_read(IL_for_rxlev, l1ps_dsp_com.pdsp_db_r_ptr, l1ps_dsp_com.pdsp_ndb_ptr);
    }
    else
    {
      // Downlink power control AGC algorithms
      if (l1ps.read_param.dl_pwr_ctl.bts_pwr_ctl_mode == 0)
      {
        // BTS Power control mode A
        l1pctl_dpcma_agc_read(IL_for_rxlev, l1ps_dsp_com.pdsp_db_r_ptr, l1ps_dsp_com.pdsp_ndb_ptr, pr_table);
      }
      else
      {
        // BTS power control mode B
        l1pctl_dpcmb_agc_read(IL_for_rxlev, l1ps_dsp_com.pdsp_db_r_ptr, l1ps_dsp_com.pdsp_ndb_ptr, pr_table);
      }
    } // End of "AGC algorithm"

    // TOA algorithm is called with toa/snr pair from last block (N-1)
    // Feed TOA histogram with values from good bursts (crc_error = FALSE)
    // otherwise input snr = 0.
    #if (TOA_ALGO != 0)
      // Good block, TOA from TS=0
      #if (TOA_ALGO == 2)
        if(l1s.toa_var.toa_snr_mask == 0)
      #else
        if(l1s.toa_snr_mask == 0)
      #endif
      {
        UWORD32 snr_temp;
        snr_temp = (crc_error_tbl[0] == FALSE) ? snr_val[burst_id] : 0;
        #if (TOA_ALGO == 2)
        {
          l1s.toa_var.toa_shift = l1ctl_toa(TOA_RUN, l1a_l1s_com.mode, snr_temp, toa_val[burst_id]);
        }
        #else
        {
          l1s.toa_shift = l1ctl_toa(TOA_RUN, l1a_l1s_com.mode, snr_temp, toa_val[burst_id], &l1s.toa_update, &l1s.toa_period_count
#if (FF_L1_FAST_DECODING == 1)
              ,0
#endif
              );
        }
        #endif
      }
    #endif

    /*---------------------------------------------------*/
    /* Read burst demodulation info for control algos    */
    /* Use all burst results to feed the algos.          */
    /*---------------------------------------------------*/
    while(ts < 8)
    {
      if(l1ps_dsp_com.pdsp_db_r_ptr->d_task_d_gprs & bit_mask)
      {
        UWORD32   toa;
        UWORD32   pm;
        UWORD32   angle;
        UWORD32   snr;
        WORD8     rxlev;

        // Read control results and feed control algorithms.
        // **************************************************

        // Read control information.
        toa   = l1ps_dsp_com.pdsp_db_r_ptr->a_burst_toa_gprs[ts]   & 0xffff;
        pm    = (l1ps_dsp_com.pdsp_db_r_ptr->a_burst_pm_gprs[ts]   & 0xffff);
        angle = l1ps_dsp_com.pdsp_db_r_ptr->a_burst_angle_gprs[ts] & 0xffff;
        snr   = l1ps_dsp_com.pdsp_db_r_ptr->a_burst_snr_gprs[ts]   & 0xffff;

        #if (TRACE_TYPE != 0) && (TRACE_TYPE != 5) // for debug trace all bursts
          trace_fct(CST_L1PS_READ_PDTCH_BURST, (UWORD32)(-1));
        #endif

        l1_check_pm_error(pm,task);
	pm = pm >> 5;

        #if TESTMODE
        // Test mode stats
          if (l1_config.TestMode)
          {
            if (bit_mask & l1_config.tmode.stats_config.stat_gprs_slots)
            {
              tm_pm_fullres += (l1ps_dsp_com.pdsp_db_r_ptr->a_burst_pm_gprs[ts]   & 0xffff);
              tm_snr        += snr;
              tm_toa        += toa;
              tm_angle      += (WORD16) angle; // signed
            }
          }
        #endif

        #if (TRACE_TYPE == 1) || (TRACE_TYPE == 4)
          RTTL1_FILL_DL_BURST(angle, snr, l1s.afc, task, pm, toa, IL_for_rxlev[ts])
        #endif
        #if (TRACE_TYPE == 1) || (TRACE_TYPE == 4)
          l1_trace_burst_param(angle, snr, l1s.afc, task, pm, toa, IL_for_rxlev[ts]);
        #endif
        #if (BURST_PARAM_LOG_ENABLE == 1)
          l1_log_burst_param(angle, snr, l1s.afc, task, pm, toa, IL_for_rxlev[ts]);
        #endif

        //Look for the pairs angle, snr with the maximum snr
        if (snr > best_snr)
        {
          best_snr = snr;
          best_angle = angle;
          best_pm = pm;
        }

        // store toa value from first TS
        if (ts==0)
        {
          toa_val[burst_id]  = toa;
          snr_val[burst_id]  = snr;
        }

        // Store Received Signal Level to be used in Uplink Transmit Power Algorithm.

        // Compute RXLEV
        rxlev = l1s_encode_rxlev(IL_for_rxlev[ts]);

        // Find first correct PDTCH, save RXLEV and CRC
        if(!crc_error_tbl[ts] && first_valid_block)
        {
          if(l1ps.read_param.pc_meas_chan)
          {
            burst_level[burst_number] = rxlev;
          }
          else
          {
            burst_level[burst_number] = (WORD8)0x80;
          }

          // Measures on first valid block have been performed. Reset flag.
          first_valid_block = FALSE;

          //  Save crc_error
          crc_error = crc_error_tbl[ts];

        } // End of measurements storage

        // If All PDTCH are incorrect (bad CRC) save RXLEV and CRC of the best PDTCH
        if(first_valid_block)
        {
          rxlev_accu[ts] += rxlev;

          if(rxlev_accu[ts] > best_rxlev_accu)
          {
            best_rxlev_accu = rxlev_accu[ts];
            crc_error       = crc_error_tbl[ts];

            if(l1ps.read_param.pc_meas_chan)
            {
              burst_level[burst_number] = rxlev;
            }
            else
            {
              burst_level[burst_number] = (WORD8)0x80;
            }
          }
        }

        // Determine first valid block to be used in next radio block
        if(l1ps_dsp_com.pdsp_db_r_ptr->d_burst_nb_gprs == 3)
        /*---------------------------------------------------*/
        /* Complete PDTCH DL block has been processed by DSP */
        /*---------------------------------------------------*/
        {
          crc_error_tbl[ts] = ((l1ps_dsp_com.pdsp_ndb_ptr->a_dd_gprs[rx_no][0] & 0x0100) >> 8);

          // Increment Rx burst number
          rx_no++;
        }

        #if TRACE_TYPE==3
          stats_samples_nb(toa,pm,angle,snr,burst_id,task);
        #endif

      }  // End of if(l1ps_dsp_com.pdsp_db_r_ptr->d_task_d_gprs & bit_mask)

      // Increment timeslot
      ts++;

      // Shift Mask.
      bit_mask >>= 1;

    } // End of while(ts < 8)

    // AFC control algorithm is called with values retrieved from
    // burst with max. snr
    // AFC algorithm is called on bursts 0 and 2: this is sufficient to
    // have a correct behavior and this permits to gain CPU
    // Update AFC: Call AFC control function (KALMAN filter).
    #if AFC_ALGO
      #if TESTMODE
        if (l1_config.afc_enable)
      #endif
        {
          if((burst_id == 0) || (burst_id == 2))
            #if (VCXO_ALGO == 0)
              l1s.afc = l1ctl_afc(AFC_CLOSED_LOOP, &l1s.afc_frame_count, (WORD16)best_angle, best_snr, radio_freq);
            #else
              l1s.afc = l1ctl_afc(AFC_CLOSED_LOOP, &l1s.afc_frame_count, (WORD16)best_angle, best_snr, radio_freq,l1a_l1s_com.mode);
            #endif
        }
    #endif

    #if (TRACE_TYPE == 1)||(TRACE_TYPE == 4)
        if (trace_info.current_config->l1_dyn_trace & 1<<L1_DYN_TRACE_CONDENSED_PDTCH)
        {
          if ((l1pa_l1ps_com.tcr_freq_list.ms_ctrl_dd != 0) ||
              ((l1ps.pc_meas_chan_ctrl == TRUE) &&
               ((l1s.actual_time.t2 == 3) || (l1s.actual_time.t2 == 11) ||
                (l1s.actual_time.t2 == 20))))
            trace_info.pdtch_trace.blk_status |= 0x80 >> burst_id;
        }
    #endif

    if(l1ps_dsp_com.pdsp_db_r_ptr->d_burst_nb_gprs == 3)
    /*---------------------------------------------------*/
    /* Complete PDTCH DL block has been processed by DSP */
    /*---------------------------------------------------*/
    {

      l1pa_l1ps_com.transfer.dl_pwr_ctrl.crc_error      = crc_error;

      if(l1ps.read_param.pc_meas_chan)
      {

        // Due to the CWR pipeleine, maca_power_control() has to be called before the
        // CTRL of the first PDTCH i.e. in l1ps_ctrl_pdtch(). It means that crc_error,
        // radio_freq_tbl[], burst_level[] and bcch_level information are stored on
        // burst4 of READ phase to be used on burst4 of CTRL phase.

        l1pa_l1ps_com.transfer.dl_pwr_ctrl.bcch_level = (WORD8)0x80;

        for(i = 0; i < 4; i++)
        {
          l1pa_l1ps_com.transfer.dl_pwr_ctrl.radio_freq_tbl[i] = radio_freq_tbl[i];
          l1pa_l1ps_com.transfer.dl_pwr_ctrl.burst_level[i]    = burst_level[i];
        }

      }
      else
      {
        // Measures have been performed on BCCH Serving Cell. "burst_level" table is
        // not applicable.

        // Download measures made on BCCH Serving Cell.
        l1pa_l1ps_com.transfer.dl_pwr_ctrl.bcch_level = l1pa_l1ps_com.tcr_freq_list.beacon_meas;

        for(i = 0; i < 4; i++)
        {
          l1pa_l1ps_com.transfer.dl_pwr_ctrl.radio_freq_tbl[i] = radio_freq_tbl[i];
          l1pa_l1ps_com.transfer.dl_pwr_ctrl.burst_level[i]    = (WORD8)0x80;
        }

        // Measures on BCCH Serving Cell are only performed every 40ms while
        // maca_power_control() is called every 20ms. "beacon_meas" must then
        // be set to invalid (0x80) until next Serving Cell measure.
        l1pa_l1ps_com.tcr_freq_list.beacon_meas = (WORD8)0x80;

      }

      #if TESTMODE
      // Test mode stats
        if (l1_config.TestMode)
        {
          // Allocate result message.
          msg = os_alloc_sig(sizeof(T_TMODE_PDTCH_INFO));
          DEBUGMSG(status,NU_ALLOC_ERR)
          msg->SignalCode = TMODE_PDTCH_INFO;

          ((T_TMODE_PDTCH_INFO *)(msg->SigP))->pm_fullres    = tm_pm_fullres; // F26.6
          ((T_TMODE_PDTCH_INFO *)(msg->SigP))->snr           = tm_snr;
          ((T_TMODE_PDTCH_INFO *)(msg->SigP))->toa           = tm_toa;
          ((T_TMODE_PDTCH_INFO *)(msg->SigP))->angle         = tm_angle; // signed
          for (i=0;i<8;i++)
            ((T_TMODE_PDTCH_INFO *)(msg->SigP))->crc_error_tbl[i] = crc_error_tbl[i];

          // send TMODE_TCH_INFO message...
          os_send_sig(msg, L1C1_QUEUE);
          DEBUGMSG(status,NU_SEND_QUEUE_ERR)

          // reset static TM variables for stats collection
          tm_pm_fullres = 0;
          tm_snr        = 0;
          tm_toa        = 0;
          tm_angle      = 0;
        }
     #endif
    }

  } //end of test "if((en_task) && !(task_param))"

  // End of task -> task must become INACTIVE.
  // PDTCH can be pipelined and therefore must stay active if
  // it has already reentered the flow.
  if(burst_id == BURST_4)
  {
    if(l1s.task_status[task].current_status == RE_ENTERED)
      l1s.task_status[task].current_status = ACTIVE;
    else
      l1s.task_status[task].current_status = INACTIVE;
  }

  l1ddsp_read_iq_dump(task);
  // Flag the use of the MCU/DSP dual page read interface.
  // ******************************************************

  // Set flag used to change the read page at the end of "l1_synch".
  l1s_dsp_com.dsp_r_page_used = TRUE;
}
//#pragma DUPLICATE_FOR_INTERNAL_RAM_END
#endif // MOVE_IN_INTERNAL_RAM

/*-------------------------------------------------------*/
/* l1ps_read_pra_result()                                */
/*-------------------------------------------------------*/
/* Parameters :                                          */
/* Return     :                                          */
/* Functionality :                                       */
/*-------------------------------------------------------*/
void l1ps_read_pra_result(UWORD8 task, UWORD8 burst_id)
{
  /*--------------------------------------------------------*/
  /* READ TRANSMIT TASK RESULTS...                          */
  /*--------------------------------------------------------*/

  /*---------------------------------------------------*/
  /* Packet Access task.                               */
  /*---------------------------------------------------*/
  // Rem: confirmation message is sent at "CTRL" to be able to give FN%42432.
  BOOL confirm_flag = TRUE; // Default is: confirmation message is sent.

  // Desactivate the PRACH task.
  l1s.task_status[task].current_status = INACTIVE;

  l1_check_com_mismatch(task);


  #if (TRACE_TYPE!=0)
    trace_fct(CST_L1PS_READ_PRA, l1pa_l1ps_com.p_idle_param.radio_freq);
  #endif

  #if FF_L1_IT_DSP_USF
    // Check PRACH was controlled
    if (l1pa_l1ps_com.pra_info.prach_controlled)
    {
  #endif

  // Check USF in case of Dynamic Allocation.
  if(l1pa_l1ps_com.pra_info.prach_alloc != FIX_PRACH_ALLOC)
  {
    API cs_type = l1ps_dsp_com.pdsp_ndb_ptr->a_du_gprs[0][0];

    if(cs_type != CS_NONE_TYPE)
      confirm_flag = FALSE;
  }

  if (confirm_flag == TRUE)
  {
    // Send confirmation msg to L1A.
    // ******************************
    // For ACCESS phase, a confirmation msg is sent to L1A.
    xSignalHeaderRec *msg;

    // send L1C_RA_DONE to L1A...
    msg = os_alloc_sig(sizeof(T_MPHP_RA_CON));
    DEBUGMSG(status,NU_ALLOC_ERR)

    ((T_MPHP_RA_CON *)(msg->SigP))->fn                   = l1pa_l1ps_com.pra_info.fn_to_report;
    ((T_MPHP_RA_CON *)(msg->SigP))->channel_request_data = l1pa_l1ps_com.pra_info.channel_request_data;
    msg->SignalCode = L1P_RA_DONE;

    os_send_sig(msg, L1C1_QUEUE);
    DEBUGMSG(status,NU_SEND_QUEUE_ERR)

  }

  // Set flag used to change the read page at the end of "l1_synch".
  l1s_dsp_com.dsp_r_page_used = TRUE;

  #if FF_L1_IT_DSP_USF
    } // if (l1pa_l1ps_com.pra_info.prach_controlled)
  #endif
}

/*-------------------------------------------------------*/
/* l1ps_read_poll_result()                               */
/*-------------------------------------------------------*/
/* Parameters :                                          */
/* Return     :                                          */
/* Functionality :                                       */
/*-------------------------------------------------------*/
void l1ps_read_poll_result(UWORD8 task, UWORD8 burst_id)
{
  /*--------------------------------------------------------*/
  /* READ TRANSMIT TASK RESULTS...                          */
  /*--------------------------------------------------------*/

  l1_check_com_mismatch(task);

  #if (TRACE_TYPE!=0) && (TRACE_TYPE!=5) // in debug trace all reads
    trace_fct(CST_L1PS_READ_POLL, l1pa_l1ps_com.p_idle_param.radio_freq);
  #endif

  /*--------------------------------------------------------*/
  /* POLL task (4xPRACH) upon packet queueing notification. */
  /*--------------------------------------------------------*/

  // Deactivate the PRACH task.
  if(burst_id == BURST_4)
  {
    // POLL is a 'one shot' task --> disable task
    l1a_l1s_com.l1s_en_task[task] = TASK_DISABLED;

    l1s.task_status[task].current_status = INACTIVE;

    #if (TRACE_TYPE==5) // in simulation trace only the latest burst
      trace_fct(CST_L1PS_READ_POLL, l1pa_l1ps_com.p_idle_param.radio_freq);
    #endif

    // Send confirmation msg to L1A.
    // ******************************
    // For PACKET POLLING, a confirmation msg is sent to L1A.
    {
      xSignalHeaderRec *msg;

      // send L1C_RA_DONE to L1A...
      msg = os_alloc_sig(sizeof(T_MPHP_POLLING_IND));
      DEBUGMSG(status,NU_ALLOC_ERR)

      ((T_MPHP_POLLING_IND *)(msg->SigP))->fn = l1pa_l1ps_com.poll_info.fn_to_report;
      msg->SignalCode = L1P_POLL_DONE;

      os_send_sig(msg, L1C1_QUEUE);
      DEBUGMSG(status,NU_SEND_QUEUE_ERR)
    }
  }

  l1ddsp_read_iq_dump(task);
  // Set flag used to change the read page at the end of "l1_synch".
  l1s_dsp_com.dsp_r_page_used = TRUE;
}

#if !((MOVE_IN_INTERNAL_RAM == 1) && (GSM_IDLE_RAM !=0))  // MOVE TO INTERNAL MEM IN CASE GSM_IDLE_RAM enabled
//#pragma GSM_IDLE_DUPLICATE_FOR_INTERNAL_RAM_START         // KEEP IN EXTERNAL MEM otherwise

/*-------------------------------------------------------*/
/* l1ps_ctrl_snb_dl()                                    */
/*-------------------------------------------------------*/
/*                                                       */
/* Description:                                          */
/* ------------                                          */
/* This function is a "COMPLEX" function used by the L1S */
/* packet serving cell normal burst reading tasks: PNP,  */
/* PEP, PALLC. This function is the control function for */
/* reading a normal burst on the packet serving cell.    */
/* It programs the DSP and the TPU for reading a         */
/* normal burst. This function flags the reading of the  */
/* Packet Normal paging burst which flag is used in      */
/* measurement manager procedure.                        */
/* Here below is a summary of the execution:             */
/*                                                       */
/*  - If SEMAPHORE(task) is low.                         */
/*      - Catch ARFCN and set CIPHERING reduced frame    */
/*        number.                                        */
/*      - Traces and debug.                              */
/*      - Programs DSP for required task.                */
/*      - Programs TPU for required task.                */
/*      - Flag the reading of a Packet Normal Paging     */
/*        burst.                                         */
/*  - Flag DSP and TPU programmation.                    */
/*                                                       */
/* Input parameters:                                     */
/* -----------------                                     */
/* "l1a_l1s_com.task_param"                              */
/*   task semaphore bit register. Used to skip           */
/*   the body of this function if L1A has changed or     */
/*   is changing some of the task parameters.            */
/*                                                       */
/* "task"                                                */
/*   PNP, Packet Normal paging reading task.             */
/*   PEP, Packet Extended paging reading task.           */
/*   PALLC, All Packet serving cell PCCCH reading task.  */
/*                                                       */
/* "burst_id"                                            */
/*   BURST_1, 1st burst of the task.                     */
/*   BURST_2, 2nd burst of the task.                     */
/*   BURST_3, 3rd burst of the task.                     */
/*   BURST_4, 4th burst of the task.                     */
/*                                                       */
/* Input parameters from globals:                        */
/* ------------------------------                        */
/* "l1a_l1s_com.Scell_info"                              */
/*  Serving cell information structure.                  */
/*    .radio_freq, serving cell beacon frequency.        */
/*                                                       */
/* "l1s.afc"                                             */
/*   current AFC value to be applied for the given task. */
/*                                                       */
/* "l1s.tpu_offset"                                      */
/*   value for the TPU SYNCHRO and OFFSET registers      */
/*   for current serving cell setting. It is used here   */
/*   to refresh the TPU SYNCHRO and OFFSET registers     */
/*   with a corrected (time tracking of the serving)     */
/*   value prior to reading a serving cell normal burst. */
/*                                                       */
/* Modified parameters from globals:                     */
/* ---------------------------------                     */
/* "pnp_ctrl"                                            */
/*   Flag set when a packet normal paging burst reading  */
/*   is controled. This flag is used by the packet       */
/*   measurement manager procedure, at the end of L1S,   */
/*    in order to scheduling the neighbor cell           */
/*    measurements.                                      */
/*   -> set to 1.                                        */
/*                                                       */
/*                                                       */
/* "l1s.tpu_ctrl_reg"                                    */
/*   bit register used to know at the end of L1S if      */
/*   something has been programmed on the MCU/TPU com.   */
/*   This is used mainly to swap then the com. page at   */
/*   the end of a control frame.                         */
/*   -> set CTRL_RX bit in the register.                 */
/*                                                       */
/* "l1s.dsp_ctrl_reg"                                    */
/*   bit register used to know at the end of L1S if      */
/*   something has been programmed on the MCU/DSP com.   */
/*   This is used mainly to swap then the com. page at   */
/*   the end of a control frame.                         */
/*   -> set CTRL_RX bit in the register.                 */
/*                                                       */
/*-------------------------------------------------------*/
void l1ps_ctrl_snb_dl(UWORD8 task, UWORD8 burst_id)
{
  UWORD16  Scell_radio_freq;
  UWORD8   tsc;
  WORD8    agc;
  UWORD8   lna_off;
  UWORD8   adc_active = INACTIVE;
 #if (RF_FAM == 61)
      UWORD16 dco_algo_ctl_nb = 0;
      UWORD8 if_ctl = 0;
	  UWORD8 if_threshold = C_IF_ZERO_LOW_THRESHOLD_GPRS;
 #endif
  // By default we choose the hardware filter
  UWORD8 csf_filter_choice = L1_SAIC_HARDWARE_FILTER;

  #if (NEW_SNR_THRESHOLD == 1)
    UWORD8 saic_flag = 0;
  #endif /* NEW_SNR_THRESHOLD */


#if (FF_L1_FAST_DECODING == 1)
  BOOL fast_decoding_authorized = FALSE;

  if ( (burst_id == BURST_1) && (l1a_apihisr_com.fast_decoding.status == C_FAST_DECODING_FORBIDDEN) )
  {
    l1a_apihisr_com.fast_decoding.status = C_FAST_DECODING_NONE;
  }

  fast_decoding_authorized = l1s_check_fast_decoding_authorized(task);

  if ( fast_decoding_authorized && l1s_check_deferred_control(task,burst_id) )
  {
    /* Control is deferred until the upcoming fast decoding IT */
    return;
  } /* if (fast_decoding_authorized)*/

  /* In all other cases, control must be performed now. */
#endif /* FF_L1_FAST_DECODING == 1 */

  if(!(l1a_l1s_com.task_param[task] == SEMAPHORE_SET))
  // Check the task semaphore. The control body is executed only
  // when the task semaphore is 0. This semaphore can be set to
  // 1 whenever L1A makes some changes to the task parameters.
  {
    Scell_radio_freq = l1a_l1s_com.Scell_info.radio_freq;

    // Catch training sequence code from serving cell BCC (part of BSIC).
    tsc = l1pa_l1ps_com.pccch.packet_chn_desc.tsc;

    // Packet PAGC Algorithm
    // **********************

    // for PCCCH serving blocks (PPCH, PEPCH, all PCCCH) we use
    // PAGC algorithm. Reference is serving cell.
    l1pctl_pagc_ctrl(&agc, &lna_off, l1pa_l1ps_com.p_idle_param.radio_freq,TRUE);

   #if(RF_FAM == 61)   // Locosto DCO
       cust_get_if_dco_ctl_algo(&dco_algo_ctl_nb, &if_ctl, (UWORD8) L1_IL_VALID ,
                                             l1a_l1s_com.Scell_used_IL.input_level,
                                             l1pa_l1ps_com.p_idle_param.radio_freq, if_threshold);
    	l1ddsp_load_dco_ctl_algo_nb(dco_algo_ctl_nb);
  #endif

    #if (L1_SAIC != 0)
      // If SAIC is enabled, call the low level SAIC control function
      // NOTE: l1a_l1s_com.Scell_used_IL.input_level is updated within
      //       the function l1pctl_pagc_ctrl
      csf_filter_choice = l1ctl_saic(l1a_l1s_com.Scell_used_IL.input_level,l1a_l1s_com.mode
      #if (NEW_SNR_THRESHOLD == 1)
          ,task
          ,&saic_flag
      #endif
          );
   #endif

    // Debug.
    // ******************

    l1s_dsp_com.dsp_db_w_ptr->d_debug = (l1s.debug_time + 2) ;
#if (FF_L1_FAST_DECODING == 1)
    l1ddsp_load_fast_dec_task(task,burst_id);
#endif

    // Programs DSP Rx Packet Idle burst, still on Timeslot number = 0
    // due to previous synchro.
  #if FF_L1_IT_DSP_USF
    {
      BOOL usf_it = FALSE;

      // Force IT USF interrupt during PCCCH reorg for Fast USF usage during
      // Packet Access. Switch to PA could happen any time during PCCCH reorg.
      // Only relevant for RBN%3 = 0 and 1
      if (l1a_l1s_com.l1s_en_task[PALLC] == TASK_ENABLED)
      {
        if (/*(l1s.next_time.fn_mod13 >= 0) && omaps00090550*/(l1s.next_time.fn_mod13 <= 7))
          usf_it = TRUE;
      }

      l1pddsp_idle_rx_nb(burst_id, tsc, l1pa_l1ps_com.p_idle_param.radio_freq, 0, FALSE, usf_it);
    }
  #else
    l1pddsp_idle_rx_nb(burst_id, tsc, l1pa_l1ps_com.p_idle_param.radio_freq, 0, FALSE);
  #endif

    // ADC measurement
    // ***************
    // check if during the 1st burst of the bloc an ADC measurement must be performed
    if ((burst_id == BURST_1) && (task == PNP))
    {
      if (l1a_l1s_com.l1s_en_task[PALLC] == TASK_DISABLED) // no reorg mode
      {
         if (l1a_l1s_com.adc_mode & ADC_NEXT_NORM_PAGING)  // perform ADC only one time
         {
            adc_active = ACTIVE;
            l1a_l1s_com.adc_mode &= ADC_MASK_RESET_IDLE; // reset in order to have only one ADC measurement in Idle
         }
         else
         {
           if (l1a_l1s_com.adc_mode & ADC_EACH_NORM_PAGING) // perform ADC on each "period" x bloc
             if ((++l1a_l1s_com.adc_cpt)>=l1a_l1s_com.adc_idle_period) // wait for the period
             {
               adc_active = ACTIVE;
               l1a_l1s_com.adc_cpt = 0;
             }
         }
      }
      else  // ADC measurement in reorg mode
      {
         if (l1a_l1s_com.adc_mode & ADC_NEXT_NORM_PAGING_REORG)  // perform ADC only one time
         {
            adc_active = ACTIVE;
            l1a_l1s_com.adc_mode &= ADC_MASK_RESET_IDLE; // reset in order to have only one ADC measurement in Idle
         }
         else
         {
           if (l1a_l1s_com.adc_mode & ADC_EACH_NORM_PAGING_REORG) // perform ADC on each "period" x bloc
             if ((++l1a_l1s_com.adc_cpt)>=l1a_l1s_com.adc_idle_period) // wait for the period
             {
               adc_active = ACTIVE;
               l1a_l1s_com.adc_cpt = 0;
             }
         }
      }
    }

    #if (TRACE_TYPE!=0) && (TRACE_TYPE!=5)
      trace_fct(CST_L1PS_CTRL_SNB_DL, 1); //OMAPS00090550
    #endif

    // Programs TPU for required task.
    // ********************************

    // update the TPU with the new TOA if necessary
    l1ctl_update_TPU_with_toa();

    // tpu pgm...
    l1dtpu_serv_rx_nb(l1pa_l1ps_com.p_idle_param.radio_freq,
                      agc,
                      lna_off,
                      l1s.tpu_offset,
                      l1s.tpu_offset,
                      FALSE,adc_active,
                      csf_filter_choice
                 #if (RF_FAM == 61)
                      ,if_ctl
		   #endif
                #if (NEW_SNR_THRESHOLD == 1)
                      ,saic_flag
                #endif /* NEW_SNR_THRESHOLD */
	               );

    // Increment tpu window identifier.
    l1s.tpu_win += (l1_config.params.rx_synth_load_split + RX_LOAD);
  }

  // Flag the reading of a Normal Packet Paging burst.
  // *************************************************

  // Set PNP controlled flag, used in l1s_meas_manager() to generate measurement only
  // if we are not receiving a PPCH.
  if((task == PNP) || (task == PEP) || (task == PALLC))
    l1pa_l1ps_com.cr_freq_list.pnp_ctrl = burst_id + 1;


  // Flag DSP and TPU programmation.
  // ********************************

  // Set "CTRL_RX" flag in the controle flag register.
  l1s.tpu_ctrl_reg |= CTRL_RX;
  l1s.dsp_ctrl_reg |= CTRL_RX;

} // end of procedure

/*-------------------------------------------------------*/
/* l1ps_read_nb_dl()                                     */
/*-------------------------------------------------------*/
/*                                                       */
/* Description:                                          */
/* ------------                                          */
/* This function is a "COMPLEX" function used by the L1S */
/* tasks: PNP,PEP,PALLC.                                 */
/*                                                       */
/* Here is a summary of the execution:                   */
/*                                                       */
/*  - If SEMAPHORE(task) is low and task still enabled.  */
/*      - Traces and debug.                              */
/*      - Read control results and feed control algo.    */
/*      - Read DL DATA block from MCU/DSP interface.     */
/*  - Disactivate task.                                  */
/*  - Flag the use of the MCU/DSP dual page read         */
/*    interface.                                         */
/*                                                       */
/* Input parameters:                                     */
/* -----------------                                     */
/* "task"                                                */
/*   PNP, Packet Normal paging reading task.             */
/*   PEP, Packet Extended paging reading task.           */
/*   PALLC, All packet serving cell PCCCH reading task.  */
/*                                                       */
/* "burst_id"                                            */
/*   BURST_1, 1st burst of the task.                     */
/*   BURST_2, 2nd burst of the task.                     */
/*   BURST_3, 3rd burst of the task.                     */
/*   BURST_4, 4th burst of the task.                     */
/*                                                       */
/* Input parameters from globals:                        */
/* ------------------------------                        */
/* "l1pa_l1ps_com.task_param[NBR_DL_L1S_TASKS]"          */
/*   packet task semaphore table. Used to skip           */
/*   the body of this function if L1A has changed or     */
/*   is changing some of the task parameters.            */
/*                                                       */
/* "l1a_l1s_com.l1s_en_task[NBR_DL_L1S_TASKS]"           */
/*   L1S task enable.                                    */
/*                                                       */
/* Modified parameters from globals:                     */
/* ---------------------------------                     */
/* "l1s.task_status[task].current_status"                */
/*   current task status. It must be reset (INACTIVE)    */
/*   when the task is completed.                         */
/*   -> disactivate task.                                */
/*                                                       */
/* "l1s_dsp_com.dsp_r_page_used"                         */
/*   Flag used by the function which closes L1S          */
/*   execution ("l1s_end_manager()") to know if the      */
/*   MCU/DSP read page must be switched.                 */
/*   -> Set to 1.                                        */
/*                                                       */
/* Use of MCU/DSP interface:                             */
/* -------------------------                             */
/* "l1s_dsp_com.dsp_ndb_ptr"                             */
/*   pointer to the non double buffered part (NDB) of    */
/*   the MCU/DSP interface. This part is R/W for both    */
/*   DSP and MCU.                                        */
/*                                                       */
/* "l1s_dsp_com.dsp_db_r_ptr"                            */
/*   pointer to the double buffered part (DB) of the     */
/*   MCU/DSP interface. This pointer points to the READ  */
/*   page.                                               */
/*                                                       */
/*-------------------------------------------------------*/
void l1ps_read_nb_dl(UWORD8 task, UWORD8 burst_id)
{
  UWORD32   toa=0; //omaps00090550
  UWORD32   pm=0; //omaps00090550;
  UWORD32   angle =0; //omaps00090550
  UWORD32   snr =0;  //omaps00090550
  BOOL      en_task;
  BOOL      task_param;
  UWORD16   scell_radio_freq;
  static UWORD16   pwr_level;

#if (FF_L1_FAST_DECODING == 1)
  UWORD8 skipped_bursts = 0;
  BOOL fast_decoding_authorized = l1s_check_fast_decoding_authorized(task);
  BOOL fast_decoded = (l1a_apihisr_com.fast_decoding.status == C_FAST_DECODING_COMPLETE);
  if (fast_decoded)
  {
    skipped_bursts = BURST_4 - burst_id;
  }
#endif /* if (FF_L1_FAST_DECODING == 1) */

  /*--------------------------------------------------------*/
  /* READ SERVING CELL RECEIVE TASK RESULTS...              */
  /*--------------------------------------------------------*/
  /* Rem: only a partial result is present in the mcu<-dsp  */
  /* communication buffer. The DATA BLOCK content itself is */
  /* in the last comm. (BURST_4)                            */
  /*--------------------------------------------------------*/
  // Get "enable" task flag and "synchro semaphore" for current task.
  en_task    = l1a_l1s_com.l1s_en_task[task];
  task_param = l1a_l1s_com.task_param[task];

  if((en_task) && !(task_param))
  // Check the task semaphore and the task enable bit. The reading
  // task body is executed only when the task semaphore is 0 and the
  // task is still enabled.
  // The semaphore can be set to 1 whenever L1A makes some changes
  // to the task parameters. The task can be disabled by L1A.
  {
    // Traces and debug.
    // ******************
    l1_check_com_mismatch(task);

    // Read control results and feed control algorithms.
    // **************************************************
    if ((task != PBCCHN_TRAN) && (task != PBCCHN_IDLE))
    {
      // From the fact that PBCCHS can be read in CS mode,
      // Idle mode and Packet Idle mode, a check on the current active DSP scheduler mode
      // has to be performed.
      // Read control information.
      // We keep compatibility with (chipset == 0) imply mask with 0xffff.
      // If only (chipset == 2) is used, mask can be removed.
      if (l1a_l1s_com.dsp_scheduler_mode == GSM_SCHEDULER)
      {
        toa   = l1s_dsp_com.dsp_db_r_ptr->a_serv_demod[D_TOA]   & 0xffff;
        pm    = (l1s_dsp_com.dsp_db_r_ptr->a_serv_demod[D_PM]   & 0xffff);
        angle = l1s_dsp_com.dsp_db_r_ptr->a_serv_demod[D_ANGLE] & 0xffff;
        snr   = l1s_dsp_com.dsp_db_r_ptr->a_serv_demod[D_SNR]   & 0xffff;
      }
      else
      {
        toa   =  l1ps_dsp_com.pdsp_db_r_ptr->a_burst_toa_gprs[0]   & 0xffff;
        pm    = (l1ps_dsp_com.pdsp_db_r_ptr->a_burst_pm_gprs[0]    & 0xffff);
        angle =  l1ps_dsp_com.pdsp_db_r_ptr->a_burst_angle_gprs[0] & 0xffff;
        snr   =  l1ps_dsp_com.pdsp_db_r_ptr->a_burst_snr_gprs[0]   & 0xffff;
      }

      #if (TRACE_TYPE!=0) && (TRACE_TYPE!=5)
        trace_fct(CST_L1PS_READ_NB_DL, 1); //OMAPS00090550
      #endif

      l1_check_pm_error(pm,task);
      pm = pm >> 5;

      // Update AGC: Call PAGC algorithm
      l1a_l1s_com.Scell_IL_for_rxlev = l1pctl_pagc_read((UWORD8)pm, l1pa_l1ps_com.p_idle_param.radio_freq_dd);

#if (FF_L1_FAST_DECODING == 1)
    if (skipped_bursts>0)
    {
      l1ctl_pagc_missing_bursts(skipped_bursts);
    }
#endif /* if (FF_L1_FAST_DECODING == 1) */

      // Update AFC: Call AFC control function (KALMAN filter).
      #if AFC_ALGO
       {
         WORD16 old_afc  = l1s.afc;
         WORD16 old_count= l1s.afc_frame_count;

         scell_radio_freq = l1a_l1s_com.Scell_info.radio_freq;
         #if (VCXO_ALGO==0)
         l1s.afc = l1ctl_afc(AFC_CLOSED_LOOP, &l1s.afc_frame_count, (WORD16)angle, snr, scell_radio_freq);
         #else
           l1s.afc = l1ctl_afc(AFC_CLOSED_LOOP, &l1s.afc_frame_count, (WORD16)angle, snr, scell_radio_freq,l1a_l1s_com.mode);
         #endif
         #if L2_L3_SIMUL
           #if (DEBUG_TRACE == BUFFER_TRACE_AFC_OPEN)
             buffer_trace (4,(WORD16)angle,old_count,old_afc,l1s.afc);
           #endif
         #endif
       }
      #endif

      // Feed TOA histogram only when the TOA result is used in the task CTRL function
      if (task != PBCCHS)
      {
        //Feed TOA histogram.
        #if (TOA_ALGO != 0)
          #if (TOA_ALGO == 2)
            if(l1s.toa_var.toa_snr_mask == 0)
          #else
            if(l1s.toa_snr_mask == 0)
          #endif
          {
            UWORD32 snr_temp;
            snr_temp = (l1a_l1s_com.Scell_IL_for_rxlev < IL_FOR_RXLEV_SNR) ? snr: 0;
            #if (TOA_ALGO == 2)
              l1s.toa_var.toa_shift = l1ctl_toa(TOA_RUN, l1a_l1s_com.mode, snr_temp, toa);
            #else
              l1s.toa_shift = l1ctl_toa(TOA_RUN, l1a_l1s_com.mode, snr_temp, toa, &l1s.toa_update, &l1s.toa_period_count
#if (FF_L1_FAST_DECODING == 1)
              ,0
#endif
	      );
            #endif
          }
        #endif
      }
    }

    #if (TRACE_TYPE == 1) || (TRACE_TYPE == 4)
      RTTL1_FILL_DL_BURST(angle, snr, l1s.afc, task, pm, toa, l1a_l1s_com.Scell_IL_for_rxlev + l1a_l1s_com.Scell_info.pb)
    #endif
    #if (TRACE_TYPE == 1) || (TRACE_TYPE == 4)
      l1_trace_burst_param(angle, snr, l1s.afc, task, pm, toa, l1a_l1s_com.Scell_IL_for_rxlev + l1a_l1s_com.Scell_info.pb);
    #endif
    #if (BURST_PARAM_LOG_ENABLE == 1)
      l1_log_burst_param(angle, snr, l1s.afc, task, pm, toa, l1a_l1s_com.Scell_IL_for_rxlev + l1a_l1s_com.Scell_info.pb);
    #endif
    // compute the Data bloc Power.
    // ******************************
    if(burst_id == BURST_1)
      pwr_level = 0;

    // add the burst power
    pwr_level += l1a_l1s_com.Scell_IL_for_rxlev;


    // Read downlink DATA block from MCU/DSP interface.
    // *************************************************
   #if (FF_L1_FAST_DECODING == 1)
    /* Perform the reporting if
        - Burst is the 4th one (whether CRC is ok or not)
        - Fast decoding enabled and CRC already ok
    */
    if ( (burst_id == BURST_4) || fast_decoded )
    #else /* #if (FF_L1_FAST_DECODING == 1) */
    if(burst_id == BURST_4)
    #endif /* FF_L1_FAST_DECODING */
    {
      #if (TRACE_TYPE==2 ) || (TRACE_TYPE==3)
        uart_trace(task);
      #endif
#if (FF_L1_FAST_DECODING == 1)
      /* Data power block = pwr_level / (nb of bursts)*/
      pwr_level = pwr_level / (burst_id + 1);
#else /* #if (FF_L1_FAST_DECODING == 1) */
      // the data power bloc = pwr_level/4.
      pwr_level = pwr_level >> 2;
#endif /* #if (FF_L1_FAST_DECODING == 1) #else*/

      // Read L3 frame block and send msg to L1A.
#if (FF_L1_FAST_DECODING == 1)
      if(!fast_decoding_authorized)
      {
        /* When fast decoding wasn't used, burst_id is undefined (for the trace) */
        l1a_l1s_com.last_fast_decoding = 0;
      }
      else
      {
        l1a_l1s_com.last_fast_decoding = burst_id + 1;
      }
#endif /* #if (FF_L1_FAST_DECODING == 1) */

      // Read L3 frame block and send msg to L1A.
      if (l1a_l1s_com.dsp_scheduler_mode == GSM_SCHEDULER)
        l1s_read_l3frm(pwr_level,&(l1s_dsp_com.dsp_ndb_ptr->a_cd[0]), task);
      else
        l1s_read_l3frm(pwr_level,&(l1ps_dsp_com.pdsp_ndb_ptr->a_dd_gprs[0][0]), task);

    } // End if...

  } //end of test "if((en_task) && !(task_param))"

  // Disactivate task.
  // ******************

  // End of task -> task must become INACTIVE.
  // Rem: some TASKS (PALLC, PNP (with SPLIT > M)) can be pipelined and therefore
  // must stay active if they have already reentered the flow.
#if (FF_L1_FAST_DECODING == 1)
    /*------------------------------------------------------*/
    /* Perform the reporting if                             */
    /* - Burst is the 4th one (whether CRC is ok or not)    */
    /* - Fast decoding enabled and CRC already ok           */
    /*------------------------------------------------------*/
  if ( (burst_id == BURST_4) || fast_decoded )
#else /* #if (FF_L1_FAST_DECODING == 1) */
  if(burst_id == BURST_4)
#endif /* #if (FF_L1_FAST_DECODING == 1) #else*/
  {
#if (FF_L1_FAST_DECODING == 1)
    if(task == PNP)
    {
      if (l1a_apihisr_com.fast_decoding.contiguous_decoding == TRUE)
      {
        /* A new block has started, a new fast API IT is expected */
        l1a_apihisr_com.fast_decoding.contiguous_decoding = FALSE;
        l1a_apihisr_com.fast_decoding.status = C_FAST_DECODING_AWAITED;
      }
      else if(task == l1a_apihisr_com.fast_decoding.task)
      {
        /* Reset decoding status */
        l1a_apihisr_com.fast_decoding.status = C_FAST_DECODING_NONE;
      }
    } /*task == PNP */
#endif /* #if (FF_L1_FAST_DECODING == 1) */    
    if(l1s.task_status[task].current_status == RE_ENTERED)
      l1s.task_status[task].current_status = ACTIVE;
    else
      l1s.task_status[task].current_status = INACTIVE;
#if (FF_L1_FAST_DECODING == 1)
 if (burst_id != BURST_4)
    {
     l1s_clean_mftab(task, burst_id + 3);
     if(l1s.frame_count == (4 -burst_id))
     {
          l1s.frame_count = 1;
     }
    }
#endif /* #if (FF_L1_FAST_DECODING == 1) */
  }

  l1ddsp_read_iq_dump(task);
  // Flag the use of the MCU/DSP dual page read interface.
  // ******************************************************

  // Set flag used to change the read page at the end of "l1_synch".
  l1s_dsp_com.dsp_r_page_used = TRUE;

} // end of procedure

//#pragma GSM_IDLE_DUPLICATE_FOR_INTERNAL_RAM_END         // KEEP IN EXTERNAL MEM otherwise
#endif
/*-------------------------------------------------------*/
/* l1ps_ctrl_pbcch()                                    */
/*-------------------------------------------------------*/
/*                                                       */
/* Description:                                          */
/* ------------                                          */
/* This function is a "COMPLEX" function used by the L1S */
/* tasks: neighbor cell PBCCH.                           */
/* This function is the control function                 */
/* for reading a PBCCH burst on the neighbor cell.       */
/* This control function:                                */
/* a) shifts the OFFSET register to match the normal     */
/*    burst received task with the PBCCH timeslot number.*/
/*                                                       */
/* b) programs a normal burst reading and restores the   */
/*    OFFSET to the serving cell timeslot. On the last   */
/*    control (4th burst), the SYNCHRO/OFFSET registers  */
/*    are shifted back to the normal idle mode PCCH      */
/*    reading setting. Here is a summary of the          */
/*    execution:                                         */
/*                                                       */
/*  - If SEMAPHORE(task) is low.                         */
/*    - Traces and debug.                                */
/*    - Programs DSP for PBCCH task, reading 1 burst.    */
/*    - Programs TPU for PBCCH task, reading 1 burst.    */
/*    - Shift TPU SYNCHRO/OFFSET registers back to the   */
/*      PACKET PAGING TASK timeslot.                     */
/*  - Flag DSP and TPU programmation.                    */
/*                                                       */
/* Input parameters:                                     */
/* -----------------                                     */
/* "task"                                                */
/*   PBCCH_TRA or PBCCH_IDLE or PBCCHS                   */
/*    Serving Cell PBCCH reading task.                   */
/*                                                       */
/* Input parameters from globals:                        */
/* ------------------------------                        */
/* "l1pa_l1ps_com.pbcch "                                */
/*   Neigh/serv Cell PBCCH description structure.        */
/*                                                       */
/* "l1a_l1s_com.Scell_info.radio_freq"                   */
/*   BSIC of the serving cell. It is used here to pass   */
/*   the training sequence number (part of BSIC) to the  */
/*   DSP.                                                */
/*                                                       */
/* "l1a_l1s_com.offset_tn0"                              */
/*   value to load in the OFFSET register to shift then  */
/*   any receive task to the timeslot 0 of the neighbor  */
/*   cell or PBCCH timeslot number .                     */
/*                                                       */
/* "l1s.tpu_offset"                                      */
/*   value for the TPU SYNCHRO and OFFSET registers      */
/*   for current serving cell setting. It is used here   */
/*   at the end of the PBCCH  task controls to restore   */
/*   the SYNCHRO/OFFSET registers to the normal setting  */
/*   in idle mode.                                       */
/*                                                       */
/* Modified parameters from globals:                     */
/* ---------------------------------                     */
/* "l1s.actual_time, l1s.next_time"                      */
/*   frame number and derived numbers for current frame  */
/*   and next frame.                                     */
/*   -> update to cope with side effect due to synchro.  */
/*      changes/restores.                                */
/*                                                       */
/* "l1s.tpu_ctrl_reg"                                    */
/*   bit register used to know at the end of L1S if      */
/*   something has been programmed on the MCU/TPU com.   */
/*   This is used mainly to swap then the com. page at   */
/*   the end of a control frame.                         */
/*   -> set CTRL_RX bit in the register.                 */
/*                                                       */
/* "l1s.dsp_ctrl_reg"                                    */
/*   bit register used to know at the end of L1S if      */
/*   something has been programmed on the MCU/DSP com.   */
/*   This is used mainly to swap then the com. page at   */
/*   the end of a control frame.                         */
/*   -> set CTRL_RX bit in the register.                 */
/*                                                       */
/*-------------------------------------------------------*/
void l1ps_ctrl_pbcch(UWORD8 task, UWORD8 burst_id)
{
  UWORD16  rx_radio_freq;
  UWORD32  offset_pbcch;
  WORD8    agc;
  UWORD8   lna_off;
  UWORD32  dsp_task;
  UWORD8   tsc;
  UWORD8   serving_cell;
#if (RF_FAM == 61)
     UWORD16 dco_algo_ctl_nb=0;
     UWORD8 if_ctl = 0;
	 UWORD8 if_threshold = C_IF_ZERO_LOW_THRESHOLD_GPRS;
#endif
  // By default we choose the hardware filter
  UWORD8 csf_filter_choice = L1_SAIC_HARDWARE_FILTER;
#if (NEW_SNR_THRESHOLD == 1)
  UWORD8 saic_flag=0;
#endif /* NEW_SNR_THRESHOLD */
  static   WORD32 new_tpu_offset;
  static   BOOL   change_synchro;

  #define  PbcchS  l1pa_l1ps_com.pbcchs
  #define  PbcchN  l1pa_l1ps_com.pbcchn

  #if (CODE_VERSION == SIMULATION)
    UWORD32 tpu_w_page;

    if (hw.tpu_r_page==0)
     tpu_w_page=1;
    else
     tpu_w_page=0;

    hw.rx_id[tpu_w_page][0]=0;
    hw.num_rx[tpu_w_page][0]=1;
    hw.rx_group_id[tpu_w_page]=1;
  #endif

  if (task == PBCCHS)
  {
    tsc            = PbcchS.packet_chn_desc.tsc;
    offset_pbcch   = (PbcchS.tn_pbcch * TN_WIDTH);
    serving_cell   = TRUE;
  }
  else
  {
    tsc            = PbcchN.packet_chn_desc.tsc;
    offset_pbcch   = PbcchN.time_alignmt;
    serving_cell   = FALSE;
  }

  if((l1a_l1s_com.l1s_en_task[task] == TASK_ENABLED) &&
    !(l1a_l1s_com.task_param[task] == SEMAPHORE_SET))
  // Check the task semaphore. The control body is executed only
  // when the task semaphore is 0. This semaphore can be set to
  // 1 whenever L1A makes some changes to the task parameters.
  {
    // Get ARFCN to be used for current control. Output of the hopping algorithm.
    rx_radio_freq    = l1pa_l1ps_com.p_idle_param.radio_freq;

    // Traces and debug.
    // ******************

    #if (TRACE_TYPE==5) && FLOWCHART
      trace_flowchart_dsp_tpu(dltsk_trace[task].name);
    #endif

    #if (TRACE_TYPE!=0)
      if (task == PBCCHS)
        trace_fct(CST_L1PS_CTRL_PBCCHS, l1a_l1s_com.Scell_info.radio_freq);
      else
        trace_fct(CST_L1PS_CTRL_PBCCHN, PbcchN.bcch_carrier);
    #endif

    l1s_dsp_com.dsp_db_w_ptr->d_debug = (l1s.debug_time + 2) ;

    // Programs DSP for PBCCHN task according to the DSP scheduler used
    // *****************************************************************
    switch(l1a_l1s_com.dsp_scheduler_mode)
    {
      // dsp pgm is made using GSM scheduler...
      case GSM_SCHEDULER:
        dsp_task = l1s_swap_iq_dl(rx_radio_freq, task);

        // dsp pgm...
        l1ddsp_load_rx_task(dsp_task,burst_id,tsc);
      break;

      // dsp pgm is made using GPRS scheduler...
      case GPRS_SCHEDULER:
          #if FF_L1_IT_DSP_USF
             l1pddsp_idle_rx_nb(burst_id,tsc,rx_radio_freq,0,FALSE,FALSE);
          #else
        l1pddsp_idle_rx_nb(burst_id,tsc,rx_radio_freq,0,FALSE);
          #endif
      break;
    }

    // Check if "Synchro" change is needed.
    // *************************************
    // If so the synchro is changed by 4 timeslots.
    if(burst_id == BURST_1)
    {
      if (task == PBCCHS)
        change_synchro = PbcchS.change_synchro;
      else
        change_synchro = PbcchN.change_synchro;

      if(change_synchro)
      {
        // compute TPU offset for "current timeslot + 4 timeslot"
        new_tpu_offset = l1s.tpu_offset + (4 * TN_WIDTH);

        if(new_tpu_offset >= TPU_CLOCK_RANGE)
          new_tpu_offset -= TPU_CLOCK_RANGE;

        // Slide synchro to match current timeslot + 4 timeslot.
        l1dmacro_synchro(SWITCH_TIME, new_tpu_offset);
      }
      else
      {
        new_tpu_offset = l1s.tpu_offset;
      }
    }

    // TPU pgm...
    //-----------
    offset_pbcch += new_tpu_offset;
    if (offset_pbcch >= TPU_CLOCK_RANGE)
      offset_pbcch -= TPU_CLOCK_RANGE;

    // add for debug TPU simu
    #if (CODE_VERSION == SIMULATION)
    if (task == PBCCHS) // PBCCH serving, compute Ts related to the L1 synchro on the serving
      hw.rx_id[tpu_w_page][0]=((TPU_CLOCK_RANGE-new_tpu_offset+offset_pbcch)%TPU_CLOCK_RANGE)/TN_WIDTH;
    else // PBCCH Neighbor -> special value for PBCCHN detection in the DSP task
      hw.rx_id[tpu_w_page][0]=10;
    #endif

    // agc is set with the input_level computed from PAGC algo
    l1pctl_pagc_ctrl(&agc, &lna_off, rx_radio_freq, serving_cell);

#if(RF_FAM == 61)   // Locosto DCO
    cust_get_if_dco_ctl_algo(&dco_algo_ctl_nb, &if_ctl, (UWORD8) L1_IL_VALID ,
                                             l1a_l1s_com.Scell_used_IL.input_level  , rx_radio_freq, if_threshold);
    l1ddsp_load_dco_ctl_algo_nb(dco_algo_ctl_nb);
#endif

    #if (L1_SAIC != 0)
      // If SAIC is enabled, call the low level SAIC control function
      // NOTE: l1a_l1s_com.Scell_used_IL.input_level is updated within
      //       l1pctl_pagc_ctrl
      if(task == PBCCHS)
      {
        // Call SAIC only for PBCCHS, not for PBCCHN_TRAN or PBCCHN_IDLE
        csf_filter_choice = l1ctl_saic(l1a_l1s_com.Scell_used_IL.input_level,l1a_l1s_com.mode
        #if (NEW_SNR_THRESHOLD == 1)
            ,task
            ,&saic_flag
        #endif
        );
      }
#endif

    l1dmacro_offset  (offset_pbcch, l1_config.params.rx_change_offset_time);  // Slide offset to cope with PBCCHN in the new sychro.
    l1dmacro_rx_synth(rx_radio_freq);                                         // load SYNTH.
    l1dmacro_agc     (rx_radio_freq,agc, lna_off
                                #if(RF_FAM == 61)
                                  ,if_ctl
				     #endif
	                         );                            // load AGC.
#if (L1_MADC_ON == 1)
#if (RF_FAM == 61)
    l1dmacro_rx_nb   (rx_radio_freq,INACTIVE, csf_filter_choice
#if (NEW_SNR_THRESHOLD == 1)
    ,saic_flag
#endif /* NEW_SNR_THRESHOLD */
        );   // RX window for NB.
#endif /* RF_FAM == 61*/
#else  /* L1_MADC_ON == 1*/
    l1dmacro_rx_nb   (rx_radio_freq, csf_filter_choice);            // RX window for NB.
#endif

    if (task == PBCCHS)
    {
      #if ((ANLG_FAM == 1) || (ANLG_FAM == 2) || (ANLG_FAM == 3))
          l1ddsp_load_afc(l1s.afc);  // Loading the afc value in DB,Flag the presence of a new afc value to send
      #endif
	  #if (RF_FAM == 61)
          l1dtpu_load_afc(l1s.afc);
      #endif
    }
    l1dmacro_offset  (new_tpu_offset, IMM); // Restore offset.
  } // End if(task enabled and semaphore false)

  // Remark:
  //--------
  // When the task is aborted, we must continue to make dummy
  // DSP programming to avoid communication mismatch due
  // to C/W/R pipelining.

  // We must also ensure the Synchro back since synchro change has surely be done
  // in the 1st CTRL phase.

  // Shift TPU SYNCHRO/OFFSET registers back to the default timeslot (normally PCCCH one).
  // **************************************************************************************
  // When the PBCCHN or PBCCHS reading control is completed ,
  // the SYNCHRO/OFFSET registers are shifted back to the normal idle
  // setting used for PCCH reading on the serving cell.
  // Check if "Synchro" change was needed.
  // If so the synchro is changed to recover normal synchro.
  if(burst_id == BURST_4)
  {
    if(change_synchro)
    {
      // Slide synchro back to mach current serving timeslot.
      l1dmacro_synchro(SWITCH_TIME, l1s.tpu_offset);

      // Increment frame number.
      l1s.actual_time    = l1s.next_time;
      l1s.next_time      = l1s.next_plus_time;
      l1s_increment_time(&(l1s.next_plus_time), 1);  // Increment "next_plus time".

      l1s.tpu_ctrl_reg |= CTRL_SYCB;
      l1s.dsp_ctrl_reg |= CTRL_SYNC;

      #if (TRACE_TYPE == 1) || (TRACE_TYPE == 4)
        trace_fct(CST_L1S_ADJUST_TIME, 1); //OMAPS00090550
      #endif
    }
  }

  // Flag DSP and TPU programmation.
  // ********************************

  // Set "CTRL_RX" flag in the controle flag register.
  l1s.tpu_ctrl_reg |= CTRL_RX;
  l1s.dsp_ctrl_reg |= CTRL_RX;

  // This task is not compatible with Neigh. Measurement. Store task length
  // in "forbid_meas" to indicate when the task will last.
  if((burst_id == BURST_1) && (task != PBCCHN_IDLE))
  {
    // In PBCCHN_IDLE task, l1s.forbid_meas is set by the AGC ctrl
    l1s.forbid_meas = TASK_ROM_MFTAB[task].size;
  }
}

#if (MOVE_IN_INTERNAL_RAM == 0) // Must be followed by the pragma used to duplicate the funtion in internal RAM
//#pragma DUPLICATE_FOR_INTERNAL_RAM_START

/*-------------------------------------------------------*/
/* l1ps_ctrl_ptcch()                                     */
/*-------------------------------------------------------*/
/*                                                       */
/* Description:                                          */
/* ------------                                          */
/*                                                       */
/* Input parameters:                                     */
/* -----------------                                     */
/*                                                       */
/* Input parameters from globals:                        */
/* ------------------------------                        */
/*                                                       */
/* Modified parameters from globals:                     */
/* ---------------------------------                     */
/*                                                       */
/*-------------------------------------------------------*/
void l1ps_ctrl_ptcch(UWORD8 param1, UWORD8 param2)
{
  UWORD16  radio_freq;
  UWORD8   burst_nb;
  #if (RF_FAM == 61)
    UWORD16 dco_algo_ctl_nb = 0;
    UWORD8 if_ctl = 0;
	UWORD8 if_threshold = C_IF_ZERO_LOW_THRESHOLD_GPRS;
  #endif
  #if (NEW_SNR_THRESHOLD == 1)
    UWORD8 saic_flag=0;
  #endif /* NEW_SNR_THRESHOLD*/

  if(!(l1a_l1s_com.task_param[PTCCH] == SEMAPHORE_SET))
  // Check the task semaphore. The control body is executed only
  // when the task semaphore is 0. This semaphore can be set to
  // 1 whenever L1A makes some changes to the task parameters.
  {
    WORD8  ts;

    radio_freq = l1pa_l1ps_com.transfer.ptcch.radio_freq;

    // Traces and debug.
    // ******************
    #if (TRACE_TYPE!=0)
      if(l1pa_l1ps_com.transfer.ptcch.activity && (PTCCH_DL || PTCCH_UL ) == 0) // trace only if a window is programmed.
        trace_fct(CST_L1PS_CTRL_PTCCH_EMPTY, radio_freq);
    #endif

    #if (TRACE_TYPE==5) && FLOWCHART
      trace_flowchart_dsp_tpu(dltsk_trace[PTCCH].name);
    #endif

    l1s_dsp_com.dsp_db_w_ptr->d_debug = (l1s.debug_time + 2) ;

    switch (l1s.next_time.fn_mod104)
    {
      case 12: burst_nb=0; break;
      case 38: burst_nb=1; break;
      case 64: burst_nb=2; break;
      case 90: burst_nb=3; break;
      default: burst_nb=0; break;
    }

    // Compute timeslot number referenced to current camp timeslot.
    ts = l1pa_l1ps_com.transfer.aset->packet_ta.ta_tn - l1a_l1s_com.dl_tn;
    if(ts < 0) ts += 8;
    else if(ts >= 8) ts -= 8;

    if(l1pa_l1ps_com.transfer.ptcch.activity & PTCCH_DL)
    // PTCCH DL activity bit is set: PTCCH DL programmation is required.
    {
      #if (TRACE_TYPE!=0)
            trace_fct(CST_L1PS_CTRL_PTCCH_DL_BURST0 + burst_nb, radio_freq);
      #endif

      // Programs DSP for PTCCH/DL.
      // ***************************
      {
      #if FF_L1_IT_DSP_USF
        l1pddsp_idle_rx_nb(burst_nb,
                           l1pa_l1ps_com.transfer.aset->tsc,
                           radio_freq,
                           ts,
                           TRUE,
                           FALSE);
      #else
        l1pddsp_idle_rx_nb(burst_nb,
                           l1pa_l1ps_com.transfer.aset->tsc,
                           radio_freq,
                           ts,
                           TRUE);
      #endif
      }


      // Programs TPU for PTCCH/DL task.
      // ********************************
      {
        WORD8  agc;
        UWORD8  lna_off;

        // AGC updating
        //-------------
        l1pctl_pagc_ctrl(&agc, &lna_off, radio_freq,TRUE);

      #if(RF_FAM == 61)   // Locosto DCO
       cust_get_if_dco_ctl_algo(&dco_algo_ctl_nb, &if_ctl, (UWORD8) L1_IL_VALID,
                                             l1a_l1s_com.Scell_used_IL.input_level  , radio_freq, if_threshold);
      	l1ddsp_load_dco_ctl_algo_nb(dco_algo_ctl_nb);
      #endif

     #if (L1_SAIC != 0)
          // If SAIC is enabled, call the low level SAIC control function
          // NOTE: l1a_l1s_com.Scell_used_IL.input_level is updated within
          //       the function l1pctl_pagc_ctrl
          l1ctl_saic(l1a_l1s_com.Scell_used_IL.input_level,l1a_l1s_com.mode
          #if (NEW_SNR_THRESHOLD == 1)
              ,PTCCH
              ,&saic_flag
          #endif
          );
      #endif

        // Compute timeslot number referenced to current camp timeslot.
        // Rem: COULD BE DONE ASYNCHRONOUSLY changing ta_tn definition!!!
        ts = l1pa_l1ps_com.transfer.aset->packet_ta.ta_tn - l1a_l1s_com.dl_tn;
        if(ts < 0) ts += 8;
        else if(ts >= 8) ts -= 8;

        // Program RX Normal Burst scenario.
        l1pdtpu_serv_rx_nb(radio_freq,
                           agc,
                           lna_off,
                           ts,
                           l1s.tpu_offset,
                           1,
                           1,
                           TRUE,INACTIVE,
                           L1_SAIC_HARDWARE_FILTER
                            #if(RF_FAM == 61)
                            ,if_ctl
                            #endif
                            #if (NEW_SNR_THRESHOLD == 1)
                            ,saic_flag
                            #endif /* NEW_SNR_THRESHOLD */
    		                                );

        // Set "CTRL_RX" flag in the controle flag registers.
        l1s.tpu_ctrl_reg |= CTRL_RX;
        l1s.dsp_ctrl_reg |= CTRL_RX;
      }
    }

    if(l1pa_l1ps_com.transfer.ptcch.activity & PTCCH_UL)
    // PTCCH UL activity bit is set: PTCCH UL programmation required.
    {
      UWORD8 adc_active = INACTIVE;

      #if (TRACE_TYPE!=0)
          trace_fct(CST_L1PS_CTRL_PTCCH_UL, radio_freq);
      #endif

      // Programs DSP for PTCCH/UL.
      // ***************************
      {
        UWORD8  cs_type;
        UWORD16 ptcch_ul_data;

        // Access burst type ?
        if (l1pa_l1ps_com.access_burst_type == ACC_BURST_8)
        {
          // PRACH 8 bits: data = (0111 1111)b
          cs_type       = CS_PAB8_TYPE;
          ptcch_ul_data = 0x7F;
        }
        else
        {
          // PRACH 11 bits: data = (111 1111 1111)b
          cs_type       = CS_PAB11_TYPE;
          ptcch_ul_data = 0x7FF;
        }

        // "As" IDLE POLLING PRACH dsp control.
        l1pddsp_ul_ptcch_data(cs_type,
                              ptcch_ul_data,
                              l1a_l1s_com.Scell_info.bsic,
                              radio_freq,
                              ts+3);

        l1pddsp_idle_prach_power(l1s.applied_txpwr,
                                 radio_freq,
                                 ts+3);
      }

      // ADC measurement
      // ***************
      {
        // check if during the SACCH burst an ADC measurement must be performed
         if (l1a_l1s_com.adc_mode & ADC_NEXT_TRAFFIC_UL)  // perform ADC only one time
         {
            adc_active = ACTIVE;
            l1a_l1s_com.adc_mode &= ADC_MASK_RESET_TRAFFIC; // reset in order to have only one ADC measurement in Traffic
         }
         else
           if (l1a_l1s_com.adc_mode & ADC_EACH_TRAFFIC_UL) // perform ADC on each period bloc
               if ((++l1a_l1s_com.adc_cpt)>=l1a_l1s_com.adc_traffic_period) // wait for the period
               {
                 adc_active = ACTIVE;
                 l1a_l1s_com.adc_cpt = 0;
               }
      }
      // Programs TPU for PTCCH/UL task.
      // ********************************
      {
        // Program TX RA scenario.
        l1pdtpu_serv_tx(radio_freq,
                        0,               // TA=0.
                        l1s.tpu_offset,
                        ts+3,            // tx_id.
                        1,               // 1 PRACH.
                        1,               // tx_group_id.
                        0,               // No switch NB->RA
                        1,               // Driver called for PRACH Burst.
                        l1pa_l1ps_com.transfer.ptcch.activity & PTCCH_DL,adc_active);
                                         // Flag RX in same frame as TX
      }

      // PTCCH/UL has been executed,
      //  -> PTCCH/DL is then requested for schedule.
      //  -> PTCCH/UL activity flag must be reset.
      l1pa_l1ps_com.transfer.ptcch.request_dl  = TRUE;
      l1pa_l1ps_com.transfer.ptcch.activity   ^= PTCCH_UL;

      // Set "CTRL_TX" flag in the controle flag register.
      l1s.tpu_ctrl_reg |= CTRL_TX;
      l1s.dsp_ctrl_reg |= CTRL_TX;

      #if (TRACE_TYPE == 1) || (TRACE_TYPE == 4)
        RTTL1_FILL_UL_AB(PTCCH,l1s.applied_txpwr)
      #endif

    } // End of PTCCH UL programmation
  } // End of if(...semaphore...)

  // This task is not compatible with Neigh. Measurement. Store task length
  // in "forbid_meas" to indicate when the task will last.
  l1s.forbid_meas = TASK_ROM_MFTAB[PTCCH].size;
}
//#pragma DUPLICATE_FOR_INTERNAL_RAM_END
#endif // MOVE_IN_INTERNAL_RAM


#if (MOVE_IN_INTERNAL_RAM == 0) // Must be followed by the pragma used to duplicate the funtion in internal RAM
//#pragma DUPLICATE_FOR_INTERNAL_RAM_START

/*-------------------------------------------------------*/
/* l1s_read_ptcch()                                      */
/*-------------------------------------------------------*/
/*                                                       */
/* Description:                                          */
/* ------------                                          */
/*                                                       */
/* Input parameters:                                     */
/* -----------------                                     */
/*                                                       */
/* Input parameters from globals:                        */
/* ------------------------------                        */
/*                                                       */
/* Modified parameters from globals:                     */
/* ---------------------------------                     */
/*                                                       */
/*-------------------------------------------------------*/
void l1ps_read_ptcch(UWORD8 param1, UWORD8 param2)
{
  // Traces and debug.
  // ******************

  l1_check_com_mismatch(PTCCH);

  if(l1pa_l1ps_com.transfer.ptcch.activity & PTCCH_DL)
  // PTCCH/DL has been executed,
  {
    UWORD32   pm;
    WORD8     ts;

    // Compute timeslot number referenced to current camp timeslot.
    ts = l1pa_l1ps_com.transfer.aset->packet_ta.ta_tn - l1a_l1s_com.dl_tn;
    if(ts < 0) ts += 8;
    else if(ts >= 8) ts -= 8;

    #if (TRACE_TYPE!=0)
           trace_fct(CST_L1PS_READ_PTCCH_DL, l1pa_l1ps_com.transfer.ptcch.radio_freq);
    #endif

    // Read control results and feed control algorithms.
    // **************************************************

    // Read control information.
    pm = (l1ps_dsp_com.pdsp_db_r_ptr->a_burst_pm_gprs[ts] & 0xffff);
    l1_check_pm_error(pm,PTCCH);
    pm = pm >> 5;

    if(l1s.actual_time.fn_mod104 == 91)
    // Read PTCCH/DL data block from DSP/MCU interface, a_dd_md_gprs[].
    {
      BOOL   crc;
      UWORD8 ordered_ta =0 ;//omaps00090550

      crc = (l1ps_dsp_com.pdsp_ndb_ptr->a_dd_md_gprs[0] & 0x0100) >> 8;

      if(!crc)
      // Block correct, we extract new TA...
      {
        UWORD8 word_position = 4+ (l1pa_l1ps_com.transfer.aset->packet_ta.ta_index >> 1);
        UWORD8 byte_position = l1pa_l1ps_com.transfer.aset->packet_ta.ta_index & 0x01;

        // Download ordered TA...
        // IF byte_position
        //   Upper byte contains TA...
        // ELSE
        //   Lower byte contains TA...
        // (see GSM04.04)

        ordered_ta = (l1ps_dsp_com.pdsp_ndb_ptr->a_dd_md_gprs[word_position] >> (8*byte_position)) & 0x7f;

        if (ordered_ta < 64)
        {
          // PTCCH/DL contains a valid TA for MS: update TA
          l1pa_l1ps_com.transfer.aset->packet_ta.ta = ordered_ta;

          // PTCCH/DL activity bit must reset when new TA has been successfully received.
          l1pa_l1ps_com.transfer.ptcch.activity ^= PTCCH_DL;
        }
      }

      #if ((TRACE_TYPE == 1) || (TRACE_TYPE == 4))
        if (trace_info.current_config->l1_dyn_trace & 1<<L1_DYN_TRACE_DL_PTCCH)
        // whatever the value is, trace it
        {
            Trace_dl_ptcch(ordered_ta,
                           crc,
                           l1pa_l1ps_com.transfer.aset->packet_ta.ta_index,
                           l1pa_l1ps_com.transfer.aset->packet_ta.ta_tn,
                           l1ps_dsp_com.pdsp_ndb_ptr->a_dd_md_gprs[4],
                           l1ps_dsp_com.pdsp_ndb_ptr->a_dd_md_gprs[5],
                           l1ps_dsp_com.pdsp_ndb_ptr->a_dd_md_gprs[6],
                           l1ps_dsp_com.pdsp_ndb_ptr->a_dd_md_gprs[7],
                           l1ps_dsp_com.pdsp_ndb_ptr->a_dd_md_gprs[8],
                           l1ps_dsp_com.pdsp_ndb_ptr->a_dd_md_gprs[9],
                           l1ps_dsp_com.pdsp_ndb_ptr->a_dd_md_gprs[10],
                           l1ps_dsp_com.pdsp_ndb_ptr->a_dd_md_gprs[11]);
        }
      #endif

      #if (TRACE_TYPE == 1) || (TRACE_TYPE == 4)
        RTTL1_FILL_DL_PTCCH(crc, ordered_ta) // Replace with ordered TA
      #endif

      // Reset CS type.
      l1ps_dsp_com.pdsp_ndb_ptr->a_dd_md_gprs[0] = CS_NONE_TYPE;
    }
  }
  else
  {
     #if (TRACE_TYPE!=0)
       trace_fct(CST_L1PS_READ_PTCCH_UL, l1pa_l1ps_com.transfer.ptcch.radio_freq);
     #endif
  }

  // Set flag used to change the read page at the end of "l1_synch".
  l1s_dsp_com.dsp_r_page_used = TRUE;

  // End of task -> task must become INACTIVE.
  l1s.task_status[PTCCH].current_status = INACTIVE;
}
//#pragma DUPLICATE_FOR_INTERNAL_RAM_END
#endif // MOVE_IN_INTERNAL_RAM


#if (MOVE_IN_INTERNAL_RAM == 0) // Must be followed by the pragma used to duplicate the funtion in internal RAM
//#pragma DUPLICATE_FOR_INTERNAL_RAM_START

/*-------------------------------------------------------*/
/* l1ps_ctrl_itmeas()                                    */
/*-------------------------------------------------------*/
/*                                                       */
/* Description:                                          */
/* ------------                                          */
/* This function is a "COMPLEX" function used by the L1S */
/* task ITMEAS.                                          */
/* This function is the control function for measuring   */
/* the signal strength on several specified timeslots (on*/
/* which it's possible according to the multi-slot class)*/
/* of an indicated carrier.                              */
/* It programs the DSP and the TPU for doing these       */
/* measurements                                          */
/* Here below is a summary of the execution:             */
/*                                                       */
/*  - If SEMAPHORE(task) is low.                         */
/*      - Traces and debug.                              */
/*      - Determines on which timeslots measurements can */
/*        be done                                        */
/*      - Programs DSP for required task.                */
/*      - Programs TPU for required task.                */
/*  - Flag DSP and TPU programmation.                    */
/*                                                       */
/* Input parameters:                                     */
/* -----------------                                     */
/*                                                       */
/* Input parameters from globals:                        */
/* ------------------------------                        */
/* "l1pa_l1ps_com.itmeas"                                */
/* Interference measurement parameters structure         */
/*                                                       */
/* "l1a_l1s_com.dl_tn"                                   */
/* Timeslot on which L1 is synchronized                  */
/*                                                       */
/* "l1pa_l1ps_com.transfer.aset->multislot_class"        */
/* Multi-slot class in Packet transfer                   */
/*                                                       */
/* Modified parameters from globals:                     */
/* ---------------------------------                     */
/*                                                       */
/* "l1s.tpu_ctrl_reg"                                    */
/*   bit register used to know at the end of L1S if      */
/*   something has been programmed on the MCU/TPU com.   */
/*   This is used mainly to swap then the com. page at   */
/*   the end of a control frame.                         */
/*   -> set CTRL_RX bit in the register.                 */
/*                                                       */
/* "l1s.dsp_ctrl_reg"                                    */
/*   bit register used to know at the end of L1S if      */
/*   something has been programmed on the MCU/DSP com.   */
/*   This is used mainly to swap then the com. page at   */
/*   the end of a control frame.                         */
/*   -> set CTRL_RX bit in the register.                 */
/*-------------------------------------------------------*/
void l1ps_ctrl_itmeas(UWORD8 param1, UWORD8 param2)
{

#if (RF_FAM == 61)
  UWORD16 dco_algo_ctl_pw = 0;
  UWORD8 if_ctl = 0;
  UWORD8 if_threshold = C_IF_ZERO_LOW_THRESHOLD_GPRS;
  UWORD8 ts = 0;
#endif

  // Traces and debug.
  // ******************

  #if (TRACE_TYPE==5)
    trace_fct(CST_L1PS_CTRL_ITMEAS, l1pa_l1ps_com.itmeas.radio_freq);
  #endif

  // Timeslots selection
  // ********************

  // Packet transfer mode <-> PDTCH task enabled: condition to check !!!!
  //---------------------

  // Timeslots already selected in l1p_asyn.c

  // Packet idle mode
  //-----------------
  if (l1a_l1s_com.l1s_en_task[PDTCH] == TASK_DISABLED)
  {
    // If a RX has been programmed on this frame
    if (l1s.dsp_ctrl_reg & CTRL_RX)
    {
      // The pre-processed bitmap with Rx taken into account is taken
      l1pa_l1ps_com.itmeas.meas_bitmap = l1pa_l1ps_com.itmeas.idle_tn_rx;
    }
    else
    {
      // The pre-processed bitmap without Rx taken into account is taken
      l1pa_l1ps_com.itmeas.meas_bitmap = l1pa_l1ps_com.itmeas.idle_tn_no_rx;
    }

  } // End if 'packet idle mode'

  l1pa_l1ps_com.itmeas.dsp_r_page_switch_req = FALSE;

  // If some measurements can be done
  if (l1pa_l1ps_com.itmeas.meas_bitmap != 0)
  {
    UWORD8 nbmeas;

    // DSP read page switched or not during the ITMEAS read phase ?

    if (l1s.dsp_ctrl_reg == NO_CTRL)
    {
      // A control task hasn't already been done in this frame --> Read page switch
      l1pa_l1ps_com.itmeas.dsp_r_page_switch_req = TRUE;
    }

    // Traces and debug.
    // ******************
    #if (TRACE_TYPE!=0) && (TRACE_TYPE!=5)
      trace_fct(CST_L1PS_CTRL_ITMEAS, l1pa_l1ps_com.itmeas.radio_freq);
    #endif


#if(RF_FAM == 61)   // Locosto DCO
    #if (PWMEAS_IF_MODE_FORCE == 0)
        cust_get_if_dco_ctl_algo(&dco_algo_ctl_pw, &if_ctl, (UWORD8) L1_IL_INVALID ,
            0, l1pa_l1ps_com.itmeas.radio_freq, if_threshold);
      #else
        if_ctl = IF_120KHZ_DSP;
        dco_algo_ctl_pw = DCO_IF_0KHZ;
      #endif



#endif

    // Program TPU
    // ************
#if (RF_FAM != 61)
    nbmeas = l1pdtpu_interf_meas(l1pa_l1ps_com.itmeas.radio_freq,
                                 l1_config.params.high_agc,
                                 0,
                                 l1pa_l1ps_com.itmeas.meas_bitmap,
                                 l1s.tpu_offset,
                                 l1s.tpu_win,
                                 l1a_l1s_com.dl_tn);
#endif

#if (RF_FAM == 61)
    nbmeas = l1pdtpu_interf_meas(l1pa_l1ps_com.itmeas.radio_freq,
                                 l1_config.params.high_agc,
                                 0,
                                 l1pa_l1ps_com.itmeas.meas_bitmap,
                                 l1s.tpu_offset,
                                 l1s.tpu_win,
                                 l1a_l1s_com.dl_tn,
                                 if_ctl);
#endif


    // Program DSP
    // ************

    l1pddsp_interf_meas_ctrl(nbmeas);

#if(RF_FAM == 61) // TBD
    // Reproduce the DCO control for all the power measurement
     dco_algo_ctl_pw = dco_algo_ctl_pw * 0x55; // Replicate ZLZLZLZL
     dco_algo_ctl_pw = dco_algo_ctl_pw >> (2*(4 - nbmeas)); // reduce to ZLs of Nbr
     if(l1s.tcr_prog_done==1)
     {
       dco_algo_ctl_pw=((dco_algo_ctl_pw<<2)|(l1s_dsp_com.dsp_db_common_w_ptr->d_dco_algo_ctrl_pw&0x3));
     }
     l1ddsp_load_dco_ctl_algo_pw(dco_algo_ctl_pw);
    // of Meas Programmed
#endif
    // Flag DSP and TPU programmation.
    // ********************************

    // Set "CTRL_RX" flag in the controle flag register.
    l1s.tpu_ctrl_reg |= CTRL_MS;
    l1s.dsp_ctrl_reg |= CTRL_MS;

  } // End if 'nbmeas != 0'

  // This task is not compatible with Neigh. Measurement. Store task length
  // in "forbid_meas" to indicate when the task will last.
  // Rem: Only FB51 task starts from this ctrl function.
  l1s.forbid_meas = TASK_ROM_MFTAB[ITMEAS].size;
}
//#pragma DUPLICATE_FOR_INTERNAL_RAM_END
#endif // MOVE_IN_INTERNAL_RAM


#if (MOVE_IN_INTERNAL_RAM == 0) // Must be followed by the pragma used to duplicate the funtion in internal RAM
//#pragma DUPLICATE_FOR_INTERNAL_RAM_START

/*-------------------------------------------------------*/
/* l1ps_read_itmeas()                                    */
/*-------------------------------------------------------*/
/*                                                       */
/* Description:                                          */
/* ------------                                          */
/*                                                       */
/* This function is a "COMPLEX" function used for the    */
/* L1S ITMEAS task.                                      */
/*                                                       */
/* Here is a summary of the execution:                   */
/*                                                       */
/*  - If SEMAPHORE(task) is low and task still enabled.  */
/*      - Traces and debug.                              */
/*      - Read interference measurement results in NDB   */
/*      - Fill and send reporting message                */
/*  - Disactivate task.                                  */
/*  - Flag the use of the MCU/DSP dual page read         */
/*    interface if needed.                               */
/*                                                       */
/* Input parameters:                                     */
/* -----------------                                     */
/*                                                       */
/* Input parameters from globals:                        */
/* ------------------------------                        */
/* "l1pa_l1ps_com.itmeas"                                */
/* Interference measurement parameters structure         */
/*                                                       */
/* Modified parameters from globals:                     */
/* ---------------------------------                     */
/* "l1s.task_status[task].current_status"                */
/*   current task status. It must be reset (INACTIVE)    */
/*   when the task is completed.                         */
/*   -> disactivate task.                                */
/*                                                       */
/* "l1s_dsp_com.dsp_r_page_used"                         */
/*   Flag used by the function which closes L1S          */
/*   execution ("l1s_end_manager()") to know if the      */
/*   MCU/DSP read page must be switched.                 */
/*   -> Set to 1 only if no other task was controlled    */
/*      in the same frame as ITMEAS                      */
/*                                                       */
/* Use of MCU/DSP interface:                             */
/* -------------------------                             */
/* "l1s_dsp_com.dsp_ndb_ptr"                             */
/*   pointer to the non double buffered part (NDB) of    */
/*   the MCU/DSP interface. This part is R/W for both    */
/*   DSP and MCU.                                        */
/*-------------------------------------------------------*/
void l1ps_read_itmeas(UWORD8 param1, UWORD8 param2)
{
  xSignalHeaderRec *msg;
  UWORD8           i;
  WORD8            delta1_freq, delta2_freq;
  UWORD16          g_magic;

  if(!(l1a_l1s_com.task_param[ITMEAS]) &&
      (l1a_l1s_com.l1s_en_task[ITMEAS]))
  {
    // Traces and debug.
    // ******************

    #if (TRACE_TYPE!=0)
      trace_fct(CST_L1PS_READ_ITMEAS, l1pa_l1ps_com.itmeas.radio_freq);
    #endif

    // Allocate result message.
    // ************************

    msg = os_alloc_sig(sizeof(T_L1P_ITMEAS_IND));
    DEBUGMSG(status,NU_ALLOC_ERR)

    // Fill msg signal code
    msg->SignalCode = L1P_ITMEAS_IND;

    // Fill msg contents
    // ******************

    ((T_L1P_ITMEAS_IND *)(msg->SigP))->fn = l1s.actual_time.fn;
    // Report measurement bitmap
    ((T_L1P_ITMEAS_IND *)(msg->SigP))->meas_bitmap = l1pa_l1ps_com.itmeas.meas_bitmap;

    // Read result from DSP
    g_magic     = l1ctl_get_g_magic(l1pa_l1ps_com.itmeas.radio_freq);
    delta1_freq = l1ctl_encode_delta1(l1pa_l1ps_com.itmeas.radio_freq);
    delta2_freq = l1ctl_encode_delta2(l1pa_l1ps_com.itmeas.radio_freq);

    for (i = 0; i < 8; i++)
    {
      UWORD8 pm;
      WORD16 IL_for_rxlev;

      pm = (l1ps_dsp_com.pdsp_ndb_ptr->a_interf_meas_gprs[i] & 0xffff) >> 5;

      // IL processing
      if (pm == 0)
      {
        ((T_L1P_ITMEAS_IND *)(msg->SigP))->rxlev[i] = (WORD8)0x80;
      }
      else
      {
        IL_for_rxlev = -(pm - (l1_config.params.high_agc << 1) - g_magic) - delta1_freq - delta2_freq;


        ((T_L1P_ITMEAS_IND *)(msg->SigP))->rxlev[i] = l1s_encode_rxlev(IL_for_rxlev);
      }
    }

    // If the Read phase is done during fn_mod26 = 13 --> measurements have been done
    // during a PTCCH frame
    if (l1s.actual_time.t2 == 13)
    {
      ((T_L1P_ITMEAS_IND *)(msg->SigP))->position  = PTCCH_FRAME;
    }
    else  // Measurements done during a search frame
    {
      ((T_L1P_ITMEAS_IND *)(msg->SigP))->position  = SEARCH_FRAME;
    }

    // send message...
    os_send_sig(msg, L1C1_QUEUE);
    DEBUGMSG(status,NU_SEND_QUEUE_ERR)

    // ITMEAS is a 'one shot' task --> disable task
    l1a_l1s_com.l1s_en_task[ITMEAS] = TASK_DISABLED;
  } // End if "task enabled and semaphore false"

  // End of task -> task must become INACTIVE.
  l1s.task_status[ITMEAS].current_status = INACTIVE;

  // Switch DSP read page if needed
  if(l1pa_l1ps_com.itmeas.dsp_r_page_switch_req)
  {
    // Set flag used to change the read page at the end of "l1_synch".
    l1s_dsp_com.dsp_r_page_used = TRUE;
  }
}

//#pragma DUPLICATE_FOR_INTERNAL_RAM_END
#endif // MOVE_IN_INTERNAL_RAM

//#pragma DUPLICATE_FOR_INTERNAL_RAM_START
#endif
//#pragma DUPLICATE_FOR_INTERNAL_RAM_END

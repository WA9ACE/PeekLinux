/************* Revision Controle System Header *************
 *                  GSM Layer 1 software
 * L1P_DRIVE.C
 *
 *        Filename l1p_driv.c
 *  Copyright 2003 (C) Texas Instruments
 *
 ************* Revision Controle System Header *************/

#define L1P_DRIVE_C

#include "l1_macro.h"
#include "l1_confg.h"

#if L1_GPRS
#if (CODE_VERSION == SIMULATION)
  #include <string.h>
  #include "l1_types.h"
  #include "sys_types.h"
  #include "l1_const.h"
  #include "l1_time.h"
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
  #if L2_L3_SIMUL
    #include "hw_debug.h"
  #endif

  #include "l1p_cons.h"
  #include "l1p_msgt.h"
  #include "l1p_deft.h"
  #include "l1p_vare.h"
  #include "l1p_tabs.h"

  #include "sim_cons.h"
  #include "sim_def.h"
  extern T_hw FAR hw;
  #include "l1_proto.h"

#else

  #include <string.h>
  #include "l1_types.h"
  #include "sys_types.h"
  #include "l1_const.h"
  #include "l1_time.h"

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
  #if L2_L3_SIMUL
    #include "hw_debug.h"
  #endif

  #include "l1p_cons.h"
  #include "l1p_msgt.h"
  #include "l1p_deft.h"
  #include "l1p_vare.h"
  #include "l1p_tabs.h"

  #include "l1_proto.h"
  #include "tpudrv.h"

#endif

#if(RF_FAM == 61)
#include "l1_rf61.h"
#include "tpudrv61.h"
#include "l1_ctl.h"
#endif

/*-------------------------------------------------------*/
/* Prototypes of external functions used in this file.   */
/*-------------------------------------------------------*/
void   l1dmacro_synchro  (UWORD32 when, UWORD32 value);
void   l1dmacro_offset   (UWORD32 offset_value, WORD32 relative_time);
void   l1dmacro_afc      (UWORD16 afc_value, UWORD8 win_id);

#if ((ANLG_FAM == 1) || (ANLG_FAM == 2) || (ANLG_FAM == 3) || (RF_FAM == 61) )
  UWORD16 Cust_get_pwr_data(UWORD8 txpwr, UWORD16 radio_freq
  										  #if(REL99 && FF_PRF)
  										  ,UWORD8 number_uplink_timeslot
  										  #endif
  										  );
#endif
void Cust_get_ramp_tab(API *a_ramp, UWORD8 txpwr_ramp_up, UWORD8 txpwr_ramp_down, UWORD16 radio_freq);

BOOL   l1ps_swap_iq_ul   (UWORD16 radio_freq);
BOOL   l1ps_swap_iq_dl   (UWORD16 radio_freq);
#if (L1_MADC_ON == 1)
#if (RF_FAM == 61)
void   l1pdmacro_rx_up   (UWORD16 radio_freq,UWORD8 adc_active, UWORD8 csf_filter_choice
#if (NEW_SNR_THRESHOLD == 1)
      ,UWORD8 saic_flag
#endif /* NEW_SNR_THRESHOLD == 1*/
    );
#endif
#else /* RF_FAM == 61*/
void   l1pdmacro_rx_up   (UWORD16 radio_freq, UWORD8 csf_filter_choice);
#endif
void   l1pdmacro_rx_down (UWORD16 radio_freq, UWORD8 num_rx, BOOL rx_done_flag);
void   l1pdmacro_tx_up   (UWORD16 radio_freq);
void   l1pdmacro_tx_down (UWORD16 radio_freq, WORD16 time, BOOL tx_flag, UWORD8 timing_advance,UWORD8  adc_active);
void   l1pdmacro_tx_synth(UWORD16 radio_freq);
void   l1pdmacro_anchor  (WORD16  time);

void l1dmacro_rx_synth(UWORD16  radio_freq);
void l1dmacro_agc(UWORD16 radio_freq, WORD8 agc_value, UWORD8 lna_off
                     	#if (RF_FAM == 61)
                              ,UWORD8 if_ctl
                     	#endif
	                           );
#if (CODE_VERSION == SIMULATION)
  void   l1dmacro_rx_ms    (UWORD16 arfcn, BOOL rxnb_select);
#else
#if (L1_MADC_ON == 1)
#if (RF_FAM == 61)
 void   l1dmacro_rx_ms    (UWORD16 arfcn,UWORD8 adc_active);
#endif
#else
  void   l1dmacro_rx_ms    (UWORD16 arfcn);
#endif
#endif
void l1pdmacro_it_dsp_gen(WORD16 time);

/*-------------------------------------------------------*/
/* Prototypes of functions defined in this file.         */
/*-------------------------------------------------------*/
// TPU Drivers...


// DSP Drivers...
void l1pddsp_synchro              (UWORD8 switch_mode, UWORD8  camp_timeslot);
void l1pddsp_idle_prach_data      (BOOL    polling, UWORD8  cs_type, UWORD16  channel_request_data,
                                   UWORD8  bsic, UWORD16 radio_freq);
void l1pddsp_idle_prach_power     (UWORD8 txpwr, UWORD16  radio_freq, UWORD8 ts);
void l1pddsp_single_tx_block      (UWORD8 burst_nb, UWORD8 *data, UWORD8  tsc,
                                   UWORD16 radio_freq);
#if FF_L1_IT_DSP_USF
void l1pddsp_idle_rx_nb           (UWORD8 burst_nb, UWORD8 tsq, UWORD16 radio_freq,
                                   UWORD8 timeslot_no, BOOL ptcch_dl, BOOL usf_interrupt);
#else
void l1pddsp_idle_rx_nb           (UWORD8 burst_nb, UWORD8 tsq, UWORD16 radio_freq,
                                   UWORD8 timeslot_no, BOOL ptcch_dl);
#endif
void l1pddsp_transfer_mslot_ctrl  (UWORD8 burst_nb, UWORD8 dl_bitmap, UWORD8  ul_bitmap,
                                   UWORD8 *usf_table, UWORD8 mac_mode, UWORD8 *ul_buffer_index,
                                   UWORD8 tsc, UWORD16 radio_freq, UWORD8 synchro_timeslot,
                                   #if FF_L1_IT_DSP_USF
                                   UWORD8  dsp_usf_interrupt
                                   #else
                                   UWORD8  usf_vote_enable
                                   #endif
				   );
void l1pddsp_transfer_mslot_power (UWORD8 *txpwr, UWORD16 radio_freq, UWORD8 ul_bitmap);
void l1pddsp_ul_ptcch_data        (UWORD8 cs_type, UWORD16  channel_request_data, UWORD8  bsic,
                                   UWORD16 radio_freq, UWORD8 timeslot_no);
void l1pddsp_interf_meas_ctrl     (UWORD8 nb_meas_req);
void l1pddsp_transfer_meas_ctrl   (UWORD8 meas_position);

/*-------------------------------------------------------*/
/* l1pd_afc()                                            */
/*-------------------------------------------------------*/
/* Parameters :                                          */
/* Return     :                                          */
/* Functionality :                                       */
/*-------------------------------------------------------*/
void l1pd_afc(void)
{
  #if ((ANLG_FAM == 1) || (ANLG_FAM == 2) || (ANLG_FAM == 3))
    l1ddsp_load_afc(l1s.afc);
  #endif
  #if (RF_FAM == 61)
      l1dtpu_load_afc(l1s.afc);
  #endif
}

/*-------------------------------------------------------*/
/* l1pdtpu_interf_meas()                                 */
/*-------------------------------------------------------*/
/* Parameters :                                          */
/* Return     :                                          */
/* Functionality :                                       */
/*-------------------------------------------------------*/
UWORD8 l1pdtpu_interf_meas(UWORD16 radio_freq,
                           WORD8  agc,
                           UWORD8  lna_off,
                           UWORD8  meas_bitmap,
                           UWORD32 offset_serv,
                           UWORD16 win_id,
                           UWORD8  synchro_ts
                       #if (RF_FAM == 61)
                          ,UWORD8 if_ctl
                       #endif
                       )
{
  UWORD8 bit_mask = 0x80;
  UWORD8 ts;
  BOOL   rf_programmed = FALSE;
  UWORD8 count = 0;

  if(!win_id)
  {
    // Nothing programmed yet, we must avoid Mirror effect in Ctrl phase.
    l1pdmacro_anchor(l1_config.params.rx_change_offset_time);
  }

  for (ts=0; ts<8; ts++)
  {
    // the bitmap corresponds to that of the idle frame of the network!!!
  #if ((CHIPSET==3)||(CHIPSET == 4))
    // limitation of 5 measurements for SAMSON (TPU RAM size limitation)
    if((meas_bitmap & bit_mask)&&(count <= 4))
  #else
    if(meas_bitmap & bit_mask)
  #endif
    {
      UWORD16 local_win_id;
      UWORD16 offset;
      WORD16  when;
      UWORD16 offset_chg;

      if((ts>synchro_ts) && (count==0))
      {
        // The 1st Work does not contain any Interf meas.
        // We must ovoid a possible Mirror effect for the rest of TS.
        l1pdmacro_anchor(l1_config.params.rx_change_offset_time);
      }

      // Increment nbr of meas. programmed.
      count++;

      local_win_id = (8 - synchro_ts + ts) * BP_SPLIT;
      if(local_win_id >= (BP_SPLIT * 8)) local_win_id -= BP_SPLIT * 8; // Modulo.

      // Compute offset
      offset_chg = ((local_win_id  * BP_DURATION) >> BP_SPLIT_PW2);
      offset     = offset_serv + offset_chg;
      if(offset >= TPU_CLOCK_RANGE) offset -= TPU_CLOCK_RANGE;

      if(!rf_programmed)
      {
        // Compute offset change timing
        when = offset_chg +
               PROVISION_TIME -
               l1_config.params.rx_synth_setup_time -
               EPSILON_OFFS;

        if(when < 0) when += TPU_CLOCK_RANGE;

        // Program TPU scenario
        l1dmacro_offset   (offset, when);               // change TPU offset according to win_id
        l1dmacro_rx_synth (radio_freq);                 // pgme SYNTH.
    #if (RF_FAM !=61)
	 l1dmacro_agc      (radio_freq, agc,lna_off);    // pgme AGC.
    #endif

    #if (RF_FAM == 61)
	l1dmacro_agc      (radio_freq, agc,lna_off, if_ctl);    // pgme AGC.
    #endif

        rf_programmed = TRUE;
      }
      else
      {
        // Compute offset change timing
        when = offset_chg - BP_DURATION + PROVISION_TIME + PW_ACQUIS_DURATION + 20;
        if(when < 0) when += TPU_CLOCK_RANGE;

        // Program TPU scenario
        l1dmacro_offset   (offset, when);    // change TPU offset according to win_id
      }

      #if (CODE_VERSION == SIMULATION)
        l1dmacro_rx_ms    (radio_freq, 1);     // pgm  PWR acquisition.
      #else
      #if (L1_MADC_ON == 1)
      #if (RF_FAM == 61)
	l1dmacro_rx_ms    (radio_freq,INACTIVE);        // pgm  PWR acquisition.
      #endif
      #else
        l1dmacro_rx_ms    (radio_freq);        // pgm  PWR acquisition.
      #endif
      #endif

      l1dmacro_offset   (offset_serv, IMM);  // restore offset
    }

    bit_mask >>= 1;

  } // for(ts...

  return(count);
}

/*-------------------------------------------------------*/
/* l1dtpu_serv_rx()                                      */
/*-------------------------------------------------------*/
/* Parameters :                                          */
/*             rx_id: range 0-7, first slot of RX group  */
/*             rx_group_id: used in case |RX|  |RX|      */
/*                                                       */
/* Return     :                                          */
/* Functionality :                                       */
/*-------------------------------------------------------*/
void l1pdtpu_serv_rx_nb(UWORD16 radio_freq, WORD8 agc, UWORD8 lna_off,
                        UWORD8 rx_id, UWORD32 offset_serv, UWORD8 num_rx,
                        UWORD8 rx_group_id, BOOL rx_done_flag,UWORD8 adc_active,
                        UWORD8 csf_filter_choice
                         #if (RF_FAM == 61)
                         ,UWORD8 if_ctl
          	            #endif
                        #if (NEW_SNR_THRESHOLD == 1)
                          ,UWORD8 saic_flag
                        #endif /* NEW_SNR_THRESHOLD*/
                                           )
{
  UWORD16 offset;

  #if (CODE_VERSION == SIMULATION)
    UWORD32 tpu_w_page;

     if (hw.tpu_r_page==0)
       tpu_w_page=1;
    else
       tpu_w_page=0;

    hw.rx_id[tpu_w_page][rx_group_id-1]=rx_id;
    hw.num_rx[tpu_w_page][rx_group_id-1]=num_rx;
    hw.rx_group_id[tpu_w_page]=rx_group_id;
  #endif

  offset = offset_serv + (rx_id * BP_DURATION);
  if(offset >= TPU_CLOCK_RANGE) offset -= TPU_CLOCK_RANGE;

  if (rx_group_id == 1)
  {
    // Time tracking.
    l1dmacro_synchro (l1_config.params.rx_change_synchro_time, offset_serv); // Adjust serving OFFSET.

    #if L2_L3_SIMUL
      #if (DEBUG_TRACE == BUFFER_TRACE_OFFSET)
        buffer_trace(3, 0x43, offset_serv, l1s.actual_time.fn, 0);
      #endif
    #endif

    // Change offset to align on RX.
    l1dmacro_offset(offset, IMM);

    // Program Synth.
    // Program ADC measurement
    // Program AGC.
    l1dmacro_rx_synth(radio_freq);
    if(adc_active == ACTIVE)
      l1dmacro_adc_read_rx();

     l1dmacro_agc     (radio_freq, agc, lna_off
                                #if (RF_FAM == 61)
                                ,if_ctl
                                #endif
	                         );
  }
  else
  {
    // Change offset to align on RX.
    l1dmacro_offset(offset, IMM); // Change offset to align on RX.
  }

  l1pdmacro_rx_up  (radio_freq,adc_active, csf_filter_choice
  #if (NEW_SNR_THRESHOLD == 1)
    ,saic_flag
  #endif /* NEW_SNR_THRESHOLD*/

      ); // RX window opened.
  l1pdmacro_rx_down(radio_freq, num_rx, rx_done_flag); // RX window closed.

  // Restore offset to synchro value.
  l1dmacro_offset (offset_serv, IMM);
}

/*-------------------------------------------------------*/
/* l1dtpu_serv_tx()                                      */
/*-------------------------------------------------------*/
/* Parameters :                                          */
/* Return     :                                          */
/* Functionality :                                       */
/*-------------------------------------------------------*/
#if ((ANLG_FAM == 1) || (ANLG_FAM == 2) || (ANLG_FAM == 3))
  #ifndef  ABB_RAMP_UP_TIME  //Flexi ABB Delays defines it in tpudrvXX.h
    #define ABB_RAMP_UP_TIME    32  // maximum time for ramp up
  #endif

  #ifndef ABB_RAMP_DELAY//Flexi ABB Delays defines it in tpudrvXX.h
    #define ABB_RAMP_DELAY       6  // minimum ramp delay APCDEL
  #endif

#ifndef ABB_BULON_HOLD_TIME //Flexi ABB Delays defines it in tpudrvXX.h
	#define ABB_BULON_HOLD_TIME 32  // min. hold time for BULON after BULENA down
#endif


#endif
void l1pdtpu_serv_tx(UWORD16 radio_freq,
                     UWORD8  timing_advance,
                     UWORD32 offset_serv,
                     UWORD8  tx_id,
                     UWORD8  num_tx,
                     UWORD8  tx_group_id,
                     UWORD8  switch_flag,
                     BOOL    burst_type,
                     BOOL    rx_flag,
                     UWORD8  adc_active)
{
  WORD16      time;
  UWORD32     offset_tx;
  UWORD32     timing_advance_in_qbit = (UWORD32)timing_advance << 2;
  #if ((ANLG_FAM == 1) || (ANLG_FAM == 2) || (ANLG_FAM == 3) || (RF_FAM == 61))
    UWORD16 apcdel1_data, apcdel1_data_up;
  #endif
  UWORD8      i;
  static UWORD8 static_switch_flag = 0;

  #if (CODE_VERSION == SIMULATION)
    UWORD32 tpu_w_page;

    if (hw.tpu_r_page==0)
       tpu_w_page=1;
    else
       tpu_w_page=0;

    hw.tx_id[tpu_w_page][tx_group_id-1]=tx_id;
    hw.num_tx[tpu_w_page][tx_group_id-1]=num_tx;
    hw.tx_group_id[tpu_w_page]=tx_group_id;
  #endif

  // Reset timing advance if TA_ALGO not enabled.
  #if !TA_ALGO
    timing_advance_in_qbit = 0;
  #endif

  // In case another group of TX bursts is called, the previous slot was a hole
  // An IT has to be generated to the DSP so that ramps and power level are reloaded
  // This does not apply to combinations of PRACH and TX NB
  if ((tx_group_id > 1) && (!static_switch_flag))
  {
    // exact timing for generation of IT during hole not required but
    // time > time of previous ramp down (BULENA -> BULON down = 32 qb) + margin (10 qb)
   #if (RF_FAM != 61)
    time = TX_TABLE[tx_id-1] + PROVISION_TIME + ABB_BULON_HOLD_TIME + 10
           - l1_config.params.prg_tx_gsm;
   #endif

   #if (RF_FAM == 61)
     time = TX_TABLE[tx_id-1] + PROVISION_TIME + APC_RAMP_DOWN_TIME + 10
           - l1_config.params.prg_tx_gsm;
   #endif

    if (burst_type == TX_NB_BURST)
      time -= timing_advance_in_qbit; // time can never be negative here

    l1pdmacro_it_dsp_gen(time);
  }


  if (tx_group_id == 1)
  {
    #if ((ANLG_FAM == 1) || (ANLG_FAM == 2) || (ANLG_FAM == 3))
      //MS TX, set ABB in MS mode
      #if (DSP == 33) || (DSP == 34) || (DSP == 35) || (DSP == 36) || (DSP == 37)
        // ABB set to MS mode if |TX|TX|.., |TX|PRACH|, |PRACH|TX| or |PRACH|PRACH|
        // switch_flag is set for the first burst of TX/PRACH or PRACH/PRACH combinations
        // MS mode in ABB must be maintained for second burst (static_switch_flag)
        if ((num_tx > 1) || (switch_flag) || (static_switch_flag))
          l1ps_dsp_com.pdsp_ndb_ptr->d_bbctrl_gprs = l1_config.params.bbctrl | B_MSLOT;
        else
          l1ps_dsp_com.pdsp_ndb_ptr->d_bbctrl_gprs = l1_config.params.bbctrl;
      #endif
    #endif
  }
  else
  {
    // handle special case |TX|  |TX|TX|
    #if ((ANLG_FAM == 1) || (ANLG_FAM == 2) || (ANLG_FAM == 3))
      //MS TX, set ABB in MS mode
      #if (DSP == 33) || (DSP == 34) || (DSP == 35) || (DSP == 36) || (DSP == 37)
        if ((num_tx > 1) || (switch_flag) || (static_switch_flag))
          l1ps_dsp_com.pdsp_ndb_ptr->d_bbctrl_gprs = l1_config.params.bbctrl | B_MSLOT;
      #endif
    #endif
  }

  // Compute offset value for TX.
  // PRG_TX has become variable, no longer contained in TIME_OFFSET_TX !
  if ((burst_type == TX_NB_BURST) || (switch_flag==1))
  {
    offset_tx = offset_serv + TX_TABLE[tx_id] + PROVISION_TIME
                 - l1_config.params.prg_tx_gsm - timing_advance_in_qbit;
  }
  else
  {
    offset_tx = offset_serv + TX_TABLE[tx_id] + PROVISION_TIME
                 - l1_config.params.prg_tx_gsm;
  }

  // offset_tx mod 5000
  if (offset_tx >= TPU_CLOCK_RANGE)
    offset_tx -= TPU_CLOCK_RANGE;

  if(rx_flag == TRUE)
  {
    time = offset_tx -
           l1_config.params.tx_synth_setup_time -
           EPSILON_OFFS
           - offset_serv;
    if ((burst_type == TX_NB_BURST) || (switch_flag==1))
      time += timing_advance_in_qbit - TA_MAX;
  }
  else
    time = TPU_CLOCK_RANGE - EPSILON_SYNC;

  if (time < 0)
    time += TPU_CLOCK_RANGE;

  if (!static_switch_flag)
    l1dmacro_offset  (offset_tx, (WORD32) time);  // load OFFSET for TX before each burst.

  #if L2_L3_SIMUL
    #if (DEBUG_TRACE == BUFFER_TRACE_OFFSET)
      buffer_trace(2, offset_tx,l1s.actual_time.fn,0,0);
    #endif
  #endif

  time=0;

  // program PLL only if no TX control carried out in same frame: |TX|  |TX|TX| possible
  // |PRACH|TX|, |TX|PRACH| or |PRACH|PRACH| also possible
  if (tx_group_id == 1)
  {
    l1pdmacro_tx_synth(radio_freq); // load SYNTH.
  }

  if (!static_switch_flag)  // window opened for previous time slot (TX/PRACH or PRACH/PRACH)
    l1pdmacro_tx_up(radio_freq);     // TX window opened


  #if (CODE_VERSION == SIMULATION)
    if (burst_type == TX_RA_BURST)
    {
      time += l1_config.params.tx_ra_duration;
    }
    else
    {
      if (num_tx > 1)
      //  num_tx * BP_DURATION
        time += TX_TABLE[num_tx - 1] + l1_config.params.tx_nb_duration;
      else
        time += l1_config.params.tx_nb_duration;
    }
  #else
    #if ((ANLG_FAM == 1) || (ANLG_FAM == 2) || (ANLG_FAM == 3))
      // Read APCDEL1 register DELU(4:0): delay of ramp up start, DELD (9:5) delay of ramp down start
      // This value is used for computations in MS TX or TX/PRACH combinations
      // This value is not modified by the computations
      apcdel1_data    = (l1s_dsp_com.dsp_ndb_ptr->d_apcdel1 >> 6) & 0x03ff;
      apcdel1_data_up = apcdel1_data & 0x001f;  //delay of ramp up start
    #endif

#if (RF_FAM == 61)
      // Read APCDEL1 register DELU(4:0): delay of ramp up start, DELD (9:5) delay of ramp down start
      // This value is used for computations in MS TX or TX/PRACH combinations
      // This value is not modified by the computations
      apcdel1_data    = (l1s_dsp_com.dsp_ndb_ptr->d_apcdel1) & 0x03ff;
      apcdel1_data_up = apcdel1_data & 0x001f;  //delay of ramp up start
#endif

    if (!switch_flag)
    {
      if (burst_type == TX_NB_BURST)
      {
        // If PRACH precedes TX normal burst(s) we have to add BP_DURATION
        if (static_switch_flag)
          time += BP_DURATION;

        // generate DSP IT for each TX slot after ramp up
        // Margin:
        // ABB_RAMP_DELAY = 4*1.5bits internal ABB delay BULENA ON -> ramp up
        // apcdel1_data_up = additional delay BULENA ON -> ramp up
        // ABB_RAMP_UP_TIME: maximum time for ramp up: 16 coeff.
        // 10 qbits of additional margin
        #if ((ANLG_FAM == 1) || (ANLG_FAM == 2) || (ANLG_FAM == 3))
          for (i=0; i<num_tx; i++)
            l1pdmacro_it_dsp_gen(time + ABB_RAMP_DELAY + ABB_RAMP_UP_TIME + i*BP_DURATION + apcdel1_data_up + 10);
        #endif

        #if (RF_FAM == 61)
          for (i=0; i<num_tx; i++)
            l1pdmacro_it_dsp_gen(time + APC_RAMP_DELAY + APC_RAMP_UP_TIME + i*BP_DURATION + apcdel1_data_up + 10);
        #endif


        if (num_tx > 1)
        //  (num_tx - 1) * BP_DURATION + normal burst duration
          time += TX_TABLE[num_tx - 1] + l1_config.params.tx_nb_duration - (num_tx - 1);
        else
          time += l1_config.params.tx_nb_duration;
      }
      else //PRACH
      {
        // If TX NB precedes PRACH we have to add BP_DURATION and TA (in qbits)
        if (static_switch_flag == 1)
        {
          if (timing_advance_in_qbit > 240) // clip TA, cf. comment below
            timing_advance_in_qbit = 240;
          time += BP_DURATION + timing_advance_in_qbit;
        }
        // If PRACH precedes PRACH we have to add BP_DURATION
        else if (static_switch_flag == 2)
          time += BP_DURATION ;

        #if ((ANLG_FAM == 1) || (ANLG_FAM == 2) || (ANLG_FAM == 3))
          l1pdmacro_it_dsp_gen(time + ABB_RAMP_DELAY + ABB_RAMP_UP_TIME + apcdel1_data_up + 10);
        #endif

	 #if (RF_FAM == 61)
          l1pdmacro_it_dsp_gen(time + APC_RAMP_DELAY + APC_RAMP_UP_TIME + apcdel1_data_up + 10);
        #endif

        time += l1_config.params.tx_ra_duration;
      }

    }
    else if (switch_flag == 1) // |TX|PRACH| or |PRACH|TX|
    {
      #if (DSP == 33) || (DSP == 34) || (DSP == 35) || (DSP == 36) || (DSP == 37) || (DSP == 38) || (DSP == 39)
        // => ABB windows are opened as for TX_NB in MS mode
        // => Ramp up start of PRACH is delayed inside this window by the TA of the TX_NB
        // => DSP inserts dummy bits such that ramp and modulation match
        // Rem.: the TA passed for the PRACH is the one for the following TX_NB!!!
        #if ((ANLG_FAM == 1) || (ANLG_FAM == 2) || (ANLG_FAM == 3)) || (RF_FAM == 61)
          // In combinations of TX_NB and PRACH apcdel1_bis and apcdel2_bis apply to the PRACH
          UWORD16 apcdel1_bis_data, apcdel1_bis_data_up, apcdel2_bis_data_up, prach_delay;
          API d_ctrl_abb_gprs;

          // clip TA (in qbit): max. TA supported = BP_DURATION - PRACH duration - max. ramp time
          //                                      = 625 - 88*4 - 32 = 241
          if (timing_advance_in_qbit > 240)
            timing_advance_in_qbit = 240;

          prach_delay = apcdel1_data_up + timing_advance_in_qbit;
          apcdel1_bis_data_up = prach_delay & 0x001f;
          apcdel2_bis_data_up = (prach_delay >> 5) & 0x001f;

          // For ramp down delay we need to keep the original value from APCDEL1 (bits 9:5)
          // APCDEL2 default value is '0'
          apcdel1_bis_data = apcdel1_bis_data_up | (apcdel1_data & 0x03e0);

       #if(RF_FAM != 61)
                 l1s_dsp_com.dsp_ndb_ptr->d_apcdel1_bis  = (apcdel1_bis_data << 6) | 0x04;
                 l1s_dsp_com.dsp_ndb_ptr->d_apcdel2_bis = (apcdel2_bis_data_up << 6) | 0x34;
       #else
                 l1s_dsp_com.dsp_ndb_ptr->d_apcdel1_bis  = (apcdel1_bis_data );
                 l1s_dsp_com.dsp_ndb_ptr->d_apcdel2_bis = (apcdel2_bis_data_up);
       #endif

                 if (burst_type == TX_RA_BURST) // |PRACH|TX|
                 {

       #if(RF_FAM != 61)
                   l1pdmacro_it_dsp_gen(time + ABB_RAMP_DELAY + ABB_RAMP_UP_TIME + prach_delay + 10);
       #else
                   l1pdmacro_it_dsp_gen(time + APC_RAMP_DELAY + APC_RAMP_UP_TIME + prach_delay + 10);
       #endif
            // apcdel1_bis, apcdel2_bis must be programmed for the current ts (PRACH)
            // here we need to overwrite (mask) bits for APCDEL1, APCDEL2 programming done in l1pddsp_transfer_mslot_power()
            d_ctrl_abb_gprs = l1ps_dsp_com.pdsp_db_w_ptr->a_ctrl_abb_gprs[tx_id];
            d_ctrl_abb_gprs |= ((1 << B_BULRAMPDEL_BIS) | (1 << B_BULRAMPDEL2_BIS));
            d_ctrl_abb_gprs &= ~((1 << B_BULRAMPDEL) | (1 << B_BULRAMPDEL2));
            l1ps_dsp_com.pdsp_db_w_ptr->a_ctrl_abb_gprs[tx_id] = d_ctrl_abb_gprs;
          }
          else // |TX|PRACH|
          {
          #if(RF_FAM != 61)
            l1pdmacro_it_dsp_gen(time + ABB_RAMP_DELAY + ABB_RAMP_UP_TIME + apcdel1_data_up + 10);
  	   #else
            l1pdmacro_it_dsp_gen(time + APC_RAMP_DELAY + APC_RAMP_UP_TIME + apcdel1_data_up + 10);
	   #endif

            // apcdel1_bis, apcdel2_bis must be programmed for the next ts (PRACH)
            d_ctrl_abb_gprs = l1ps_dsp_com.pdsp_db_w_ptr->a_ctrl_abb_gprs[tx_id + 1];
            d_ctrl_abb_gprs |= ((1 << B_BULRAMPDEL_BIS) | (1 << B_BULRAMPDEL2_BIS));
            d_ctrl_abb_gprs &= ~((1 << B_BULRAMPDEL) | (1 << B_BULRAMPDEL2));
            l1ps_dsp_com.pdsp_db_w_ptr->a_ctrl_abb_gprs[tx_id + 1] = d_ctrl_abb_gprs;
          }
        #endif // ANALOG

        static_switch_flag = 1;

      #endif // DSP == 33 || DSP == 34 || (DSP == 36)  || (DSP == 37)
    }
    else if (switch_flag == 2) // |PRACH|PRACH|
    // Combination handled by programming ABB with MS mode = 1
    // => first burst length of first PRACH = BP_DURATION
    {
      #if ((ANLG_FAM == 1) || (ANLG_FAM == 2) || (ANLG_FAM == 3))
        l1pdmacro_it_dsp_gen(time + ABB_RAMP_DELAY + ABB_RAMP_UP_TIME + apcdel1_data_up + 10);
      #endif

      #if (RF_FAM == 61)
        l1pdmacro_it_dsp_gen(time + APC_RAMP_DELAY + APC_RAMP_UP_TIME + apcdel1_data_up + 10);
      #endif

      static_switch_flag = 2;
    }
  #endif //Codeversion

  // In case of combinations TX_NB/PRACH or PRACH/PRACH the TX window is kept open
  if (!switch_flag)
  {
    l1pdmacro_tx_down(radio_freq, time, switch_flag, timing_advance_in_qbit,adc_active);     // TX window closed

    l1dmacro_offset  (offset_serv, IMM); // Restore offset with serving value.

    static_switch_flag = 0;
  }

  #if L2_L3_SIMUL
    #if (DEBUG_TRACE == BUFFER_TRACE_OFFSET)
      buffer_trace(2, offset_serv,l1s.actual_time.fn,0,0);
    #endif
  #endif
}

/*-------------------------------------------------------*/
/* l1pddsp_synchro()                                     */
/*-------------------------------------------------------*/
/* Parameters :                                          */
/* Return     :                                          */
/* Functionality :                                       */
/*-------------------------------------------------------*/
void l1pddsp_synchro(UWORD8 switch_mode, UWORD8  camp_timeslot)
{
  // Set "b_abort" to TRUE.
  l1s_dsp_com.dsp_db_w_ptr->d_ctrl_system |= (1 << B_TASK_ABORT);

  // Set switch mode within "b_switch_to_gprs" & "b_switch_to_gms"
  l1ps_dsp_com.pdsp_ndb_ptr->d_sched_mode_gprs = (switch_mode << B_SWITCH);

  // In case of a switch to GPRS_SCHEDULER, last_used_txpwr is set to "NO_TXPWR"
  // in order to force GSM ramp programming when the MS will switch back to
  // GSM_SCHEDULER
  // Moreover, the d_win_start_gprs register must be initialized only during the
  // GSM->GPRS switch too.
  if(switch_mode == GPRS_SCHEDULER)
  {
    l1s.last_used_txpwr = NO_TXPWR;

    // Set camp timeslot.
    l1ps_dsp_com.pdsp_ndb_ptr->d_win_start_gprs = camp_timeslot;
  }
}


/*-------------------------------------------------------*/
/* l1pddsp_idle_prach_data()                             */
/*-------------------------------------------------------*/
/* Parameters :                                          */
/* Return     :                                          */
/* Functionality :                                       */
/*-------------------------------------------------------*/
void l1pddsp_idle_prach_data(BOOL    polling,
                             UWORD8  cs_type,
                             UWORD16  channel_request_data,
                             UWORD8  bsic,
                             UWORD16 radio_freq)
{
  UWORD16  swap_bit; // 16 bit wide to allow shift left.

  // UL on TS=3.
  l1ps_dsp_com.pdsp_db_w_ptr->d_task_u_gprs |= 0x80 >> 3;

  // Swap I/Q management.
  swap_bit = l1ps_swap_iq_ul(radio_freq);
  l1ps_dsp_com.pdsp_db_w_ptr->d_task_u_gprs |= swap_bit << 15;

  // Load UL buffer according to "polling" bit.
  if(polling)
  {
    // Select first UL polling buffer.
    l1ps_dsp_com.pdsp_ndb_ptr->a_ul_buffer_gprs[3] = 8;

    // Store CS type.
    l1ps_dsp_com.pdsp_ndb_ptr->a_pu_gprs[0][0] = cs_type;

    // Store UL data block.
    if(cs_type == CS_PAB8_TYPE)
    {
      l1ps_dsp_com.pdsp_ndb_ptr->a_pu_gprs[0][2] = ((API)(bsic << 2)) |
                                                   ((API)(channel_request_data) << 8);
      l1ps_dsp_com.pdsp_ndb_ptr->a_pu_gprs[0][3] = 0;
    }
    else
    {
      l1ps_dsp_com.pdsp_ndb_ptr->a_pu_gprs[0][2] = ((API)(channel_request_data) << 5);
      l1ps_dsp_com.pdsp_ndb_ptr->a_pu_gprs[0][3] = ((API)(bsic << 10));
    }
  }
  else
  {
    // Set "b_access_prach" to indicate 1 Prach only to DSP.
    l1ps_dsp_com.pdsp_db_w_ptr->d_task_u_gprs |= (1 << B_ACCESS_PRACH);

    // Select first UL data buffer.
    l1ps_dsp_com.pdsp_ndb_ptr->a_ul_buffer_gprs[3] = 0;

    // Store CS type.
    l1ps_dsp_com.pdsp_ndb_ptr->a_du_gprs[0][0] = cs_type;

    // Store UL data block.
    if(cs_type == CS_PAB8_TYPE)
    {
      l1ps_dsp_com.pdsp_ndb_ptr->a_du_gprs[0][1] = ((API)(bsic << 2)) |
                                                   ((API)(channel_request_data) << 8);
      l1ps_dsp_com.pdsp_ndb_ptr->a_du_gprs[0][2] = 0;
    }
    else
    {
      l1ps_dsp_com.pdsp_ndb_ptr->a_du_gprs[0][1] = ((API)(channel_request_data) << 5);
      l1ps_dsp_com.pdsp_ndb_ptr->a_du_gprs[0][2] = ((API)(bsic << 10));
    }

    if (l1pa_l1ps_com.pra_info.prach_alloc == FIX_PRACH_ALLOC)
    {
      // Set fix alloc bit.
      l1ps_dsp_com.pdsp_ndb_ptr->d_sched_mode_gprs |= (2 << B_MAC_MODE);
    }
    else
    {
      // Reset MAC mode to dynamic allocation
      l1ps_dsp_com.pdsp_ndb_ptr->d_sched_mode_gprs &= ~(3 << B_MAC_MODE);

      #if !FF_L1_IT_DSP_USF
      #if (DSP == 33) || (DSP == 34) || (DSP == 35) || (DSP == 36) || (DSP == 37) || (DSP == 38) || (DSP == 39)
        // Enable USF vote on timeslot 0
        l1ps_dsp_com.pdsp_ndb_ptr->d_usf_vote_enable = 0x80;
      #endif
      #endif
    }
  }
}

/*-------------------------------------------------------*/
/* l1pddsp_idle_prach_power()                            */
/*-------------------------------------------------------*/
/* Parameters :                                          */
/* Return     :                                          */
/* Functionality :                                       */
/*-------------------------------------------------------*/
void l1pddsp_idle_prach_power(UWORD8   txpwr,
                              UWORD16  radio_freq,
                              UWORD8   ts)
{
  #if ((ANLG_FAM == 1) || (ANLG_FAM == 2) || (ANLG_FAM == 3) || (RF_FAM == 61))
    UWORD16 pwr_data;
  #endif

  #if ((ANLG_FAM == 1) || (ANLG_FAM == 2) || (ANLG_FAM == 3) )
    // Force FIXED transmit power if requested.
    if(l1_config.tx_pwr_code == 0)
    {
      l1ps_dsp_com.pdsp_db_w_ptr->a_ctrl_power_gprs[ts] = l1_config.params.fixed_txpwr;

      // Control bitmap: update RAMP, use RAMP[5][..].
      l1ps_dsp_com.pdsp_db_w_ptr->a_ctrl_abb_gprs[ts] =
                      ((1 << B_RAMP_GPRS) | (5 << B_RAMP_NB_GPRS) | (1 << B_BULRAMPDEL) | (1 << B_BULRAMPDEL2));

             // Store Ramp.
	  #if (CODE_VERSION != SIMULATION)
        Cust_get_ramp_tab(l1ps_dsp_com.pdsp_ndb_ptr->a_ramp_gprs[5],
                                            0, /* not used */
                                            0, /* not used */
                                            1  /* arbitrary value for arfcn */ );
	  #endif
    }
    else
    {
      // Get H/W value corresponding to txpwr command.
      pwr_data = Cust_get_pwr_data(txpwr, radio_freq
                                          #if(REL99 && FF_PRF)
                                          ,1
                                          #endif
                                          );

      // Store Transmit power.
      l1ps_dsp_com.pdsp_db_w_ptr->a_ctrl_power_gprs[ts] = ((pwr_data << 6) | 0x12);

      // Control bitmap: update RAMP, use RAMP[5][..].
      l1ps_dsp_com.pdsp_db_w_ptr->a_ctrl_abb_gprs[ts] = ((1 << B_RAMP_GPRS) | (5 << B_RAMP_NB_GPRS) | (1 << B_BULRAMPDEL) | (1 << B_BULRAMPDEL2));

      // Store Ramp.
	  #if (CODE_VERSION != SIMULATION)
        Cust_get_ramp_tab(&(l1ps_dsp_com.pdsp_ndb_ptr->a_ramp_gprs[5][0]), txpwr, txpwr, radio_freq);
	  #endif
    }
  #endif

  #if (RF_FAM == 61)
    // Force FIXED transmit power if requested.
    if(l1_config.tx_pwr_code == 0)
    {
      l1ps_dsp_com.pdsp_db_w_ptr->a_ctrl_power_gprs[ts] = l1_config.params.fixed_txpwr;

      // Control bitmap: update RAMP, use RAMP[5][..].
      l1ps_dsp_com.pdsp_db_w_ptr->a_ctrl_abb_gprs[ts] =
                      ((1 << B_RAMP_GPRS) | (5 << B_RAMP_NB_GPRS) | (1 << B_BULRAMPDEL) | (1 << B_BULRAMPDEL2));

      // Store Ramp.
    #if (DSP ==38) || (DSP == 39)
       Cust_get_ramp_tab(l1ps_dsp_com.pdsp_ndb_ptr->a_drp_ramp2_gprs[5],
                                        0, /* not used */
                                        0, /* not used */
                                        1  /* arbitrary value for arfcn */ );
    #endif
    }
    else
    {
      // Get H/W value corresponding to txpwr command.
      pwr_data = Cust_get_pwr_data(txpwr, radio_freq
      									  #if(REL99 && FF_PRF)
      									  ,1
      									  #endif
      									  );

      // Store Transmit power.
      l1ps_dsp_com.pdsp_db_w_ptr->a_ctrl_power_gprs[ts] = (API) (pwr_data);

      // Control bitmap: update RAMP, use RAMP[5][..].
      l1ps_dsp_com.pdsp_db_w_ptr->a_ctrl_abb_gprs[ts] = ((1 << B_RAMP_GPRS) | (5 << B_RAMP_NB_GPRS) | (1 << B_BULRAMPDEL) | (1 << B_BULRAMPDEL2));

      // Store Ramp.
      #if(DSP == 38) || (DSP == 39)
        Cust_get_ramp_tab(&(l1ps_dsp_com.pdsp_ndb_ptr->a_drp_ramp2_gprs[5][0]), txpwr, txpwr, radio_freq);
     #endif
    }
  #endif //RF_FAM == 61

}

/*-------------------------------------------------------*/
/* l1pddsp_single_block()                                */
/*-------------------------------------------------------*/
/* Parameters :                                          */
/* Return     :                                          */
/* Functionality :                                       */
/*-------------------------------------------------------*/
void l1pddsp_single_tx_block(UWORD8  burst_nb,
                             UWORD8 *data,
                             UWORD8  tsc,
                             UWORD16 radio_freq)
{
  UWORD16  swap_bit; // 16 bit wide to allow shift left.

  // Burst number within a block.
  l1ps_dsp_com.pdsp_db_w_ptr->d_burst_nb_gprs = burst_nb;

  // UL on TS=3.
  l1ps_dsp_com.pdsp_db_w_ptr->d_task_u_gprs |= 0x80 >> 3;

  // Swap I/Q management.
  swap_bit = l1ps_swap_iq_ul(radio_freq);
  l1ps_dsp_com.pdsp_db_w_ptr->d_task_u_gprs |= swap_bit << 15;

  // Select first UL polling buffer.
  l1ps_dsp_com.pdsp_ndb_ptr->a_ul_buffer_gprs[3] = 8;

  // Store CS type: CS1 for Polling.
  l1ps_dsp_com.pdsp_ndb_ptr->a_pu_gprs[0][0] = CS1_TYPE_POLL;

  if(burst_nb == BURST_1)
  // Store UL data block.
  {
    API    *ul_block_ptr = &(l1ps_dsp_com.pdsp_ndb_ptr->a_pu_gprs[0][2]);
    UWORD8  i,j;

    // Copy first 22 bytes in the first 11 words after header.
    for (i=0, j=0; j<11; j++)
    {
      ul_block_ptr[j] = ((API)(data[i])) | ((API)(data[i+1]) << 8);
      i += 2;
    }
    // Copy last UWORD8 (23rd) in the 12th word after header.
    ul_block_ptr[11] = data[22];
  }

  // Training sequence.
  // Rem: bcch_freq_ind is set within Hopping algo.
  l1s_dsp_com.dsp_db_w_ptr->d_ctrl_system |= tsc << B_TSQ;
}

/*-------------------------------------------------------*/
/* l1pddsp_idle_rx_nb()                                  */
/*-------------------------------------------------------*/
/* Parameters :                                          */
/* Return     :                                          */
/* Functionality :                                       */
/*-------------------------------------------------------*/
#if FF_L1_IT_DSP_USF
void l1pddsp_idle_rx_nb(UWORD8  burst_nb,
                        UWORD8  tsc,
                        UWORD16 radio_freq,
                        UWORD8  timeslot_no,
                        BOOL    ptcch_dl,
                        BOOL    usf_interrupt)
#else
void l1pddsp_idle_rx_nb(UWORD8  burst_nb,
                        UWORD8  tsc,
                        UWORD16 radio_freq,
                        UWORD8  timeslot_no,
                        BOOL    ptcch_dl)
#endif
{
  UWORD16  swap_bit; // 16 bit wide to allow shift left.

  // DL on TS=0.
  l1ps_dsp_com.pdsp_db_w_ptr->d_task_d_gprs |= 0x80 >> timeslot_no;

  // Swap I/Q management.
  swap_bit = l1ps_swap_iq_dl(radio_freq);
  l1ps_dsp_com.pdsp_db_w_ptr->d_task_d_gprs |= swap_bit << 15;

  if(ptcch_dl)
  {
    // PTCCH/DL case must be flagged to DSP.
    l1ps_dsp_com.pdsp_db_w_ptr->d_task_d_gprs |= (1 << B_PTCCH_DL);
  }

  // Burst number within a block.
  l1ps_dsp_com.pdsp_db_w_ptr->d_burst_nb_gprs = burst_nb;

  // Channel coding is forced to CS1.
  l1ps_dsp_com.pdsp_ndb_ptr->a_ctrl_ched_gprs[timeslot_no] = CS1_TYPE_DATA;

  // pass information to DSP which good USF value is to be expected
  l1ps_dsp_com.pdsp_ndb_ptr->a_usf_gprs[0] = (API) 0x07;

  #if FF_L1_IT_DSP_USF
    // In case of connection establishment mode with dynamic or fixed
    // allocation scheme we need to request the DSP USF interrupt for PRACH
    // scheduling. Latched by DSP during Work3
    if  (burst_nb == 3)
    {
      if (usf_interrupt)
        l1ps_dsp_com.pdsp_ndb_ptr->d_usf_vote_enable |= (1 << B_USF_IT);
      else
        l1ps_dsp_com.pdsp_ndb_ptr->d_usf_vote_enable &= ~(1 << B_USF_IT);
    }
  #endif

  // RIF receiver algorithm: select 156.25.
  l1ps_dsp_com.pdsp_ndb_ptr->d_sched_mode_gprs &= 0xFFFF ^ (1 << B_RIF_RX_MODE);

  // Training sequence.
  // Rem: bcch_freq_ind is set within Hopping algo.
  l1s_dsp_com.dsp_db_w_ptr->d_ctrl_system |= tsc << B_TSQ;

}


/*-------------------------------------------------------*/
/* l1pddsp_transfer_mslot_ctrl()                         */
/*-------------------------------------------------------*/
/* Parameters :                                          */
/* Return     :                                          */
/* Functionality :                                       */
/*-------------------------------------------------------*/
void l1pddsp_transfer_mslot_ctrl(UWORD8  burst_nb,
                                 UWORD8  dl_bitmap,
                                 UWORD8  ul_bitmap,
                                 UWORD8  *usf_table,
                                 UWORD8  mac_mode,
                                 UWORD8  *ul_buffer_index,
                                 UWORD8  tsc,
                                 UWORD16 radio_freq,
                                 UWORD8  synchro_timeslot,
                               #if FF_L1_IT_DSP_USF
                                 UWORD8  dsp_usf_interrupt
                               #else
                                 UWORD8  usf_vote_enable
                               #endif
                                  )
{
  UWORD8   i;
  UWORD16  swap_bit; // 16 bit wide to allow shift left.

  // Burst number within a block.
  l1ps_dsp_com.pdsp_db_w_ptr->d_burst_nb_gprs = burst_nb;

  // DL bitmap.
  l1ps_dsp_com.pdsp_db_w_ptr->d_task_d_gprs = dl_bitmap;

  // UL bitmap.
  l1ps_dsp_com.pdsp_db_w_ptr->d_task_u_gprs = ul_bitmap;

  // Swap I/Q management for DL.
  swap_bit = l1ps_swap_iq_dl(radio_freq);
  l1ps_dsp_com.pdsp_db_w_ptr->d_task_d_gprs |= swap_bit << 15;

  // Swap I/Q management for UL.
  swap_bit = l1ps_swap_iq_ul(radio_freq);
  l1ps_dsp_com.pdsp_db_w_ptr->d_task_u_gprs |= swap_bit << 15;

  if(burst_nb == 0)
  {
    // Store USF table
    for(i=0;i<(8 - synchro_timeslot);i++)
      l1ps_dsp_com.pdsp_ndb_ptr->a_usf_gprs[i] = usf_table[i+synchro_timeslot];

    // Automatic CS detection.
    for(i=0;i<8;i++)
    {
      l1ps_dsp_com.pdsp_ndb_ptr->a_ctrl_ched_gprs[i] = CS_AUTO_DETECT;

      // Select first UL polling buffer.
      l1ps_dsp_com.pdsp_ndb_ptr->a_ul_buffer_gprs[i] = ul_buffer_index[i];
    }

    #if !FF_L1_IT_DSP_USF
    // USF vote enable programming

    #if (DSP == 33) || (DSP == 34) || (DSP == 35) || (DSP == 36) || (DSP == 37) || (DSP == 38) || (DSP == 39)
      // Multislot TX allowed and usf_vote_enable suported: programs usf_vote_enable
      l1ps_dsp_com.pdsp_ndb_ptr->d_usf_vote_enable = usf_vote_enable;
    #else
      // Single slot TX only and usf_vote_enable not supported
      // Modify MAC mode
      if (usf_vote_enable)
        // USF vote enabled  --> Set MAC mode to dynamic mode
        mac_mode = DYN_ALLOC;
      else
        // USF vote disabled --> Set MAC mode to fixed mode
        mac_mode = FIX_ALLOC_NO_HALF;
    #endif

    #endif // !FF_L1_IT_DSP_USF

    // MAC mode.
    l1ps_dsp_com.pdsp_ndb_ptr->d_sched_mode_gprs &= ~(3 << B_MAC_MODE);
    l1ps_dsp_com.pdsp_ndb_ptr->d_sched_mode_gprs |= mac_mode << B_MAC_MODE;
  }

  #if FF_L1_IT_DSP_USF
    if(burst_nb == 3)
    {
      // Program DSP to generate an interrupt once USF available if
      // required. Latched by DSP during Work3.
      if (dsp_usf_interrupt)
        l1ps_dsp_com.pdsp_ndb_ptr->d_usf_vote_enable = (1 << B_USF_IT);
      else
        l1ps_dsp_com.pdsp_ndb_ptr->d_usf_vote_enable = 0;
    }
  #endif

  // RIF receiver algorithm: select 156.25.
  l1ps_dsp_com.pdsp_ndb_ptr->d_sched_mode_gprs &= 0xFFFF ^ (1 << B_RIF_RX_MODE);

  // d_fn
  // ----
  //   bit [0..7]  -> b_fn_report, unused for GPRS
  //   bit [8..15] -> b_fn_sid   , FN%104
  l1s_dsp_com.dsp_db_w_ptr->d_fn = ((l1s.next_time.fn_mod104)<<8);

  // Training sequence.
  // Rem: bcch_freq_ind is set within Hopping algo.
  l1s_dsp_com.dsp_db_w_ptr->d_ctrl_system |= tsc << B_TSQ;

}


/*-------------------------------------------------------*/
/* l1pddsp_transfer_mslot_power()                        */
/*-------------------------------------------------------*/
/* Parameters :                                          */
/* Return     :                                          */
/* Functionality :                                       */
/*-------------------------------------------------------*/
void l1pddsp_transfer_mslot_power(UWORD8  *txpwr,
                                  UWORD16  radio_freq,
                                  UWORD8   ul_bitmap)
{
  #define  NO_TX   100

  UWORD16  i;        // 16 bit needed for shifting pupose.
  UWORD8   last_TX = NO_TX;
  UWORD8   txpwr_ramp_up;
  UWORD8   txpwr_ramp_down;
  UWORD8   cpt_TX = 0;
  UWORD8   ts_mask;
  WORD16 ts_conv;

    #if (REL99 && FF_PRF)
      UWORD8 number_uplink_timeslot = 0 ; // number of uplink timeslot for power reduction feature
    #endif


  #if ((ANLG_FAM == 1) || (ANLG_FAM == 2) || (ANLG_FAM == 3) || (RF_FAM == 61))
    UWORD16 pwr_data;
    UWORD16 d_ramp_idx;
  #endif

//Locosto  #if ((ANLG_FAM == 1) || (ANLG_FAM == 2) || (ANLG_FAM == 3))

    // This function is called with an ul_bitmap which represents the abolute
    // position of any Tx bursts in this frame. This bitmap has already
    // absorbed any synchro change (in dl_tn), hence we need to do some
    // processing to recover the actual Tx timeslot number which is used
    // as an index into the txpwr array.
    //
    // Example : MS Class 8 with 4 Rx and 1 Tx :
    //
    //
    // dl_ts_alloc      :   0x0f         0 0 0 0 R R R R
    // ul_ts_alloc      :   0x02         0 0 0 0 0 0 T 0
    // shift + combine  :                0 0 0 0 R R R R 0 T
    // set dl_tn=4      :                R R R R 0 T 0 0
    // ul_bitmap        :   0x04         0 0 0 0 0 1 0 0
    // i                :      5
    //
    // Example : MS Class 8 with 1 Rx and 1 Tx on TS=7
    //
    // dl_ts_alloc      :   0x01         0 0 0 0 0 0 0 R
    // ul_ts_alloc      :   0x01         0 0 0 0 0 0 0 T
    // shift + combine  :                0 0 0 0 0 0 0 R 0 0 T
    // set dl_tn=7      :                R 0 0 T 0 0 0 0
    // ul_bitmap        :   0x10         0 0 0 1 0 0 0 0
    // i                :      3
    //
    // We recover the actual timeslot from the ul_bitmap by the following
    // method :
    //
    //  ts = (i + dl_tn) - 3
    //
    //  Where i is the loopindex usd to detect "1" in the ul_bitmap.
    //  This works for MS class 8 because (3 <= i <= 5) if the
    //  multislot class is respected.

        #if (REL99 && FF_PRF)// power reduction feature
	      for (i=0; i<8; i++)
	      {
	        // computed number of uplink timeslot in order to determine uplink power reduction
	        ts_mask = (0x80>>i);
	        if (ul_bitmap & ts_mask)
	          number_uplink_timeslot++;
	      }
	    #endif


    ts_conv = l1a_l1s_com.dl_tn - 3;

    // Index of the programmed ramps
    d_ramp_idx = 0;

    for(i=0;i<8;i++)
    {
      // Program Transmit power and ramp for allocated timeslots.
      if(ul_bitmap & (0x80>>i))
      {
        // Fixe transmit power.
        if(l1_config.tx_pwr_code == 0)
        {
          // Store Transmit power.
          l1ps_dsp_com.pdsp_db_w_ptr->a_ctrl_power_gprs[i] = l1_config.params.fixed_txpwr;

          // Control bitmap: update RAMP, use RAMP[d_ramp_idx][..].
          l1ps_dsp_com.pdsp_db_w_ptr->a_ctrl_abb_gprs[i] =
                          ((d_ramp_idx << B_RAMP_NB_GPRS) | (1 << B_RAMP_GPRS) | (1 << B_BULRAMPDEL) | (1 << B_BULRAMPDEL2));

          // Store Ramp.
#if (RF_FAM == 61)
  #if (DSP ==38) || (DSP == 39)
          Cust_get_ramp_tab(l1ps_dsp_com.pdsp_ndb_ptr->a_drp_ramp2_gprs[d_ramp_idx++],
                                         0, /* not used */
                                         0, /* not used */
                                         1  /* arbitrary value for arfcn */ );
    #endif
#else
			#if (CODE_VERSION != SIMULATION)
          Cust_get_ramp_tab(l1ps_dsp_com.pdsp_ndb_ptr->a_ramp_gprs[d_ramp_idx++],
                                         0, /* not used */
                                         0, /* not used */
                                         1  /* arbitrary value for arfcn */ );
#endif
		  #endif
        }
        else
        {
          // count the number of TX windows
          cpt_TX ++;

          // Get power amplifier data.
          #if(REL99 && FF_PRF)
          pwr_data = Cust_get_pwr_data(txpwr[i+ts_conv], radio_freq, number_uplink_timeslot);
          #else
          pwr_data = Cust_get_pwr_data(txpwr[i+ts_conv], radio_freq);
          #endif


          // Store Transmit power.
          #if(RF_FAM == 61)
            l1ps_dsp_com.pdsp_db_w_ptr->a_ctrl_power_gprs[i] = (pwr_data);
          #else
            l1ps_dsp_com.pdsp_db_w_ptr->a_ctrl_power_gprs[i] = ((pwr_data << 6) | 0x12);
          #endif

          // Control bitmap: update RAMP, use RAMP[d_ramp_idx][..] for slot i.
          l1ps_dsp_com.pdsp_db_w_ptr->a_ctrl_abb_gprs[i] = ((d_ramp_idx << B_RAMP_NB_GPRS) | (1 << B_RAMP_GPRS) | (1 << B_BULRAMPDEL) | (1 << B_BULRAMPDEL2));

          // Store Ramp.
          // ==========
          // for the 1st  TX the RAMP is:        RAMP_UP_TX1     / RAMP_DOWN_TX1
          // for the 2nd  TX the RAMP is:        RAMP_UP_TX2     / RAMP_DOWN_TX1
          // for the 3rd   TX the RAMP is:        RAMP_UP_TX3    / RAMP_DOWN_TX2
          // (...)
          // for the (i)th TX the RAMP is:        RAMP_UP_TX_(i) / RAMP_DOWN_TX_(i-1)
          // for the additionnal RAMP   :            xxxx        / RAMP_DOWN_TX_last

          txpwr_ramp_up = txpwr[i+ts_conv]; // the ramp up is the current TX

          if(last_TX == NO_TX)  // specific case of the first TX
            txpwr_ramp_down = txpwr[i+ts_conv];       // the ramp down is the current TX
          else
            txpwr_ramp_down = txpwr[last_TX+ts_conv]; // the ramp down is the previous TX

         #if(RF_FAM == 61)
	    #if(DSP == 38) || (DSP == 39)
             Cust_get_ramp_tab(&(l1ps_dsp_com.pdsp_ndb_ptr->a_drp_ramp2_gprs[d_ramp_idx++][0]), txpwr_ramp_up, txpwr_ramp_down, radio_freq);
	    #endif
        #else
			#if (CODE_VERSION != SIMULATION)
             Cust_get_ramp_tab(&(l1ps_dsp_com.pdsp_ndb_ptr->a_ramp_gprs[d_ramp_idx++][0]), txpwr_ramp_up, txpwr_ramp_down, radio_freq);
         #endif
		  #endif
        }

        // memorize the last TX window
        last_TX = i;
      }
      else
      {
         // program an interrupt in the TS following
         // the last TX window and needed by the DSP

         // Is it the TS following a TX window ?
         if((i == last_TX+1) && (i<8))
         {
           // program the interrupt
           l1ps_dsp_com.pdsp_db_w_ptr->a_ctrl_abb_gprs[i] = (1 << B_MS_RULE);
         }
        }
      }

    // in a multi-TX case an additionnal ramp down must be set
    if(cpt_TX > 1)
    {
       // Control bitmap: update RAMP, use RAMP[d_ramp_idx][..] for slot i and set the interrupt
	   if((last_TX+1) <= 7)
       l1ps_dsp_com.pdsp_db_w_ptr->a_ctrl_abb_gprs[last_TX+1] = ((d_ramp_idx << B_RAMP_NB_GPRS) | (1 << B_RAMP_GPRS) | (1 << B_MS_RULE));

       // Store Ramp.
       // ==========
       txpwr_ramp_up   = txpwr[last_TX+ts_conv]; // this ramp up is unused (default: set to last_TX)
       txpwr_ramp_down = txpwr[last_TX+ts_conv]; // the  ramp down is the last TX

      #if(RF_FAM == 61)
        #if(DSP ==38) || (DSP == 39)
             Cust_get_ramp_tab(&(l1ps_dsp_com.pdsp_ndb_ptr->a_drp_ramp2_gprs[d_ramp_idx][0]), txpwr_ramp_up, txpwr_ramp_down, radio_freq);
	 #endif
      #else
		#if (CODE_VERSION != SIMULATION)
             Cust_get_ramp_tab(&(l1ps_dsp_com.pdsp_ndb_ptr->a_ramp_gprs[d_ramp_idx][0]), txpwr_ramp_up, txpwr_ramp_down, radio_freq);
		#endif
      #endif
    }
 //  #endif Locosto
}


/*-------------------------------------------------------*/
/* l1pddsp_ul_ptcch_data()                               */
/*-------------------------------------------------------*/
/* Parameters :                                          */
/* Return     :                                          */
/* Functionality :                                       */
/*-------------------------------------------------------*/
void l1pddsp_ul_ptcch_data(UWORD8   cs_type,
                           UWORD16  channel_request_data,
                           UWORD8   bsic,
                           UWORD16  radio_freq,
                           UWORD8   timeslot_no)
{
  UWORD16  swap_bit; // 16 bit wide to allow shift left.

  // UL on TS=timeslot_no.
  l1ps_dsp_com.pdsp_db_w_ptr->d_task_u_gprs |= 0x80 >> timeslot_no;

  // Swap I/Q management.
  swap_bit = l1ps_swap_iq_ul(radio_freq);
  l1ps_dsp_com.pdsp_db_w_ptr->d_task_u_gprs |= swap_bit << 15;

  // Set "b_ptcch_ul" to indicate PTCCH/UL to DSP.
  l1ps_dsp_com.pdsp_db_w_ptr->d_task_u_gprs |= (1 << B_PTCCH_UL);

  // Store CS type.
  l1ps_dsp_com.pdsp_ndb_ptr->a_ptcchu_gprs[0] = cs_type;

  // Store UL data block.
  if(cs_type == CS_PAB8_TYPE)
  {
    l1ps_dsp_com.pdsp_ndb_ptr->a_ptcchu_gprs[1] = ((API)(bsic << 2)) |
                                                  ((API)(channel_request_data) << 8);
    l1ps_dsp_com.pdsp_ndb_ptr->a_ptcchu_gprs[2] = 0;
  }
  else
  {
    l1ps_dsp_com.pdsp_ndb_ptr->a_ptcchu_gprs[1] = ((API)(channel_request_data) << 5);
    l1ps_dsp_com.pdsp_ndb_ptr->a_ptcchu_gprs[2] = ((API)(bsic << 10));
  }
}


/*-------------------------------------------------------*/
/* l1pddsp_interf_meas_ctrl()                            */
/*-------------------------------------------------------*/
/* Parameters :                                          */
/* Return     :                                          */
/* Functionality :                                       */
void l1pddsp_interf_meas_ctrl(UWORD8  nb_meas_req)
{
  // Interference measurement task set as a monitoring task within GSM interface.
  // 101 means 1 meas, 102 means 2 meas ...
  // Rem: swap I/Q is not managed for power measurements.
  l1s_dsp_com.dsp_db_w_ptr->d_task_md = INTERF_DSP_TASK + nb_meas_req;
}


/*-------------------------------------------------------*/
/* l1pddsp_transfer_meas_ctrl()                          */
/*-------------------------------------------------------*/
/* Parameters :                                          */
/* Return     :                                          */
/* Functionality :                                       */
/*-------------------------------------------------------*/
void l1pddsp_transfer_meas_ctrl(UWORD8  meas_position)
{
  // Store measurement position.
  // Rem: This is a L1S filtered information giving the position of the meas. as a
  // bitmap.
  // Rem: swap I/Q is not managed for power measurements.
  l1ps_dsp_com.pdsp_db_w_ptr->d_task_pm_gprs = meas_position;
}

/*-------------------------------------------------------*/
/* l1pddsp_meas_ctrl()                                   */
/*-------------------------------------------------------*/
/* Parameters :                                          */
/* Return     :                                          */
/* Functionality :                                       */
/*-------------------------------------------------------*/
void l1pddsp_meas_ctrl(UWORD8 nbmeas, UWORD8 pm_pos)
{
  // Request Signal level measurement task to DSP. A bit map is passed
  // to DSP in order to specify the position of the measurement.
  // Note: MSB is TN = 0 and LSB is TN = 7.
  // Rem: swap I/Q is not managed for power measurements.
  // Note: currently a maximum of four Pm can be performed / TDMA. This would
  // be modified in a near futur.
  // Note: If a Rx is programmed i.e. pm_pos = 1, only a maximum
  // of 3 Pm is requested to DSP and position of the Pm are  right shifted (Rx on TN = 0).
  // Remark: In packet Idle mode Rx are still on TN = 0. This implies three Pm
  // always after the Rx.
    l1ps_dsp_com.pdsp_db_w_ptr->d_task_pm_gprs = ((UWORD8) (0xff << (8 - nbmeas))) >> pm_pos;
}

/*-------------------------------------------------------*/
/* l1pddsp_meas_read()                                   */
/*-------------------------------------------------------*/
/* Parameters :                                          */
/* Return     :                                          */
/* Functionality :                                       */
/*-------------------------------------------------------*/
void l1pddsp_meas_read(UWORD8 nbmeas, UWORD16 *a_pm)
{
  UWORD8 i = 0;
  UWORD8 j;
  UWORD8 bit_mask = 0x80;

  // Looks for first PM position
  while ((i < 8) && (l1ps_dsp_com.pdsp_db_r_ptr->d_task_pm_gprs & bit_mask) == 0)
  {
    i++;
    bit_mask >>= 1;
  }

  // Read 'nbmeas' contiguous PM levels from the first PM position
  // Note: PM are always programmed on contiguous timeslots
  for (j = 0; ((j < nbmeas)&&(i < 8)); j++)
  {
    // Download PM from DSP/MCU memory interface
    a_pm[j] = ((l1ps_dsp_com.pdsp_db_r_ptr->a_burst_pm_gprs[i] & 0xffff));

    // Read next PM on following TN
    i++;
  }
}

/*-------------------------------------------------------*/
/* l1pddsp_load_bcchn_task()                             */
/*-------------------------------------------------------*/
/* Parameters :                                          */
/* Return     :                                          */
/* Functionality :                                       */
/*-------------------------------------------------------*/
void l1pddsp_load_bcchn_task(UWORD8 tsq,UWORD16 radio_freq )
{
  UWORD16 swap_bit = l1ps_swap_iq_dl(radio_freq);

  l1s_dsp_com.dsp_db_w_ptr->d_task_md      = NBN_DSP_TASK | (swap_bit << 15);  // Load BCCHN task
  l1s_dsp_com.dsp_db_w_ptr->d_ctrl_system |= tsq << B_TSQ;
}
#endif

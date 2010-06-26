/************* Revision Controle System Header *************
 *                  GSM Layer 1 software 
 * L1P_CTL.C
 *
 *        Filename l1p_ctl.c
 *  Copyright 2003 (C) Texas Instruments  
 *
 ************* Revision Controle System Header *************/

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
  #include "l1_mftab.h"
  #include "l1_tabs.h"
  #include "l1_ver.h"

  #include "l1_ctl.h"

  #include "l1p_cons.h"
  #include "l1p_msgt.h"
  #include "l1p_deft.h"
  #include "l1p_vare.h"
  #include "l1p_sign.h"
#if (OP_L1_STANDALONE == 1)
  #ifdef _INLINE
    #define INLINE static inline // Inline functions when -v option is set 
  #else                          // when the compiler is ivoked.
    #define INLINE
  #endif
#endif  //0maps00090550
#else
  #include <string.h>
  #include "l1_types.h"
  #include "sys_types.h"
  #include "l1_const.h"
  #include "l1_time.h"
  #include "l1_signa.h"

  #if (RF_FAM == 61)
      #include "tpudrv61.h"
  #endif 

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
  #include "cust_os.h"
  #include "l1_msgty.h"
  #include "l1_varex.h"
  #include "l1_proto.h"

  #include "l1_ctl.h"

  #include "l1p_cons.h"
  #include "l1p_msgt.h"
  #include "l1p_deft.h"
  #include "l1p_vare.h"
  #include "l1p_sign.h"
  #if (OP_L1_STANDALONE == 1)
  #ifdef _INLINE
    #define INLINE static inline // Inline functions when -v option is set 
  #else                          // when the compiler is ivoked.
    #define INLINE
  #endif
#endif //omaps00090550
#endif

#if(RF_FAM == 61)
   #include "l1_rf61.h"
#endif


// Macro definition
//-----------------
#define min(value1,value2) \
  value1 < value2 ? value1 : value2

// External prototypes
//--------------------

WORD8 l1ctl_encode_delta1(UWORD16 radio_freq);

/*********************************************************/
/* GPRS AGC Algorithms                                   */
/*********************************************************/

/*-------------------------------------------------------*/
/* l1pctl_pagc_ctrl()                                    */
/*-------------------------------------------------------*/
/* Description:                                          */
/* ===========                                           */
/* Based on the same principle as the one used for PAGC  */
/* algorithm except that we also feed the beacon FIFO    */
/* with IL measured on other carriers (Pb parameter is   */
/* applied)                                              */
/* This function is used in the control phase of PCCCH,  */
/* serving PBCCH and PTCCH reading tasks to determine    */
/* which AGC and lna_off must apply                      */
/*                                                       */
/* WARNING: in the layer 1 code, input levels IL(l1) use */
/*          format 7.1:                                  */
/*               *********************                   */
/*               * IL(l1) = - 2 x IL *                   */
/*               *********************                   */
/*          -> Reversed sign, reversed test conditions   */
/*          -> max replaced by min                       */
/*          ex: if IL -120 dBm, IL(l1) = 240             */
/*-------------------------------------------------------*/
void l1pctl_pagc_ctrl(WORD8 *agc, UWORD8 *lna_off, UWORD16 radio_freq, UWORD8 serving_cell)
{
  UWORD8  pb;
  WORD16  input_level, new_calibrated_IL;
  WORD32  freq_index;
  UWORD16 beacon_frequency;
  UWORD8  *lna_off_ptr;
  UWORD8  curve_id;

  // We memorize the LNA state used for other serving frequencies that can be used
  // in Packet idle mode
  static UWORD8 lna_off_others = 0;

  if (serving_cell == TRUE)
  {
    beacon_frequency = l1a_l1s_com.Scell_info.radio_freq;
    pb               = l1a_l1s_com.Scell_info.pb;
    lna_off_ptr      = &lna_off_others;
    curve_id         = MAX_ID;
  }
  else
  {
    beacon_frequency = l1pa_l1ps_com.pbcchn.bcch_carrier;
    pb               = l1pa_l1ps_com.pbcchn.pb;
    lna_off_ptr      = lna_off;
    curve_id         = AV_ID;
  }

#if(L1_FF_MULTIBAND == 0)
  freq_index = beacon_frequency - l1_config.std.radio_freq_index_offset;
#else
  freq_index = l1_multiband_radio_freq_convert_into_operative_radio_freq(beacon_frequency);
#endif
  

  // If the downlink channel is decoded on the beacon frequency
  if (radio_freq == beacon_frequency)
  {
    // Downlink task on the serving beacon: process AGC according to the "beacon IL"
    input_level = l1a_l1s_com.last_input_level[freq_index].input_level;

    // lna_off already processed in the read phase
    *lna_off = l1a_l1s_com.last_input_level[freq_index].lna_off;
  }

  // If the downlink channel is decoded on a frequency other than the beacon
  else
  {

    // Process AGC according to "beacon IL + Pb"
    input_level = (WORD16) (l1a_l1s_com.last_input_level[freq_index].input_level + pb);

    // IL_2_AGC_xx array size
    if (input_level>INDEX_MAX)
      input_level = INDEX_MAX;

    // lna_off must be processed in the control phase because input_level
    // depends on last_input_level and Pb, and last_input_level or pb can have changed
    // at any time

    // New calibrated IL to reach on radio freq other than beacon
    new_calibrated_IL = (WORD16) (input_level - l1ctl_encode_delta1(radio_freq)
                                              - l1ctl_encode_delta2(radio_freq));

    // IL_2_AGC_xx array size
    if (new_calibrated_IL>INDEX_MAX)
      new_calibrated_IL = INDEX_MAX;

    // lna_off computing...
    l1ctl_encode_lna((UWORD8)(new_calibrated_IL>>1),
                     lna_off_ptr,
                     radio_freq);

    *lna_off = *lna_off_ptr;
  } // End if "radio_freq != beacon_frequency"

  // Process AGC to apply
  *agc     = Cust_get_agc_from_IL(radio_freq,
                                  input_level >> 1,
                                  curve_id, *lna_off);

  // Store lna_off and input_level field used for current CTRL in order to be able 
  // to build IL from pm in READ phase.
  l1a_l1s_com.Scell_used_IL.input_level = (UWORD8)input_level;
  l1a_l1s_com.Scell_used_IL.lna_off     = *lna_off;

} // End of "l1pctl_pagc_ctrl"

/*-------------------------------------------------------*/
/* l1pctl_pagc_read()                                    */
/*-------------------------------------------------------*/
/* Description :                                         */
/* ===========                                           */
/* Based on the same principle as the one used for PAGC  */
/* algorithm except that we also feed the beacon FIFO    */
/* with IL measured on other carriers (Pb parameter is   */
/* applied)                                              */
/* This function is used in the read phase of PCCCH and  */
/* serving PBCCH reading tasks to determine the IL value */
/* store it in the FIFO and find the next IL to use      */
/*                                                       */
/* WARNING: in the layer 1 code, input levels IL(l1) use */
/*          format 7.1:                                  */
/*               *********************                   */
/*               * IL(l1) = - 2 x IL *                   */
/*               *********************                   */
/*          -> Reversed sign, reversed test conditions   */
/*          -> max replaced by min                       */
/*          ex: if IL -120 dBm, IL(l1) = 240             */
/*-------------------------------------------------------*/
UWORD8 l1pctl_pagc_read(UWORD8 pm, UWORD16 radio_freq)
{
  UWORD8   i, new_IL;
  WORD8    delta1_freq, delta2_freq;
  WORD16   delta_drp_gain=0;
  UWORD16  lna_value;
  WORD16   used_agc, current_IL, new_calibrated_IL, current_calibrated_IL;
  WORD32   serving_index;
  UWORD16  arfcn;
  UWORD8   lna_off;
  UWORD16  dco_algo_ctl_pw_temp = 0;
  UWORD8   if_ctl = 0;
  #if (CODE_VERSION != SIMULATION)
  UWORD8   if_threshold = C_IF_ZERO_LOW_THRESHOLD_GPRS;
#endif

#if (L1_FF_MULTIBAND == 0)
  serving_index = l1a_l1s_com.Scell_info.radio_freq - l1_config.std.radio_freq_index_offset;

#else
  serving_index = l1_multiband_radio_freq_convert_into_operative_radio_freq(l1a_l1s_com.Scell_info.radio_freq);
#endif /*if (L1_FF_MULTIBAND == 0)*/
  

  // Calibration factors
  delta1_freq = l1ctl_encode_delta1(radio_freq);
  delta2_freq = l1ctl_encode_delta2(radio_freq);

  // AGC used in the control phase (format F7.1)
  used_agc = (Cust_get_agc_from_IL(radio_freq, l1a_l1s_com.Scell_used_IL_dd.input_level >> 1, MAX_ID, l1a_l1s_com.Scell_used_IL_dd.lna_off)) << 1;

  // LNA attenuation
  lna_value = l1a_l1s_com.Scell_used_IL_dd.lna_off * l1ctl_get_lna_att(radio_freq);

  // DRP correction 
#if (L1_FF_MULTIBAND == 0)  
  arfcn = Convert_l1_radio_freq(radio_freq);
#else
  arfcn=radio_freq;
#endif 

#if(RF_FAM == 61)
   #if (CODE_VERSION != SIMULATION)

    cust_get_if_dco_ctl_algo(&dco_algo_ctl_pw_temp, &if_ctl, (UWORD8) L1_IL_VALID , 
                                          l1a_l1s_com.Scell_used_IL_dd.input_level,
                                         radio_freq,if_threshold);
     lna_off = l1a_l1s_com.Scell_used_IL_dd.lna_off;
     delta_drp_gain = drp_gain_correction(arfcn, lna_off, used_agc);    // F7.1 format
     if(if_ctl == IF_100KHZ_DSP){
       delta_drp_gain += SCF_ATTENUATION_LIF_100KHZ;
     }
     else{ /* i.e. if_ctl = IF_120KHZ_DSP*/
       delta_drp_gain += SCF_ATTENUATION_LIF_120KHZ;    
     }
       
   #endif
#endif

  // current_IL processing

  if (0==pm)  // Check and filter illegal pm value by using last valid IL
  {
    current_IL = l1a_l1s_com.last_input_level[serving_index].input_level;
  }
  else 
  { 
    current_IL = -(pm - (used_agc - delta_drp_gain) + lna_value - l1ctl_get_g_magic(radio_freq));

    // IL normalization to beacon (ILnorm = IL - Pb)
    if (radio_freq != l1a_l1s_com.Scell_info.radio_freq)
      current_IL -= l1a_l1s_com.Scell_info.pb;
  }

  // Calibrated IL processing
  // NOTE: calibrated_IL is normalized to beacon. This is needed for the
  //       pccch_lev processing
  current_calibrated_IL = (WORD16) (current_IL - delta1_freq - delta2_freq);
  
  // Protect IL stores against overflow
  if (current_calibrated_IL>INDEX_MAX)
    current_calibrated_IL=INDEX_MAX;
  if (current_IL>INDEX_MAX)
    current_IL=INDEX_MAX;

  // FIFO management
  for (i=3;i>0;i--)
    l1a_l1s_com.Scell_info.buff_beacon[i] = l1a_l1s_com.Scell_info.buff_beacon[i-1];
  l1a_l1s_com.Scell_info.buff_beacon[0] = (UWORD8)current_IL;

  // Find min IL in FIFO
  new_IL = l1ctl_find_max(l1a_l1s_com.Scell_info.buff_beacon, 4);

  // Input levels are always stored with lna_on 
  new_calibrated_IL = (WORD16)  (new_IL - delta1_freq - delta2_freq);

  if (new_calibrated_IL>INDEX_MAX) new_calibrated_IL = INDEX_MAX;

  l1ctl_encode_lna( (UWORD8)(new_calibrated_IL>>1),
                    &(l1a_l1s_com.last_input_level[serving_index].lna_off),
                    radio_freq );

  l1a_l1s_com.last_input_level[serving_index].input_level =  new_IL;
  return((UWORD8)current_calibrated_IL);
} // End of "l1pctl_pagc_read"

/*-------------------------------------------------------*/
/* l1pctl_transfer_agc_init()                            */
/*-------------------------------------------------------*/
/* Description :                                         */
/* ===========                                           */
/* Packet transfer AGC algorithm initialization          */
/*-------------------------------------------------------*/
void l1pctl_transfer_agc_init()
{
  WORD16   calibrated_IL;
  UWORD16  radio_freq;
  WORD32   serving_index;
  WORD16   input_level;

#if (L1_FF_MULTIBAND ==0)
  serving_index = l1a_l1s_com.Scell_info.radio_freq - l1_config.std.radio_freq_index_offset;

#else
  serving_index = l1_multiband_radio_freq_convert_into_operative_radio_freq(l1a_l1s_com.Scell_info.radio_freq);
#endif /*if L1_FF_MULTIBAND*/
  

  // Daughter frequencies input level initialization
  //------------------------------------------------
  if (l1pa_l1ps_com.transfer.aset->dl_pwr_ctl.p0 == 255)
  {
    // No power control mode AGC algorithm
    input_level = (WORD16) (l1a_l1s_com.last_input_level[serving_index].input_level + l1a_l1s_com.Scell_info.pb);

    // Set fn_select to current_fn
    l1ps.fn_select                                   = l1s.actual_time.fn;
    // Initialize algorithm in "SEARCH" phase
    l1ps.phase                                       = SEARCH;
  }
  else
  {
    // Downlink power control AGC algorithms
    if (l1pa_l1ps_com.transfer.aset->dl_pwr_ctl.bts_pwr_ctl_mode == 0)
    {
      // BTS Power control mode A
      input_level = (WORD16) (l1a_l1s_com.last_input_level[serving_index].input_level +
                              l1pa_l1ps_com.transfer.aset->dl_pwr_ctl.p0 + 10);
      
    }
    else
    {
      // BTS power control mode B
      input_level = (WORD16) (l1a_l1s_com.last_input_level[serving_index].input_level +
                              l1pa_l1ps_com.transfer.aset->dl_pwr_ctl.p0);

      // Initialization: PR = P0
      l1ps.last_PR_good = l1pa_l1ps_com.transfer.aset->dl_pwr_ctl.p0;
    }
  }

  if (input_level>INDEX_MAX) input_level = INDEX_MAX;
  l1a_l1s_com.Scell_info.transfer_meas.input_level = (UWORD8)input_level;


  // Daughter frequencies lna_off processing
  //----------------------------------------

  // We need to know on which frequency band we work
  if (!l1pa_l1ps_com.transfer.aset->freq_param.chan_sel.h)
  {
    // Single frequency
    radio_freq = l1pa_l1ps_com.transfer.aset->freq_param.chan_sel.rf_channel.single_rf.radio_freq;
  }
  else
  {
    // Frequency hopping: all frequencies of the frequency list are on the same band
    // We take the first frequency of the list
    radio_freq = l1pa_l1ps_com.transfer.aset->freq_param.freq_list.rf_chan_no.A[0];
  }

  calibrated_IL = (WORD16) (l1a_l1s_com.Scell_info.transfer_meas.input_level
                  - l1ctl_encode_delta1(radio_freq) - l1ctl_encode_delta2(radio_freq)
                  - l1a_l1s_com.last_input_level[serving_index].lna_off * l1ctl_get_lna_att(radio_freq));
                  
  if (calibrated_IL>INDEX_MAX) calibrated_IL = INDEX_MAX; 

  l1ctl_encode_lna((UWORD8)(calibrated_IL>>1), &(l1a_l1s_com.Scell_info.transfer_meas.lna_off), radio_freq);

}  // End of "l1pctl_transfer_agc_init"

/*-------------------------------------------------------*/
/* l1pctl_transfer_agc_ctrl()                            */
/*-------------------------------------------------------*/
/* Description :                                         */
/* ===========                                           */
/* This function is used in the control phase of PDTCH/D */
/* to determine which AGC and lna_off must apply         */
/*-------------------------------------------------------*/
void l1pctl_transfer_agc_ctrl(WORD8 *agc, UWORD8 *lna_off, UWORD16 radio_freq)
{
  T_INPUT_LEVEL *selected_IL;
#if(L1_FF_MULTIBAND == 1)
  UWORD16 operative_radio_freq;
#endif
  

  // input_level selection
  if (radio_freq == l1a_l1s_com.Scell_info.radio_freq)
  {
    // Beacon frequency input_level used for AGC processing
#if(L1_FF_MULTIBAND == 0)    
    selected_IL = &l1a_l1s_com.last_input_level[l1a_l1s_com.Scell_info.radio_freq
                                                - l1_config.std.radio_freq_index_offset];

#else // L1_FF_MULTIBAND = 1 below

    operative_radio_freq = 
      l1_multiband_radio_freq_convert_into_operative_radio_freq(l1a_l1s_com.Scell_info.radio_freq);
    selected_IL = &(l1a_l1s_com.last_input_level[operative_radio_freq]);


#endif // #if(L1_FF_MULTIBAND == 0) else

  }
  else
  {
    // Daughter frequency input_level used for AGC processing
    selected_IL = &l1a_l1s_com.Scell_info.transfer_meas;
  }
  *lna_off = selected_IL->lna_off;
  *agc     = Cust_get_agc_from_IL(radio_freq,selected_IL->input_level >> 1, MAX_ID, *lna_off);

  // Store lna_off and input_level field used for current CTRL in order to be able 
  // to build IL from pm in READ phase.
  l1a_l1s_com.Scell_used_IL.input_level = selected_IL->input_level;
  l1a_l1s_com.Scell_used_IL.lna_off     = *lna_off;

}

/*-------------------------------------------------------*/
/* l1pctl_npc_agc_read()                                 */
/*-------------------------------------------------------*/
/* Description :                                         */
/* ===========                                           */
/* AGC algorithm in packet transfer used when            */
/* NO POWER CONTROL is done by the BTS.                  */
/* This function is used during the read phase of PDTCH: */
/* 1- to determine the IL value for each timeslot in each*/
/*    TDMA                                               */
/* 2- to find the IL value to use for the next PDCH      */
/*    block                                              */
/*                                                       */
/* Algorithm                                             */
/* ---------                                             */
/* For each timeslot i used for PDCH                     */
/*   IL(i) = fct(used AGC, pm)                           */
/*   if (beacon)                                         */
/*     ILmax_beacon = max(ILmax_beacon, IL(i))           */
/*   else                                                */
/*     ILmax_others(i) = max(IL(i), ILmax_others(i))     */
/*                                                       */
/* If (burst_nb == 3)                                    */
/*   If (ILmax_beacon was found during the block)        */
/*     FIFO[beacon] updated with ILmax_beacon            */
/*     transfer_meas = max(FIFO[beacon])                 */
/*     Reset ILmax_beacon                                */
/*                                                       */
/*   For each timeslot i used for PDCH                   */
/*     if (CRC good)                                     */
/*       ILmax_correct = max(ILmax_correct,              */
/*                           ILmax_others(i))            */
/*     else                                              */
/*       ILmax_not_correct = max(ILmax_not_correct,      */
/*                               ILmax_others(i))        */
/*                                                       */
/*   If (no ILmax_correct was found)                     */
/*     ILselected = ILmax_correct                        */
/*     FNselected = current FN                           */
/*   else                                                */
/*     DeltaFN = (current FN - FNselected) % MAX_FN      */
/*                                                       */
/*     if (DeltaFN < 78)                                 */
/*       ILweighted = ILselected * (1 - DeltaFN/78)      */
/*                    - 120 * DeltaFN /78                */
/*                                                       */
/*       if (ILweighted < -120) ILweighted = -120        */
/*       if (ILmax_not_correct > ILweighted)             */
/*         ILselected = ILmax_not_correct                */
/*         FNselected = current FN                       */
/*     else                                              */
/*       ILselected = -120                               */
/*       FNselected = current FN                         */
/*                                                       */
/*   Reset ILmax_others[8]                               */
/*                                                       */
/* WARNING: in the layer 1 code, input levels IL(l1) use */
/*          format 7.1:                                  */
/*               *********************                   */
/*               * IL(l1) = - 2 x IL *                   */
/*               *********************                   */
/*          -> Reversed sign, reversed test conditions   */
/*          -> max replaced by min                       */
/*          ex: if IL -120 dBm, IL(l1) = 240             */
/*                                                       */
/* Parameters                                            */
/* ----------                                            */
/* "calibrated_IL[8]"                                    */
/*   contains the IL found on timeslots                  */
/*   used for PDCH/D. These ILs can be used to process   */
/*   RXLEV values.                                       */
/*                                                       */
/* "*pdsp_db_r_ptr"                                      */
/*   Pointer on the DSP DB Read page, used to extract    */
/*   pm values, burst number and timeslot allocated      */
/*   for downlink PDCH                                   */
/*                                                       */
/* "*pdsp_ndb_ptr"                                       */
/*   Pointer on the DSP NDB page, used to extract the    */
/*   CRC value for each decoded burst                    */
/*                                                       */
/* Global parameters                                     */
/* -----------------                                     */
/* "l1a_l1s_com.Scell_info.transfer_meas.input_level"    */
/* "l1a_l1s_com.Scell_info.transfer_meas.lna_off"        */
/*   Used to store the ILselected and the associated     */
/*   lna_off value.                                      */
/*                                                       */
/* "l1a_l1s_com.Scell_info.fn_select"                    */
/*   Used to store the FNselected value.                 */
/*                                                       */
/* "l1a_l1s_com.last_input_level[freq index]             */
/*     .input_level                                      */
/*     .lna_off"                                         */
/*   Used to store the beacon input level and            */
/*   the associated lna_off value.                       */
/*                                                       */
/* "l1ps.transfer_beacon_buf[4]"                         */
/*   FIFO[beacon]                                        */
/*                                                       */
/* "l1ps.ILmin_beacon"                                   */
/* "l1ps.ILmin_others[8]"                                */
/*-------------------------------------------------------*/
void l1pctl_npc_agc_read(UWORD8 calibrated_IL[8], T_DB_DSP_TO_MCU_GPRS *pdsp_db_r_ptr,
                         T_NDB_MCU_DSP_GPRS *pdsp_ndb_ptr)
{
  UWORD8       ts;
  UWORD8       rx_no                      = 0;
  UWORD8       bit_mask                   = 0x80;
  UWORD8       ILmin_correct              = 255;
  UWORD8       ILmin_not_correct          = 255;
  WORD8        delta1_freq, delta2_freq;
  WORD16       delta_drp_gain=0;
  UWORD16      radio_freq, lna_value;
  WORD16       used_agc;
  WORD32       serving_index;
  UWORD16      arfcn;
  UWORD8       lna_off;
  UWORD16 dco_algo_ctl_pw_temp = 0;
  UWORD8 if_ctl = 0;
  #if (CODE_VERSION != SIMULATION)
  UWORD8 if_threshold = C_IF_ZERO_LOW_THRESHOLD_GPRS;
#endif

#if(L1_FF_MULTIBAND == 0)
  serving_index = l1a_l1s_com.Scell_info.radio_freq - l1_config.std.radio_freq_index_offset;

#else
  serving_index = l1_multiband_radio_freq_convert_into_operative_radio_freq(l1a_l1s_com.Scell_info.radio_freq);
#endif
  

  // Control phase parameters: same AGC, radio_freq, lna_off used for all PDTCH
  // ***************************************************************************

  // Get radio_freq on which the downlink block was received
  radio_freq = l1a_l1s_com.dedic_set.radio_freq_dd;

  // Compute calibration factors
  delta1_freq                                      = l1ctl_encode_delta1(radio_freq);
  delta2_freq                                      = l1ctl_encode_delta2(radio_freq);

  // AGC used in the control phase (format F7.1)
  used_agc = (Cust_get_agc_from_IL(radio_freq, l1a_l1s_com.Scell_used_IL_dd.input_level >> 1, MAX_ID, l1a_l1s_com.Scell_used_IL_dd.lna_off)) << 1;

  // LNA attenuation
  lna_value = l1a_l1s_com.Scell_used_IL_dd.lna_off * l1ctl_get_lna_att(radio_freq);

  // Burst 0: Reset ILmin_beacon and ILmin_others
  if(pdsp_db_r_ptr->d_burst_nb_gprs == 0)
  {
    l1ps.ILmin_beacon = 255; // Not valid

    for (ts = 0; ts < 8; ts++)
    {
      l1ps.ILmin_others[ts] = (UWORD8) l1_config.params.il_min;
    }
  } // End if "burst 0"

  // IL processing for each received burst
  // **************************************

  // For each timeslot on which a burst was received
  for(ts = 0; ts < 8; ts ++)
  {
    if(pdsp_db_r_ptr->d_task_d_gprs & bit_mask)
    {
      WORD16       current_IL, current_calibrated_IL;
      UWORD8       pm;

      // IL = fct(pm, last_known_agc, lna_value, g_magic)
      //-------------------------------------------------

      pm    = (UWORD8) ((pdsp_db_r_ptr->a_burst_pm_gprs[ts]   & 0xffff) >> 5);

      // current_IL processing
      if (0==pm)  // Check and filter illegal pm value by using last valid IL
      {
        if (radio_freq == l1a_l1s_com.Scell_info.radio_freq)
          current_IL = l1a_l1s_com.last_input_level[serving_index].input_level;
        else
          current_IL = l1a_l1s_com.Scell_info.transfer_meas.input_level;
      }
      else

	  {
        // DRP correction 
#if (L1_FF_MULTIBAND == 0)  
  arfcn = Convert_l1_radio_freq(radio_freq);
#else
  arfcn=radio_freq;
#endif 

#if(RF_FAM == 61)
   #if (CODE_VERSION != SIMULATION)

    cust_get_if_dco_ctl_algo(&dco_algo_ctl_pw_temp, &if_ctl, (UWORD8) L1_IL_VALID , 
                                         l1a_l1s_com.Scell_used_IL_dd.input_level,
                                         radio_freq,if_threshold);
     lna_off = l1a_l1s_com.Scell_used_IL_dd.lna_off;
     delta_drp_gain = drp_gain_correction(arfcn, lna_off, used_agc);    // F7.1 format     
     if(if_ctl == IF_100KHZ_DSP){
       delta_drp_gain += SCF_ATTENUATION_LIF_100KHZ;
     }
     else{ /* i.e. if_ctl = IF_120KHZ_DSP*/
       delta_drp_gain += SCF_ATTENUATION_LIF_120KHZ;    
     }

   #endif
#endif

        current_IL = -(pm - (used_agc - delta_drp_gain) + lna_value - l1ctl_get_g_magic(radio_freq));
	  }

      // Calibrated IL processing
      current_calibrated_IL = current_IL - delta1_freq - delta2_freq;
      
      // Protect IL stores against overflow
      if(current_calibrated_IL>INDEX_MAX)
        current_calibrated_IL=INDEX_MAX;
      if (current_IL>INDEX_MAX)
        current_IL=INDEX_MAX;
         
      calibrated_IL[ts] = (UWORD8)(current_calibrated_IL);  
   
      // Keep ILmax
      if (radio_freq == l1a_l1s_com.Scell_info.radio_freq)
      {
        // Beacon frequency
        l1ps.ILmin_beacon = min((UWORD8) current_IL, l1ps.ILmin_beacon);
      }
      else
      {
        // Daughter frequency
        l1ps.ILmin_others[ts] = min((UWORD8) current_IL,l1ps.ILmin_others[ts]);
      }

      // Input Level selection among ILmax found on each timeslot during the block (when burst = 3)
      // *******************************************************************************************

      if(pdsp_db_r_ptr->d_burst_nb_gprs == 3)
      {
        // If CRC good
        if (!(pdsp_ndb_ptr->a_dd_gprs[rx_no][0] & 0x0100))
        {
          // Find the min found IL for blocks that were correctly received
          ILmin_correct = min(l1ps.ILmin_others[ts],ILmin_correct);
        }
        // If CRC bad
        else
        {
          // Find the min found IL for blocks that were not correctly received
          ILmin_not_correct = min(l1ps.ILmin_others[ts],ILmin_not_correct);
        }
      } // End if "burst = 3"

      // Next downlink block
      rx_no ++;

    } // End if "timeslot used for downlink PDCH"

    // Next timeslot
    bit_mask >>= 1;
  } // End for "each timeslot...."


  // IL selection for the next block if burst = 3
  // **********************************************

  if(pdsp_db_r_ptr->d_burst_nb_gprs == 3)
  {
    WORD16 new_calibrated_IL;

    // Beacon frequency input level updating
    //--------------------------------------

    // If a PDCH has been received on the beacon
    if (l1ps.ILmin_beacon != 255)
    {
      UWORD8 i, new_IL;

      // FIFO management
      for (i=3;i>0;i--)
        l1a_l1s_com.Scell_info.buff_beacon[i] = l1a_l1s_com.Scell_info.buff_beacon[i-1];
      l1a_l1s_com.Scell_info.buff_beacon[0] = l1ps.ILmin_beacon;

      // Find min IL in FIFO
      new_IL = l1ctl_find_max(l1a_l1s_com.Scell_info.buff_beacon, 4);

      // Input levels are always stored with lna_on

      // lna_off processing
      new_calibrated_IL = (WORD16) (new_IL - l1ctl_encode_delta1(l1a_l1s_com.Scell_info.radio_freq) - l1ctl_encode_delta2(l1a_l1s_com.Scell_info.radio_freq));

      if (new_calibrated_IL>INDEX_MAX) new_calibrated_IL = INDEX_MAX;

      l1ctl_encode_lna((UWORD8)(new_calibrated_IL>>1),
                       &(l1a_l1s_com.last_input_level[serving_index].lna_off),
                       l1a_l1s_com.Scell_info.radio_freq);

      l1a_l1s_com.last_input_level[serving_index].input_level = new_IL ;

    } // End of "beacon frequency input level updating"

    // Daughter frequencies input level updating
    //------------------------------------------

    // If at least one block was correctly received
    // (Note: ILs truncated to 240 so 255 isn't valid)
    if (ILmin_correct != 255)
    {
      // Select the min input level found on correctly received blocks
      l1a_l1s_com.Scell_info.transfer_meas.input_level = ILmin_correct;
      l1ps.fn_select                                   = l1s.actual_time.fn;

      // Algorithm switch to "TRACK" phase if it was in "SEARCH" phase
      l1ps.phase                                       = TRACK;
    }

    // No block was correctly received
    else
    {
      UWORD8 input_level_ref = l1a_l1s_com.Scell_info.transfer_meas.input_level;
      // SEARCH phase
      if (l1ps.phase == SEARCH)
      {
        // If measured level superior to currently tracket level, switch to TRACK mode
        if (input_level_ref > ILmin_not_correct)
          l1ps.phase = TRACK;

        // Select the min input level found on badly received blocks
        l1a_l1s_com.Scell_info.transfer_meas.input_level = ILmin_not_correct;
        l1ps.fn_select                                   = l1s.actual_time.fn;
      }

      // TRACK phase
      else
      {
        // If the IL found on incorrect block is lower than current wanted IL
        if (ILmin_not_correct < input_level_ref)
        {
          // Select the new IL
          l1a_l1s_com.Scell_info.transfer_meas.input_level = ILmin_not_correct;
          l1ps.fn_select = l1s.actual_time.fn;
        }

        // If the IL found on incorrect block is higher than current wanted IL
        else
        {
          UWORD32 delta_fn;

          // delta_fn processing for IL selection forgetting factor
          delta_fn = l1s.actual_time.fn - l1ps.fn_select;

          // MAX_FN modulo management
          if (l1s.actual_time.fn < l1ps.fn_select)
            delta_fn += MAX_FN;

          // If the last selected IL is more recent than 72 frames
          // 
          //  |....|R...............................C|....| 
          //        ^                               ^
          //     fn_selected                    IL reset to -120
          //  <-------------------------------------->
          //                    312
          // 306 = 312 - 4 (block_size) - 1 (Read phase fn delay) - 1 (Control phase fn advance)
          if (delta_fn > 306)
          {
            WORD16 input_level;

            // IL initialized to "beacon level - Pb"
            input_level = (WORD16) (l1a_l1s_com.last_input_level[serving_index].input_level + l1a_l1s_com.Scell_info.pb);
                                   

            if (input_level>INDEX_MAX) input_level = INDEX_MAX;    
            l1a_l1s_com.Scell_info.transfer_meas.input_level = (UWORD8)input_level;
            
            l1ps.fn_select                                   = l1s.actual_time.fn;
            // Returns to "SEARCH" phase
            l1ps.phase                                       = SEARCH;
          }
          else
          {
            WORD16 input_level;

            input_level = l1a_l1s_com.Scell_info.transfer_meas.input_level;

            if (input_level>INDEX_MAX) input_level = INDEX_MAX;

            l1a_l1s_com.Scell_info.transfer_meas.input_level = (UWORD8)input_level;
          }
        } // End if the IL found on incorrect block is higher than current wanted IL 
      } // End of "track phase"
    } // End if no block correctly received

    // lna_off processing
    new_calibrated_IL = (WORD16) (l1a_l1s_com.Scell_info.transfer_meas.input_level - l1ctl_encode_delta1(l1ps.read_param.radio_freq_for_lna));
    if (new_calibrated_IL>INDEX_MAX) new_calibrated_IL = INDEX_MAX;

    l1ctl_encode_lna((UWORD8)(new_calibrated_IL>>1), &(l1a_l1s_com.Scell_info.transfer_meas.lna_off), l1ps.read_param.radio_freq_for_lna);
  } // End if "burst = 3"

} // End of "l1pctl_npc_agc_read"

/*-------------------------------------------------------*/
/* l1pctl_dpcma_agc_read()                               */
/*-------------------------------------------------------*/
/* Description :                                         */
/* ===========                                           */
/* AGC algorithm in packet transfer used when the BTS    */
/* use DOWNLINK POWER CONTROL mode A.                    */
/* This function is used during the read phase of PDTCH: */
/* 1- to determine the IL value for each timeslot in each*/
/*    TDMA                                               */
/* 2- to find the IL value to use for the next PDCH      */
/*    block                                              */
/*                                                       */
/* Algorithm                                             */
/* ---------                                             */
/* For each timeslot i used for PDCH                     */
/*   IL(i) = fct(used AGC, pm)                           */
/*   if (beacon)                                         */
/*     ILmax_beacon = max(ILmax_beacon, IL(i))           */
/*   else                                                */
/*     ILmax_others(i) = max(IL(i), ILmax_others(i))     */
/*                                                       */
/* If (burst_nb == 3)                                    */
/*                                                       */
/*   For each timeslot i used for PDCH                   */
/*     if (CRC good) and                                 */
/*        ((PR_MODE A) or (PR_MODE B and TFI good))      */
/*       ILmax = max(ILmax, ILmax_others(i) + P0 + PR(i))*/
/*                                                       */
/*   ILmax=max(ILmax, ILmax_beacon)                      */
/*   FIFO[beacon] updated with ILmax                     */
/*   last_input_level[serving beacon] = max(FIFO[beacon])*/
/*   transfer_meas = max(FIFO[beacon]) - P0 - 5          */
/*                                                       */
/*   Reset ILmax_others[8] and ILmax_beacon              */
/*                                                       */
/* WARNING: in the layer 1 code, input levels IL(l1) use */
/*          format 7.1:                                  */
/*               *********************                   */
/*               * IL(l1) = - 2 x IL *                   */
/*               *********************                   */
/*          -> Reversed sign, reversed test conditions   */
/*          -> max replaced by min                       */
/*          ex: if IL -120 dBm, IL(l1) = 240             */
/*                                                       */
/* Parameters                                            */
/* ----------                                            */
/* "calibrated_IL[8]"                                    */
/*   contains the IL found on timeslots                  */
/*   used for PDCH/D. These ILs can be used to process   */
/*   RXLEV values.                                       */
/*                                                       */
/* "*pdsp_db_r_ptr"                                      */
/*   Pointer on the DSP DB Read page, used to extract    */
/*   pm values, burst number and timeslot allocated      */
/*   for downlink PDCH                                   */
/*                                                       */
/* "*pdsp_ndb_ptr"                                       */
/*   Pointer on the DSP NDB page, used to extract the    */
/*   CRC value for each decoded burst                    */
/*                                                       */
/* Global parameters                                     */
/* -----------------                                     */
/* "l1a_l1s_com.Scell_info.transfer_meas.input_level"    */
/* "l1a_l1s_com.Scell_info.transfer_meas.lna_off"        */
/*   Used to store the ILselected and the associated     */
/*   lna_off value.                                      */
/*                                                       */
/* "l1a_l1s_com.last_input_level[freq. index]            */
/*     .input_level                                      */
/*     .lna_off"                                         */
/*   Used to store the beacon input level and            */
/*   the associated lna_off value.                       */
/*                                                       */
/* "l1ps.transfer_beacon_buf[4]"                         */
/*   FIFO[beacon]                                        */
/*                                                       */
/* "l1ps.ILmin_beacon"                                   */
/* "l1ps.ILmin_others[8]"                                */
/*-------------------------------------------------------*/
void l1pctl_dpcma_agc_read(UWORD8 calibrated_IL[8], T_DB_DSP_TO_MCU_GPRS *pdsp_db_r_ptr,
                           T_NDB_MCU_DSP_GPRS *pdsp_ndb_ptr, UWORD8 pr_table[8])
{
  UWORD8       ts                         = 0;
  UWORD8       rx_no                      = 0;
  UWORD8       bit_mask                   = 0x80;
  UWORD8       IL_norm_min                = 255;
  WORD8        delta1_freq, delta2_freq;
  WORD16       delta_drp_gain=0;
  UWORD16      radio_freq, lna_value;
  WORD16       used_agc;
  WORD32       serving_index;
  UWORD16      arfcn;
  UWORD8       lna_off;
  UWORD16 dco_algo_ctl_pw_temp = 0;
  UWORD8 if_ctl = 0;
  #if (CODE_VERSION != SIMULATION)
  UWORD8 if_threshold = C_IF_ZERO_LOW_THRESHOLD_GPRS;
#endif

#if(L1_FF_MULTIBAND == 0)
  serving_index = l1a_l1s_com.Scell_info.radio_freq - l1_config.std.radio_freq_index_offset;

#else
  serving_index = l1_multiband_radio_freq_convert_into_operative_radio_freq(l1a_l1s_com.Scell_info.radio_freq);
#endif


  // Control phase parameters: same AGC, radio_freq, lna_off used for all PDTCH
  // ***************************************************************************

  // Get radio_freq on which the downlink block was received
  radio_freq     = l1a_l1s_com.dedic_set.radio_freq_dd;

  // Compute calibration factors
  delta1_freq = l1ctl_encode_delta1(radio_freq);
  delta2_freq = l1ctl_encode_delta2(radio_freq);

  // Last known AGC (format F7.1)
  used_agc = (Cust_get_agc_from_IL(radio_freq, l1a_l1s_com.Scell_used_IL_dd.input_level >> 1, MAX_ID,l1a_l1s_com.Scell_used_IL_dd.lna_off)) << 1;

  // LNA attenuation
  lna_value = l1a_l1s_com.Scell_used_IL_dd.lna_off * l1ctl_get_lna_att(radio_freq);

  // Burst 0: Reset ILmin_beacon and ILmin_others
  if(pdsp_db_r_ptr->d_burst_nb_gprs == 0)
  {
    l1ps.ILmin_beacon = 255;  // Not valid

    for (ts = 0; ts < 8; ts++)
    {
       l1ps.ILmin_others[ts] = 255;  // Not valid
    }
  }

  // IL processing for each received burst
  // **************************************

  // DRP correction
#if (L1_FF_MULTIBAND == 0)  
  arfcn = Convert_l1_radio_freq(radio_freq);
#else
  arfcn=radio_freq;
#endif 

#if(RF_FAM == 61)
   #if (CODE_VERSION != SIMULATION)
   
    cust_get_if_dco_ctl_algo(&dco_algo_ctl_pw_temp, &if_ctl, (UWORD8) L1_IL_VALID , 
                                         l1a_l1s_com.Scell_used_IL_dd.input_level,
                                         radio_freq, if_threshold);
     lna_off = l1a_l1s_com.Scell_used_IL_dd.lna_off;
     delta_drp_gain = drp_gain_correction(arfcn, lna_off, used_agc);    // F7.1 format

     if(if_ctl == IF_100KHZ_DSP){
       delta_drp_gain += SCF_ATTENUATION_LIF_100KHZ;
     }
     else{ /* i.e. if_ctl = IF_120KHZ_DSP*/
       delta_drp_gain += SCF_ATTENUATION_LIF_120KHZ;    
     }

   #endif
#endif
	 
  // For each timeslot on which a burst was received
  for(ts = 0; ts < 8; ts ++)
  {
    if(pdsp_db_r_ptr->d_task_d_gprs & bit_mask)
    {
      WORD16       current_IL, current_calibrated_IL;
      UWORD8       pm;

      // IL = fct(pm, last_known_agc, lna_value, g_magic)
      //-------------------------------------------------

      pm    = (UWORD8) ((pdsp_db_r_ptr->a_burst_pm_gprs[ts]   & 0xffff) >> 5);

      // current_IL processing
      if (0==pm)  // Check and filter illegal pm value by using last valid IL
      {
          current_IL = l1a_l1s_com.last_input_level[serving_index].input_level;

          if (radio_freq != l1a_l1s_com.Scell_info.radio_freq)
            current_IL += (l1ps.read_param.dl_pwr_ctl.p0 + 10);
      }
      else
	  {		  
        current_IL = -(pm - (used_agc - delta_drp_gain) + lna_value - l1ctl_get_g_magic(radio_freq));
	  }

      // Calibrated IL processing
      current_calibrated_IL = current_IL - delta1_freq - delta2_freq;
      
      // Protect IL stores against overflow
      if(current_calibrated_IL>INDEX_MAX)
        current_calibrated_IL=INDEX_MAX;
      if (current_IL>INDEX_MAX)
        current_IL=INDEX_MAX;
         
      calibrated_IL[ts] = (UWORD8)(current_calibrated_IL);  

      // Keep the minimum IL
      if (radio_freq == l1a_l1s_com.Scell_info.radio_freq)
      {
        // Beacon frequency
        l1ps.ILmin_beacon     = min((UWORD8) current_IL,l1ps.ILmin_beacon);
      }
      else
      {
        // Daughter frequency
        l1ps.ILmin_others[ts] = min((UWORD8) current_IL, l1ps.ILmin_others[ts]);
      }

      // Input Level selection among ILmax found on each timeslot during the block (when burst = 3)
      //-------------------------------------------------------------------------------------------

      if(pdsp_db_r_ptr->d_burst_nb_gprs == 3)
      {
        // If CRC good
        if (!(pdsp_ndb_ptr->a_dd_gprs[rx_no][0] & 0x0100))
        {
          // If ((PR_MODE A and TFI good) or (PR_MODE B)) AND PR != 0 [Not usable])
          if (((l1ps.read_param.dl_pwr_ctl.pr_mode != 0) || (!(pr_table[ts] & 0x80)))
              && ((pr_table[ts] & 0x1f) != 0))
          {
            if (l1ps.ILmin_others[ts] != 255)
            {
              UWORD8 IL_norm;

              // IL normalization to beacon (ILnorm = ILmax_others(ts) - P0 - PR)
              IL_norm = l1ps.ILmin_others[ts] - l1ps.read_param.dl_pwr_ctl.p0 - ((pr_table[ts] & 0x1f) << 1);

              // Update IL_min with the minimum found IL
              IL_norm_min = min(IL_norm, IL_norm_min);
            }
          }
        } // End if "CRC good"

      } // End if "burst = 3"

      // Next downlink block
      rx_no ++;

    } // End if "timeslot used for downlink PDCH"

    // Next timeslot
    bit_mask >>= 1;

  } // End for "each timeslot...."


  // IL selection for the next block if burst = 3
  // **********************************************

  if(pdsp_db_r_ptr->d_burst_nb_gprs == 3)
  {
    UWORD8  i, new_IL;
    UWORD16 input_level;
    WORD16  new_calibrated_IL;

    // Select the minimum IL between minimum IL found on daughter frequencies (normalized to beacon)
    // and minimum IL found on the beacon
    IL_norm_min = min(IL_norm_min, l1ps.ILmin_beacon);

    // If a valid IL has been found
    if (IL_norm_min != 255)
    {
      // FIFO management
      for (i=3;i>0;i--)
        l1a_l1s_com.Scell_info.buff_beacon[i] = l1a_l1s_com.Scell_info.buff_beacon[i-1];

      l1a_l1s_com.Scell_info.buff_beacon[0] = IL_norm_min;

      // last_input_level[serving beacon] updating
      //------------------------------------------

      // Find min IL in FIFO
      new_IL = l1ctl_find_max(l1a_l1s_com.Scell_info.buff_beacon,4);
  
      // Input levels are always stored with lna_on

      // lna_off processing
      new_calibrated_IL = (WORD16) (new_IL - l1ctl_encode_delta1(l1a_l1s_com.Scell_info.radio_freq) - l1ctl_encode_delta2(l1a_l1s_com.Scell_info.radio_freq));
      if (new_calibrated_IL>INDEX_MAX) new_calibrated_IL = INDEX_MAX;

      l1ctl_encode_lna((UWORD8)(new_calibrated_IL >> 1),
                       &(l1a_l1s_com.last_input_level[serving_index].lna_off),
                       l1a_l1s_com.Scell_info.radio_freq);

      l1a_l1s_com.last_input_level[serving_index].input_level = new_IL;
    }

    // transfer_meas updating
    //-----------------------

    // IL = (min IL in FIFO) + P0 + 10 (PR = 5 format 7.1)
    // Input levels are always stored with lna_on
    input_level = l1a_l1s_com.last_input_level[serving_index].input_level + l1ps.read_param.dl_pwr_ctl.p0 + 10;
  
    // IL_2_AGC_xx array size 
    if (input_level>INDEX_MAX) input_level = INDEX_MAX;

    // lna_off processing
    new_calibrated_IL = (WORD16) (input_level - l1ctl_encode_delta1(l1ps.read_param.radio_freq_for_lna));

    if (new_calibrated_IL>INDEX_MAX) new_calibrated_IL = INDEX_MAX;

    l1ctl_encode_lna((UWORD8)(new_calibrated_IL>>1), &(l1a_l1s_com.Scell_info.transfer_meas.lna_off), l1ps.read_param.radio_freq_for_lna);

    l1a_l1s_com.Scell_info.transfer_meas.input_level = (UWORD8)input_level;
  } // End if "burst = 3"

} // End of "l1pctl_dpcma_agc_read"

/*-------------------------------------------------------*/
/* l1pctl_dpcmb_agc_read()                               */
/*-------------------------------------------------------*/
/* Description :                                         */
/* ===========                                           */
/* AGC algorithm in packet transfer used when the BTS    */
/* use DOWNLINK POWER CONTROL mode B.                    */
/* This function is used during the read phase of PDTCH: */
/* 1- to determine the IL value for each timeslot in each*/
/*    TDMA                                               */
/* 2- to find the IL value to use for the next PDCH      */
/*    block                                              */
/*                                                       */
/* Algorithm                                             */
/* ---------                                             */
/* For each timeslot i used for PDCH                     */
/*   IL(i) = fct(used AGC, pm)                           */
/*   if (beacon)                                         */
/*     ILmax_beacon = max(ILmax_beacon, IL(i))           */
/*   else                                                */
/*     ILmax_others(i) = max(IL(i), ILmax_others(i))     */
/*                                                       */
/* If (burst_nb == 3)                                    */
/*                                                       */
/*   For each timeslot i used for PDCH                   */
/*     if (CRC good)                                     */
/*       if (TFI good) last_PR_good = PR(i)              */
/*       if ((PR_MODE A) or (PR_MODE B and TFI good))    */
/*         ILmax = max(ILmax, ILmax_others(i)            */
/*                 + P0 + PR(i))                         */
/*                                                       */
/*   ILmax=max(ILmax, ILmax_beacon)                      */
/*   FIFO[beacon] updated with ILmax                     */
/*   last_input_level[serving beacon] = max(FIFO[beacon])*/
/*   transfer_meas = max(FIFO[beacon]) - last_PR_good    */
/*                                                       */
/*   Reset ILmax_others[8] and ILmax_beacon              */
/*                                                       */
/* WARNING: in the layer 1 code, input levels IL(l1) use */
/*          format 7.1:                                  */
/*               *********************                   */
/*               * IL(l1) = - 2 x IL *                   */
/*               *********************                   */
/*          -> Reversed sign, reversed test conditions   */
/*          -> max replaced by min                       */
/*          ex: if IL -120 dBm, IL(l1) = 240             */
/*                                                       */
/* Parameters                                            */
/* ----------                                            */
/* "calibrated_IL[8]"                                    */
/*   contains the IL found on timeslots                  */
/*   used for PDCH/D. These ILs can be used to process   */
/*   RXLEV values.                                       */
/*                                                       */
/* "*pdsp_db_r_ptr"                                      */
/*   Pointer on the DSP DB Read page, used to extract    */
/*   pm values, burst number and timeslot allocated      */
/*   for downlink PDCH                                   */
/*                                                       */
/* "*pdsp_ndb_ptr"                                       */
/*   Pointer on the DSP NDB page, used to extract the    */
/*   CRC value for each decoded burst                    */
/*                                                       */
/* Global parameters                                     */
/* -----------------                                     */
/* "l1a_l1s_com.Scell_info.transfer_meas.input_level"    */
/* "l1a_l1s_com.Scell_info.transfer_meas.lna_off"        */
/*   Used to store the ILselected and the associated     */
/*   lna_off value.                                      */
/*                                                       */
/* "l1a_l1s_com.last_input_level[freq. index]            */
/*     .input_level                                      */
/*     .lna_off"                                         */
/*   Used to store the beacon input level and            */
/*   the associated lna_off value.                       */
/*                                                       */
/* "l1ps.transfer_beacon_buf[4]"                         */
/*   FIFO[beacon]                                        */
/*                                                       */
/* "l1ps.ILmin_beacon"                                   */
/* "l1ps.ILmin_others[8]"                                */
/*-------------------------------------------------------*/
void l1pctl_dpcmb_agc_read(UWORD8 calibrated_IL[8], T_DB_DSP_TO_MCU_GPRS *pdsp_db_r_ptr,
                           T_NDB_MCU_DSP_GPRS *pdsp_ndb_ptr, UWORD8 pr_table[8])
{
  UWORD8       ts                         = 0;
  UWORD8       rx_no                      = 0;
  UWORD8       bit_mask                   = 0x80;
  UWORD8       IL_norm_min                = 255;
  WORD8        delta1_freq, delta2_freq;
  WORD16       delta_drp_gain=0;
  UWORD16      radio_freq, lna_value;
  WORD16       used_agc;
  WORD32       serving_index;
  UWORD16      arfcn;
  UWORD8       lna_off;
  UWORD16 dco_algo_ctl_pw_temp = 0;
  UWORD8 if_ctl = 0;
  #if (CODE_VERSION != SIMULATION)
  UWORD8 if_threshold = C_IF_ZERO_LOW_THRESHOLD_GPRS;
#endif

#if(L1_FF_MULTIBAND == 0)
  serving_index = l1a_l1s_com.Scell_info.radio_freq - l1_config.std.radio_freq_index_offset;
#else
  serving_index = l1_multiband_radio_freq_convert_into_operative_radio_freq(l1a_l1s_com.Scell_info.radio_freq);
#endif


  // Control phase parameters: same AGC, radio_freq, lna_off used for all PDTCH
  // ***************************************************************************

  // Get radio_freq on which the downlink block was received
  radio_freq     = l1a_l1s_com.dedic_set.radio_freq_dd;

  // Compute calibration factors
  delta1_freq = l1ctl_encode_delta1(radio_freq);
  delta2_freq = l1ctl_encode_delta2(radio_freq);

  // Last known AGC (format F7.1)
  used_agc = (Cust_get_agc_from_IL(radio_freq, l1a_l1s_com.Scell_used_IL_dd.input_level >> 1, MAX_ID,l1a_l1s_com.Scell_used_IL_dd.lna_off)) << 1;

  // LNA attenuation
  lna_value = l1a_l1s_com.Scell_used_IL_dd.lna_off * l1ctl_get_lna_att(radio_freq);

  // Burst 0: Reset ILmin_beacon and ILmin_others
  if(pdsp_db_r_ptr->d_burst_nb_gprs == 0)
  {
    l1ps.ILmin_beacon = 255;  // Not valid

    for (ts = 0; ts < 8; ts++)
    {
       l1ps.ILmin_others[ts] = 255;  // Not valid
    }
  }

  // IL processing for each received burst
  // **************************************

  // For each timeslot on which a burst was received
  for(ts = 0; ts < 8; ts ++)
  {
    if(pdsp_db_r_ptr->d_task_d_gprs & bit_mask)
    {
      WORD16       current_IL, current_calibrated_IL;
      UWORD8       pm;

      // IL = fct(pm, last_known_agc, lna_value, g_magic)
      //-------------------------------------------------

      pm    = (UWORD8) ((pdsp_db_r_ptr->a_burst_pm_gprs[ts]   & 0xffff) >> 5);

      // current_IL processing
      if (0==pm)  // Check and filter illegal pm value by using last valid IL
      {
          current_IL = l1a_l1s_com.last_input_level[serving_index].input_level;

          if (radio_freq != l1a_l1s_com.Scell_info.radio_freq)
            current_IL += (l1ps.last_PR_good);
      }
      else
	  {
	    // DRP correction
#if (L1_FF_MULTIBAND == 0)  
  arfcn = Convert_l1_radio_freq(radio_freq);
#else
  arfcn=radio_freq;
#endif 

#if(RF_FAM == 61)
   #if (CODE_VERSION != SIMULATION)


    cust_get_if_dco_ctl_algo(&dco_algo_ctl_pw_temp, &if_ctl, (UWORD8) L1_IL_VALID , 
                                         l1a_l1s_com.Scell_used_IL_dd.input_level,
                                         radio_freq, if_threshold);
     lna_off = l1a_l1s_com.Scell_used_IL_dd.lna_off;
     delta_drp_gain = drp_gain_correction(arfcn, lna_off, used_agc);    // F7.1 format

     if(if_ctl == IF_100KHZ_DSP){
       delta_drp_gain += SCF_ATTENUATION_LIF_100KHZ;
     }
     else{ /* i.e. if_ctl = IF_120KHZ_DSP*/
       delta_drp_gain += SCF_ATTENUATION_LIF_120KHZ;    
     }


   #endif
#endif

        current_IL = -(pm - ( used_agc - delta_drp_gain) + lna_value - l1ctl_get_g_magic(radio_freq));
	  }

      // Calibrated IL processing
      current_calibrated_IL = current_IL - delta1_freq - delta2_freq;
      
      // Protect IL stores against overflow
      if(current_calibrated_IL>INDEX_MAX)
        current_calibrated_IL=INDEX_MAX;
      if (current_IL>INDEX_MAX)
        current_IL=INDEX_MAX;
         
      calibrated_IL[ts] = (UWORD8)(current_calibrated_IL);  

      // Keep the minimum IL
      if (radio_freq == l1a_l1s_com.Scell_info.radio_freq)
      {
        // Beacon frequency
        l1ps.ILmin_beacon     = min((UWORD8) current_IL,l1ps.ILmin_beacon);
      }
      else
      {
        // Daughter frequency
        l1ps.ILmin_others[ts] = min((UWORD8) current_IL, l1ps.ILmin_others[ts]);
      }

      // Input Level selection among ILmax found on each timeslot during the block (when burst = 3)
      //-------------------------------------------------------------------------------------------

      if(pdsp_db_r_ptr->d_burst_nb_gprs == 3)
      {
        // If CRC good
        if (!(pdsp_ndb_ptr->a_dd_gprs[rx_no][0] & 0x0100))
        {
          // If ((PR_MODE A and TFI good) or (PR_MODE B))
          if ((l1ps.read_param.dl_pwr_ctl.pr_mode != 0) || (!(pr_table[ts] & 0x80)))
          {

            // If TFI good
            if (!(pr_table[ts] & 0x80))
            {
              // Memorize decoded PR
              l1ps.last_PR_good = ((pr_table[ts] & 0x1f) << 1);
            }            

            if (l1ps.ILmin_others[ts] != 255)
            {
              UWORD8 IL_norm;

              // IL normalization to beacon (ILnorm = ILmax_others(ts) - PR)
              IL_norm = l1ps.ILmin_others[ts] - ((pr_table[ts] & 0x1f) << 1);

              // Update IL_min with the minimum found IL
              IL_norm_min = min(IL_norm, IL_norm_min);
            }
          }
        } // End if "CRC good"

      } // End if "burst = 3"

      // Next downlink block
      rx_no ++;

    } // End if "timeslot used for downlink PDCH"

    // Next timeslot
    bit_mask >>= 1;

  } // End for "each timeslot...."


  // IL selection for the next block if burst = 3
  // **********************************************

  if(pdsp_db_r_ptr->d_burst_nb_gprs == 3)
  {
    UWORD8  i, new_IL;
    UWORD16 input_level;
    WORD16  new_calibrated_IL;

    // Select the minimum IL between minimum IL found on daughter frequencies (normalized to beacon)
    // and minimum IL found on the beacon
    IL_norm_min = min(IL_norm_min, l1ps.ILmin_beacon);

    // If a valid IL has been found
    if (IL_norm_min != 255)
    {
      // FIFO management
      for (i=3;i>0;i--)
        l1a_l1s_com.Scell_info.buff_beacon[i] = l1a_l1s_com.Scell_info.buff_beacon[i-1];

      l1a_l1s_com.Scell_info.buff_beacon[0] = IL_norm_min;

      // last_input_level[serving beacon] updating
      //------------------------------------------

      // Find min IL in FIFO
      new_IL = l1ctl_find_max(l1a_l1s_com.Scell_info.buff_beacon,4);
  
      // Input levels are always stored with lna_on

      // lna_off processing
      new_calibrated_IL = (WORD16) (new_IL - l1ctl_encode_delta1(l1a_l1s_com.Scell_info.radio_freq) - l1ctl_encode_delta2(l1a_l1s_com.Scell_info.radio_freq));

      if (new_calibrated_IL>INDEX_MAX) new_calibrated_IL = INDEX_MAX;

      l1ctl_encode_lna((UWORD8)(new_calibrated_IL >> 1),
                       &(l1a_l1s_com.last_input_level[serving_index].lna_off),
                       l1a_l1s_com.Scell_info.radio_freq);

      l1a_l1s_com.last_input_level[serving_index].input_level = new_IL;
    }

    // transfer_meas updating
    //-----------------------

    // IL = (min IL in FIFO) + PR (Middle of the range specified by the last decoded PR with CRC and TFI good)
    // Input levels are always stored with lna_on
    input_level = l1a_l1s_com.last_input_level[serving_index].input_level + l1ps.last_PR_good;

    // IL_2_AGC_xx array size 
    if (input_level>INDEX_MAX) input_level = INDEX_MAX;

    // lna_off processing
    new_calibrated_IL = (WORD16) (input_level - l1ctl_encode_delta1(l1ps.read_param.radio_freq_for_lna));
    
    if (new_calibrated_IL>INDEX_MAX) new_calibrated_IL = INDEX_MAX;

    l1ctl_encode_lna((UWORD8)(new_calibrated_IL>>1), &(l1a_l1s_com.Scell_info.transfer_meas.lna_off), l1ps.read_param.radio_freq_for_lna);

    l1a_l1s_com.Scell_info.transfer_meas.input_level = (UWORD8)input_level;

  } // End if "burst = 3"

} // End of "l1pctl_dpcmb_agc_read"

/*-------------------------------------------------------*/
/* l1pctl_pgc()                                          */
/*-------------------------------------------------------*/
/* Description :                                         */
/* =============                                         */
/*  This function is used in packet transfer mode for the*/
/*  Read phase of power measurements. It permits to:     */
/*  - Process the IL value in function of the Pm and AGC */
/*    used                                               */
/*  - Update the FIFO[beacon] used in packet transfer AGC*/
/*    algorithms                                         */
/*  - Update last_input_level                            */
/*                                                       */
/* WARNING: in the layer 1 code, input levels IL(l1) use */
/*          format 7.1:                                  */
/*               *********************                   */
/*               * IL(l1) = - 2 x IL *                   */
/*               *********************                   */
/*          -> Reversed sign, reversed test conditions   */
/*          -> max replaced by min                       */
/*          ex: if IL -120 dBm, IL(l1) = 240             */
/*-------------------------------------------------------*/
UWORD8 l1pctl_pgc(UWORD8 pm, UWORD8 last_known_il, UWORD8 lna_off, UWORD16 radio_freq)
{
  UWORD8       i, new_IL;
  WORD8        delta1_freq, delta2_freq;
  WORD16       delta_drp_gain=0;
  UWORD16      lna_value;
  WORD16       used_agc, current_IL, current_calibrated_IL, new_calibrated_IL;
  WORD32       index;
  UWORD16      arfcn;
  UWORD16 dco_algo_ctl_pw_temp = 0;
  UWORD8 if_ctl = 0;
  #if (CODE_VERSION != SIMULATION)
  UWORD8 if_threshold = C_IF_ZERO_LOW_THRESHOLD_GPRS;
#endif

  // Calibration factors
  delta1_freq = l1ctl_encode_delta1(radio_freq);
  delta2_freq = l1ctl_encode_delta2(radio_freq);

  // initialize index
#if(L1_FF_MULTIBAND == 0)
  index = radio_freq - l1_config.std.radio_freq_index_offset;

#else
  index = l1_multiband_radio_freq_convert_into_operative_radio_freq(radio_freq);
#endif


  // LNA attenuation
  lna_value = lna_off * l1ctl_get_lna_att(radio_freq);

  // Used AGC in the control phase (format F7.1)
  used_agc = (Cust_get_agc_from_IL(radio_freq, last_known_il >> 1, PWR_ID, lna_off)) << 1;


  // DRP correction
#if (L1_FF_MULTIBAND == 0)  
  arfcn = Convert_l1_radio_freq(radio_freq);
#else
  arfcn=radio_freq;
#endif 

#if(RF_FAM == 61)
   #if (CODE_VERSION != SIMULATION)


#if (PWMEAS_IF_MODE_FORCE == 0)            
       cust_get_if_dco_ctl_algo(&dco_algo_ctl_pw_temp, &if_ctl, (UWORD8) L1_IL_VALID , 
           last_known_il,
           radio_freq, if_threshold);
     #else
       if_ctl = IF_120KHZ_DSP;
       dco_algo_ctl_pw_temp = DCO_IF_0KHZ;                     
     #endif        
    
     lna_off = l1a_l1s_com.Scell_used_IL_dd.lna_off;
     delta_drp_gain = drp_gain_correction(arfcn, lna_off, used_agc);    // F7.1 format

     if(if_ctl == IF_100KHZ_DSP){
       delta_drp_gain += SCF_ATTENUATION_LIF_100KHZ;
     }
     else{ /* i.e. if_ctl = IF_120KHZ_DSP*/
       delta_drp_gain += SCF_ATTENUATION_LIF_120KHZ;    
     }


   #endif
#endif

  if (0==pm)  // Check and filter illegal pm value by using last valid IL
    current_IL = l1a_l1s_com.last_input_level[index].input_level;
  else
    current_IL = -(pm - (used_agc - delta_drp_gain) + lna_value - l1ctl_get_g_magic(radio_freq));

  // Calibrated IL processing
  current_calibrated_IL = current_IL - delta1_freq - delta2_freq;
  
  // Protect IL stores against overflow 
  if (current_calibrated_IL>INDEX_MAX)
    current_calibrated_IL=INDEX_MAX;
  if (current_IL>INDEX_MAX)
    current_IL=INDEX_MAX;

  // if radio freq is the serving beacon
  //------------------------------------
  if (radio_freq == l1a_l1s_com.Scell_info.radio_freq)
  {
    // FIFO management
    for (i=3;i>0;i--)
      l1a_l1s_com.Scell_info.buff_beacon[i] = l1a_l1s_com.Scell_info.buff_beacon[i-1];
    l1a_l1s_com.Scell_info.buff_beacon[0] = (UWORD8) current_IL;

      // Find min IL in FIFO
    new_IL = l1ctl_find_max(l1a_l1s_com.Scell_info.buff_beacon,4);

    // lna_off processing
    new_calibrated_IL = (WORD16) (new_IL - delta1_freq - delta2_freq);
    if (new_calibrated_IL>INDEX_MAX) new_calibrated_IL = INDEX_MAX;

    l1ctl_encode_lna((UWORD8)(new_calibrated_IL>>1),
                     &(l1a_l1s_com.last_input_level[index].lna_off),
                     radio_freq);

    l1a_l1s_com.last_input_level[index].input_level = new_IL;
  }

  // if radio freq is a neighbor beacon
  //-----------------------------------
  else
  {
    // Update last_input_level (IL with LNA ON)
    l1ctl_encode_lna((UWORD8)(current_calibrated_IL>>1), 
                     &(l1a_l1s_com.last_input_level[index].lna_off),
                     radio_freq);

    l1a_l1s_com.last_input_level[index].input_level = (UWORD8)current_IL;
  }

  return((UWORD8)current_calibrated_IL);

} // End of "l1pctl_pgc"
#endif

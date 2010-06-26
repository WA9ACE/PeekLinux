/************* Revision Controle System Header *************
 *                  GSM Layer 1 software
 * L1P_DEFTY.H
 *
 *        Filename l1p_deft.h
 *  Copyright 2003 (C) Texas Instruments
 *
 ************* Revision Controle System Header *************/

typedef struct
{
  WORD32  rand;
  UWORD16 channel_request_data;
  UWORD8  bs_prach_blks;
  UWORD8  prach_alloc;
  UWORD32 fn_to_report;
#if FF_L1_IT_DSP_USF
    UWORD8  prach_controlled;
#endif
}
T_PRACH_INFO;

typedef union
{
  UWORD16 prach_data[12];
  UWORD8  cs1_data[24];
}
T_CHAN_REQ_DATA;

typedef struct
{
  UWORD8          pol_resp_type;
  T_CHAN_REQ_DATA chan_req;
  UWORD32         fn;
  UWORD8          timing_advance;
  UWORD32         fn_to_report;
}
T_POLL_INFO;


typedef struct
{
  UWORD8  data_array[23];
  UWORD8  tn;
  UWORD8  dl_tn_to_restore;
  UWORD8  activity;
}
T_SINGLE_BLOCK;


typedef struct
{
  UWORD32               SignalCode;
  UWORD8                assignment_id;
  UWORD8                synchro_tn;
  UWORD8                allocated_tbf;
  UWORD8                dl_tbf_synchro_timeslot;
  UWORD8                ul_tbf_synchro_timeslot;
  UWORD8                transfer_synchro_timeslot;

  UWORD8                assignment_command;
  UWORD8                multislot_class;
  T_PACKET_TA           packet_ta;
  T_DL_POWER_CTL        dl_pwr_ctl;
  UWORD8                tsc;
  T_PACKET_FREQ_PARAM   freq_param;
  UWORD8                mac_mode;
  T_TBF_STI             tbf_sti;

  T_UL_RESSOURCE_ALLOC  *ul_tbf_alloc;
  T_DL_RESSOURCE_ALLOC  dl_tbf_alloc;

  BOOL                  interf_meas_enable;
  UWORD8                ts_override;
  BOOL                  pc_meas_chan;
  #if FF_TBF
    BOOL                  pseudo_tbf_two_phase_acc;
  #endif
}
T_TRANSFER_SET;


typedef struct
{
  UWORD8       ta_update_cmd;
  UWORD8       assignment_id;
  UWORD16      radio_freq;
  UWORD8       activity;
  BOOL         request_dl;
  T_PACKET_TA  packet_ta;
}
T_PTCCH_PARAM;

typedef struct
{
  UWORD8       tbf_release_cmd;
  UWORD8       released_tbf;
}
T_TBF_RELEASE_PARAM;

typedef struct
{
  UWORD8       pdch_release_cmd;
  UWORD8       timeslot_available;
  UWORD8       assignment_id;
}
T_PDCH_RELEASE_PARAM;

typedef struct
{
 UWORD8        txpwr[8];
 BOOL          crc_error;
 WORD8         bcch_level;
 UWORD16       radio_freq_tbl[4];
 WORD8         burst_level[4];
}
T_DL_PWR_CTRL;

typedef struct
{
  UWORD8       psi_param_update_cmd;
  UWORD8       Scell_pb;
  UWORD8       access_burst_type;
}
T_PSI_PARAM_UPDATE;

typedef struct
{
  BOOL                              semaphore;

  T_TRANSFER_SET                    *aset;
  T_TRANSFER_SET                    *fset[2];
  T_TRANSFER_SET                    set[3];

  T_UL_RESSOURCE_ALLOC              ul_tbf_alloc[3];

  T_SINGLE_BLOCK                    single_block;

  T_PTCCH_PARAM                     ptcch;
  T_TBF_RELEASE_PARAM               tbf_release_param;
  T_PDCH_RELEASE_PARAM              pdch_release_param;
  T_MPHP_REPEAT_UL_FIXED_ALLOC_REQ  repeat_alloc;
  T_PSI_PARAM_UPDATE                psi_param;

  T_DL_PWR_CTRL                     dl_pwr_ctrl;
}
T_TRANSFER_PARAM;

/***************************************************************************************/
/* L1S packet global variable structure...                                             */
/***************************************************************************************/

typedef struct
{
  BOOL            new_set;
  UWORD8          assignment_id;
  UWORD8          allocated_tbf;
  UWORD8          dl_tfi;
  UWORD8          ul_tfi;
  T_DL_POWER_CTL  dl_pwr_ctl;
  BOOL            pc_meas_chan;
  UWORD16         radio_freq_for_lna;
  UWORD8          dl_tn;
} T_READ_PARAM;

typedef struct
{
  UWORD8     ILmin_beacon;
  UWORD8     ILmin_others[8];

  // No power control AGC algorithm variables
  UWORD32    fn_select;
  BOOL       phase;

  // Downlink power control mode B variables
  UWORD8     last_PR_good;

  // TBF Parameters used during the PDTCH read phase
  T_READ_PARAM   read_param;

  // Flag used to know if the pc_meas_chan measurement has been controlled
  BOOL pc_meas_chan_ctrl;

  #if (GSM_IDLE_RAM == 1)
    // Used to avoid allocation of ext mem data while in L1PS_meas_manager (allocate signal long time before sending)
    WORD8  ncell_meas_rxlev[33];
  #endif
}
T_L1PS_GLOBAL;

/***********************************************************/
/* L1PA global variable structure...                       */
/***********************************************************/
typedef struct
{
  // State for L1A state machines...
  //-----------------------------------------
  UWORD8    state[NBR_L1PA_PROCESSES];

  // Measurement tasks management...
  //-----------------------------------------
  UWORD32   l1pa_en_meas[NBR_L1PA_PROCESSES];
}
T_L1PA_GLOBAL;


/*************************************************************/
/* Packet Idle mode tasks information...                     */
/*************************************************************/
/* must be filled according to Packet Idle parameters...     */
/* ...                                                       */
/*************************************************************/
// Packet Idle Parameter description
typedef struct
{
  UWORD16  radio_freq;
  UWORD16  radio_freq_d;
  UWORD16  radio_freq_dd;
}
T_P_IDLE_PARAM;

// Packet Paging description
typedef struct
{
  // -------- Packet Paging description --------------
  UWORD8                  bs_pag_blks_res; // BS_PAG_BLKS_RES parameter (cf. S921 specification)
  UWORD8                  bs_pbcch_blks;   // BS_PBCCH_BLKS parameter (cf. S921 specification)
  T_PACKET_CHANNEL_DESC   packet_chn_desc; // Packet Channel Description: arfcn/Hopping description + TN + TSC
  T_MOBILE_ALLOCATION     frequency_list;  // Mobile allocation description
  UWORD8                  nb_ppch_per_mf52;// Number of Packet PCCH per MF52
  UWORD16                 first_pg_grp;    // First Paging Group index
  UWORD16                 pg_blks_avail;   // Number of Paging blocks available per MF52
  UWORD16                 pg_offset;       // (IMSI mo 1000) div (KC*N) (Note N = 1 for PCCCH)
  UWORD16                 split_pg_value;  // Min(pg_blks_avail, split_pg_cycle)

  //--
  BOOL                    epg_computation; // Packet Paging STATE used in Extended Paging scheduling
  UWORD16                 pnp_period;      // Paging Period = (52*64)/split_pg_value
  WORD16                  time_to_pnp;     // Time to  next PPCH block position.
}
T_PCCCH_PARAM;

typedef struct
{
  BOOL                    change_synchro;
  BOOL                    control_offset;
  BOOL                    read_all_psi;

  UWORD8                  nbr_psi;
  UWORD8                  bs_pbcch_blks;
  UWORD8                  psi1_repeat_period;
  UWORD8                  relative_position_array[20]; // Array for Relative position
  UWORD16                 offset_array[20];            // Array of offset computed from relative position
  T_PACKET_CHANNEL_DESC   packet_chn_desc;             // Packet Channel Description: arfcn/Hopping description + TN + TSC
  T_MOBILE_ALLOCATION     frequency_list;              // Mobile allocation description
  UWORD8                  rel_pos_to_report;           // Relative Position of the PBCCH block read and to
                                                       // to report to L3.
  UWORD16                 pbcch_period;                // PBCCH period = MFL52 * psi1_repaet_period;
  UWORD8                  tn_pbcch;                    // Time slot number relative for DSP point of view.
}
T_PBCCHS_PARAM;

typedef struct
{
  BOOL                    change_synchro;

  UWORD8                  pb;
  UWORD8                  bs_pbcch_blks;
  UWORD8                  psi1_repeat_period;
  UWORD8                  relative_position;           // for Relative position
  UWORD16                 offset;                      // offset computed from relative position
  T_PACKET_CHANNEL_DESC   packet_chn_desc;             // Packet Channel Description: arfcn/Hopping description + TN + TSC
  T_MOBILE_ALLOCATION     frequency_list;              // Mobile allocation description
  UWORD16                 pbcch_period;                // PBCCH period = MFL52 * psi1_repaet_period;
  UWORD16                 bcch_carrier;                // beacon of the neighbor cell (used by the DSP)
  WORD32                  fn_offset;
  WORD32                  time_alignmt;
}
T_PBCCHN_PARAM;

typedef struct
{
  BOOL                    dsp_r_page_switch_req;       // If true, the dsp_r_page must be toggled in the interference measurement read
  UWORD8                  multislot_class;             // Mobile multislot class
  UWORD8                  meas_bitmap;                 // Timeslots on which the interference measurements are done
  UWORD8                  position;                    // Position on which measurements must be done (search or ptcch frame)
  UWORD8                  idle_tn_rx;                  // Timeslot bitmap in packet idle with PCCCH on the frame before
  UWORD8                  idle_tn_no_rx;               // Timeslot bitmap in packet idle without PCCCH on the frame before
  UWORD16                 radio_freq;                  // Radio freq. processed by l1s_hopping_algo()
  T_PACKET_FREQ_PARAM     packet_intm_freq_param;      // Frequency parameters
}
T_ITMEAS_PARAM;

typedef struct
{
  UWORD32             fn;               // Absolute FN reported during the last measurement session
  UWORD8              rxlev[8];         // Buffer used to save the parameters reported to L3
  UWORD8              meas_bitmap;      // Interf meas bitmap.
  UWORD8              id;               // Id.
} T_L1A_INT_MEAS_PARAM;

typedef struct
{
  UWORD8                  lna;  // Input level result
  UWORD8                  il;   // LNA parameter
}
T_IL_LNA;

typedef struct
{
  UWORD8         next_to_ctrl;     // Carrier for next power measurement result.
  UWORD8         next_to_read;     // Next carrier to measure.

  UWORD8         pnp_ctrl;         // Specify to l1s_meas_manager() that a PPCH burst has been ctrl.

  UWORD8         ms_ctrl;
  UWORD8         ms_ctrl_d;
  UWORD8         ms_ctrl_dd;

  T_IL_LNA       used_il_lna[NB_MEAS_MAX_GPRS];
  T_IL_LNA       used_il_lna_d[NB_MEAS_MAX_GPRS];
  T_IL_LNA       used_il_lna_dd[NB_MEAS_MAX_GPRS];
}
T_CR_FREQ_LIST;

typedef struct
{
  UWORD8   nb_carrier;
  UWORD16  freq_list[32+1];
  UWORD8   list_id;
} T_CRES_LIST_PARAM;

typedef struct
{
  T_CRES_LIST_PARAM  *alist;
  T_CRES_LIST_PARAM  *flist;
  T_CRES_LIST_PARAM  list[2];
} T_CRES_FREQ_LIST;

typedef struct
{
  UWORD8            tcr_next_to_ctrl;
  UWORD8            tcr_next_to_read;
  UWORD8            last_stored_tcr_to_read;

  UWORD8            ms_ctrl;
  UWORD8            ms_ctrl_d;
  UWORD8            ms_ctrl_dd;

  BOOL              first_pass_flag;

  T_IL_LNA          used_il_lna;
  T_IL_LNA          used_il_lna_d;
  T_IL_LNA          used_il_lna_dd;

  BOOL              new_list_present;

  WORD8             beacon_meas;

  WORD8             fn_report;
  UWORD8            cres_meas_report;

} T_TCR_LIST_SET;

/***************************************************************************************/
/* L1A <-> L1S communication structure dedicated to packet mode                        */
/***************************************************************************************/
typedef struct
{
  // Packet Idle Parameters

  // PCCCH parameters
  T_PCCCH_PARAM     pccch;

  //--
  T_P_IDLE_PARAM          p_idle_param;    // Packet Idle Parameters description

  // Packet Access Parameters
  T_PRACH_INFO      pra_info;

  // Packet Access Parameters
  T_POLL_INFO       poll_info;

  // Packet Transfer Parameters
  T_TRANSFER_PARAM  transfer;

  // Serving Cell PBCCH parameters
  T_PBCCHS_PARAM     pbcchs;

  // Neigbour Cell PBCCH parameters
  T_PBCCHN_PARAM     pbcchn;

  // Interference measurement parameters
  T_ITMEAS_PARAM     itmeas;

  // Measurement tasks management...
  //-----------------------------------------
  UWORD32         meas_param;         // Synchro semaphore bit register for packet measurement tasks.
  UWORD32         l1ps_en_meas;       // Enable packet Meas task bit register.

  // Access burst type
  //------------------
  UWORD8           access_burst_type;

  // Frequency list Parameters used in Cell Reselection measurements
  // ---------------------------------------------------------------
  T_CR_FREQ_LIST   cr_freq_list;

  // Parameters used in Packet Transfer Neighbour measurement task
  // -------------------------------------------------------------
  T_TCR_LIST_SET   tcr_freq_list;

  // Cell Reselection list, used both in Packet Idle and Packet Transfer
  // -------------------------------------------------------------------
  T_CRES_FREQ_LIST  cres_freq_list;

} T_L1PA_L1PS_COM;


/***************************************************************************************/
/* API (MCU <-> DSP) communication structures                                          */
/***************************************************************************************/

typedef struct
{
  // GPRS Extension...............
  API d_task_d_gprs;        // (map?) Task, burst per burst  (part of header)
  API d_task_u_gprs;        // (map?) Task, burst per burst  (part of header)
  API d_task_pm_gprs;       // (map?) Task, burst per burst  (part of header)
  API d_burst_nb_gprs;      // (map?) burst identifier. (part of header)
  API a_ctrl_abb_gprs[8];   // (map?) Analog baseband control, burst per burst.
  API a_ctrl_power_gprs[8]; // (map?) Power control value, burst per burst.
}
T_DB_MCU_TO_DSP_GPRS;

typedef struct
{
  // GPRS Extension...............
  API d_task_d_gprs;        // (map?) Task, burst per burst  (part of header)
  API d_task_u_gprs;        // (map?) Task, burst per burst  (part of header)
  API d_task_pm_gprs;       // (map?) Task, burst per burst  (part of header)
  API d_burst_nb_gprs;      // (map?) burst identifier. (part of header)

  API a_burst_toa_gprs[8];  // (map?) Time of arrival, burst per burst
  API a_burst_pm_gprs[8];   // (map?) Receive Power Level, burst per burst
  API a_burst_angle_gprs[8];// (map?) Angle deviation, burst per burst
  API a_burst_snr_gprs[8];  // (map?) Signal to noise ratio, burst per burst
}
T_DB_DSP_TO_MCU_GPRS;

#if (DSP == 38) || (DSP == 39)
  typedef struct
  {
    API d_gea_mode_hole;   // 0x09AE
    API a_gea_kc_hole[4];

    API d_hole1_ndb_gprs[8];
    API d_a5mode_ovly; // 0x09BB

    API d_sched_mode_gprs;    // 0x09BC

    API d_hole2_ndb_gprs[5];

    API d_usf_updated_gprs;   // 0x09C2
    API d_win_start_gprs;     //
    API d_usf_vote_enable;    //
    API d_bbctrl_gprs_hole;

    API d_hole3_ndb_gprs[2];

    // (map?) BTS physical timeslot mapping.
    API a_ctrl_ched_gprs[8];   // 0x09C8 (map?) Ched configuration, burst per burst
    API a_ul_buffer_gprs[8];   // 0x09D0 (map?) UL burst / UL buffer mapping, burst per burst. (part of header)
    API a_usf_gprs[8];         // 0x09D8
    API a_interf_meas_gprs[8]; // 0x09E0

    API a_ptcchu_gprs[4];      // 0x09E8

    API a_dd_md_gprs[16];      // 0x09EC

    API a_du_gprs[4][29];      //
    API a_pu_gprs[4][15];

    API d_rlcmac_rx_no_gprs;

    API a_dd_gprs[8][31];      //

    API a_drp_ramp2_gprs_holes[2907];      // 0x0C25
    API a_drp_ramp2_gprs[8][20];     // 0x1700 - Power Ramp up/down in DRP registers format
  }
  T_NDB_MCU_DSP_GPRS;

#elif ((DSP == 34) || (DSP == 35) || (DSP == 36) || (DSP == 37))
  typedef struct
  {
    // GEA module
    API d_gea_mode;
    API a_gea_kc[4];

    API d_hole1_ndb_gprs[8];
    API d_a5mode_ovly;

    API d_sched_mode_gprs;

    API d_hole2_ndb_gprs[5];

    API d_usf_updated_gprs;
    API d_win_start_gprs;
    API d_usf_vote_enable;
    API d_bbctrl_gprs;

    API d_hole3_ndb_gprs[2];

    // (map?) BTS physical timeslot mapping.
    API a_ctrl_ched_gprs[8];  // (map?) Ched configuration, burst per burst
    API a_ul_buffer_gprs[8];  // (map?) UL burst / UL buffer mapping, burst per burst. (part of header)
    API a_usf_gprs[8];
    API a_interf_meas_gprs[8];

    API a_ptcchu_gprs[4];

    API a_dd_md_gprs[16];

    API a_du_gprs[4][29];
    API a_pu_gprs[4][15];

    API d_rlcmac_rx_no_gprs;

    API a_dd_gprs[8][31];

    API a_ramp_gprs[8][16];

  }
  T_NDB_MCU_DSP_GPRS;

#elif (DSP == 33)
  typedef struct
  {
    // GEA module
    API d_gea_mode;
    API a_gea_kc[4];

    API d_hole1_ndb_gprs[8];
    API d_a5mode_ovly;

    API d_sched_mode_gprs;

    API d_hole2_ndb_gprs[5];

    API d_usf_updated_gprs;
    API d_win_start_gprs;
    API d_usf_vote_enable;
    API d_bbctrl_gprs;

    API d_hole3_ndb_gprs[2];

    // (map?) BTS physical timeslot mapping.
    API a_ctrl_ched_gprs[8];  // (map?) Ched configuration, burst per burst
    API a_ul_buffer_gprs[8];  // (map?) UL burst / UL buffer mapping, burst per burst. (part of header)
    API a_usf_gprs[8];
    API a_interf_meas_gprs[8];

    API a_ptcchu_gprs[4];

    API a_dd_md_gprs[16];

    API a_du_gprs[4][29];
    API a_pu_gprs[4][15];

    API d_rlcmac_rx_no_gprs;

    API a_dd_gprs[8][31];

    API a_ramp_gprs[8][16];

  }
  T_NDB_MCU_DSP_GPRS;

#else

typedef struct
{
  // GPRS Extension...............
  API a_ramp_gprs[8][16];

  #if (CODE_VERSION != SIMULATION)
  API a_hole1_gprs[22];         // hole for a_du_1 and d_a5_mode
  #endif

  API d_ptcchu_ctrl_abb_gprs;
  API d_ptcchu_ctrl_power_gprs;
  API d_ptcchd_burst_nb_gprs;

  #if (CODE_VERSION != SIMULATION)
  API a_hole2_gprs[184];
  #endif

  API d_usf_updated_gprs;
  API d_win_start_gprs;     // (map?) BTS physical timeslot mapping.
  API a_ctrl_ched_gprs[8];  // (map?) Ched configuration, burst per burst
  API a_ul_buffer_gprs[8];  // (map?) UL burst / UL buffer mapping, burst per burst. (part of header)
  API a_usf_gprs[8];
  API a_interf_meas_gprs[8];

  API d_background_enable;
  API d_background_abort;
  API d_background_state;
  API d_max_background;
  API a_background_tasks[16];
  API a_back_task_io[10];

  #if (CODE_VERSION != SIMULATION)
  API a_hole3_gprs[19];
  #endif
  API d_gea_mode;

  API a_gea_kc[4];

  API d_sched_mode_gprs;

  #if (CODE_VERSION != SIMULATION)
  API a_hole4_gprs[384];
  #endif

  API a_du_gprs[4][29];
  API d_rlcmac_rx_no_gprs;
  API a_dd_gprs[4][31];
  API a_pu_gprs[4][15];

  #if (CODE_VERSION != SIMULATION)
  API a_hole5_gprs[112];
  #endif

  API a_ptcchu_gprs[4];
  API a_dd_md_gprs[16];
}
T_NDB_MCU_DSP_GPRS;
#endif

#if (DSP == 34) || (DSP == 35) || (DSP == 36) || (DSP == 37) || (DSP == 38) || (DSP == 39)
  typedef struct
  {
    // GPRS Extension...............
    API_SIGNED d_overlay_rlcmac_cfg_gprs;
    API_SIGNED d_mac_threshold;
    API_SIGNED d_sd_threshold;
    API_SIGNED d_nb_max_iteration;
  }
  T_PARAM_MCU_DSP_GPRS;
#elif (DSP == 33)
  typedef struct
  {
    // GPRS Extension...............
    API_SIGNED d_overlay_rlcmac_cfg_gprs;
    API_SIGNED d_mac_threshold;
    API_SIGNED d_sd_threshold;
    API_SIGNED d_nb_max_iteration;
  }
  T_PARAM_MCU_DSP_GPRS;
#else
  typedef struct
  {
    // GPRS Extension...............
    API_SIGNED d_overlay_rlcmac_cfg_gprs;
    API_SIGNED d_lat_mcu_bridge;
    API_SIGNED d_lat_pll2div;
    API_SIGNED d_lat_mcu_hom2sam;
    API_SIGNED d_gprs_install_address;
    API_SIGNED d_mac_threshold;
    API_SIGNED d_sd_threshold;
    API_SIGNED d_nb_max_iteration;
  }
  T_PARAM_MCU_DSP_GPRS;
#endif

/***************************************************************************************/
/* L1PS -> DSP communication structure...                                              */
/***************************************************************************************/
typedef struct
{
  T_DB_DSP_TO_MCU_GPRS  *pdsp_db_r_ptr;   // MCU<->DSP comm. read  page (Double Buffered comm. memory).
  T_DB_MCU_TO_DSP_GPRS  *pdsp_db_w_ptr;   // MCU<->DSP comm. write page (Double Buffered comm. memory).
  T_NDB_MCU_DSP_GPRS    *pdsp_ndb_ptr;    // MCU<->DSP comm. read/write (Non Double Buffered comm. memory).
  T_PARAM_MCU_DSP_GPRS  *pdsp_param_ptr;  // MCU<->DSP comm. read/write (Param comm. memory).
}
T_L1PS_DSP_COM;

/***************************************************************************************/
/* L1PS -> MAC-S communication structure...                                            */
/***************************************************************************************/

typedef struct
{
  UWORD8  nb;      // Number of error found
  UWORD8  id[10];  // Error ID
}
T_MACS_STATUS;

typedef struct
{
  BOOL            new_set;                 // Set to 1 if allocation parameters have changed, reset by MAC-S
  UWORD8          rx_allocation;           // Downlink resources relative to Layer 1 frames
  UWORD8          tx_nb_allocation;        // Uplink resources used to transmit normal bursts relative to Layer 1 frames
  UWORD8          tx_prach_allocation;     // Uplink resources used to transmit PRACH bursts relative to Layer 1 frames
#if FF_L1_IT_DSP_USF
    UWORD8        usf_status;              // USF uncertainty status
#endif
  UWORD8          pwr_allocation;          // Measurement gap relative to Layer 1 frames
  BOOL            fix_alloc_exhaust_flag;  // Set to 1 if the fixed allocation has just exhausted
  BOOL            rlc_downlink_call;
  T_MACS_STATUS   macs_status;

#if L1_EDA
    //To cope in class 12 with FB/SB and some PDTCH task allocations (worst case)
    UWORD8    fb_sb_task_enabled;
    UWORD8    fb_sb_task_detect;
#endif
  #if FF_TBF
    //To cope with the spreading issue, the "rlc_downlink_bufferize_param" structure is used to
    //memorize parameters over a block period.
    //In the case below:
    // C|W R    |
    //  |C W R  |
    //  |  C W R|       TBF 1
    //  |    C W|R <------------
    //----------------------
    //  |      C|W R <--------------
    //  |       |C W R
    //  |       |  C W R       TBF 2
    //  |       |    C W R
    //                   ^
    //                   |
    //                   worst case where the rlc_downlink() function can be called
    //                   when spreading occurs

    //The case above depicts a new TBF assignment without change of the synchronization.
    //Due to spreading, the rlc_downlink() function call can be delayed over the block.
    //Therefore, as this function needs the assignment id parameter, that variable should
    //be memorized until the call of the rlc_downlink() function.

    T_READ_PARAM   rlc_downlink_bufferize_param;
  #endif
}
T_L1PS_MACS_COM;

#if FF_TBF
 //type for POLL in EGPRS/GPRS
  typedef struct
  {
    API a_du_xgprs[4][SIZE_GPRS_PL_BUFF];   // poll buffer a_pu_xgprs[4][15]
  }
  T_A_APU_XGPRS;

  typedef union
  {
    #if L1_EGPRS
      API     a_dd_egprs[NBR_BUFFER_EGPRS][SIZE_EGPRS_DL_BUFF];
    #endif
    API     a_dd_gprs[NBR_BUFFER_GPRS][SIZE_GPRS_DL_BUFF+1]; //add +1 in order to be aligned on 32 bits
  }
  T_XGPRS;

  typedef struct
  {
    API      d_rlcmac_rx_no_gprs;
    API      dl_status;
    T_XGPRS  buffer;
  }
  T_A_DL_XGPRS;

  typedef union
  {
    #if L1_EGPRS
      API     a_du_egprs[NBR_BUFFER_EGPRS][SIZE_EGPRS_UL_BUFF+1];
    #endif
    API     a_du_gprs[NBR_BUFFER_GPRS][SIZE_GPRS_UL_BUFF];
  }
  T_A_UL_XGPRS;

#endif

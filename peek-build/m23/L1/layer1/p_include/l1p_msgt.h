/************* Revision Controle System Header *************
 *                  GSM Layer 1 software 
 * L1P_MSGT.H
 *
 *        Filename l1p_msgt.h
 *  Copyright 2003 (C) Texas Instruments  
 *
 ************* Revision Controle System Header *************/

/* channels types */
#define L2_PCHANNEL_PCCCH    1
#define L2_PCHANNEL_PBCCH    2
#define L2_PCHANNEL_PPCH     3
#define L2_PCHANNEL_PEPCH    4
#define L2_PCHANNEL_PACCH    5


typedef struct
{
  UWORD8  txpwr;
  UWORD16 rand;
  UWORD16 channel_request_data;
  UWORD8  bs_prach_blks;
  UWORD8  access_burst_type;
}
T_MPHP_RA_REQ;

typedef struct
{
  UWORD32 fn;
  UWORD16  channel_request_data;
}
T_MPHP_RA_CON;

typedef struct
{
  UWORD8  pol_resp_type;
  UWORD8  channel_request_data[24];
  UWORD32 fn;
  UWORD8  timing_advance;
  UWORD8  txpwr;
}
T_MPHP_POLLING_RESPONSE_REQ;

typedef struct
{
  UWORD32 fn;
}
T_MPHP_POLLING_IND;


typedef struct
{
  UWORD8  ta;
  UWORD8  ta_index;
  UWORD8  ta_tn;
}
T_PACKET_TA;


typedef struct
{
  BOOL     present;
  UWORD32  absolute_fn;
}
T_TBF_STI;


typedef struct
{
  T_CHN_SEL            chan_sel;
  T_MOBILE_ALLOCATION  freq_list;
}
T_PACKET_FREQ_PARAM;


typedef struct
{
  UWORD8  ctrl_timeslot;
  UWORD8  bitmap_length;
  UWORD8  bitmap[127];
}
T_FIXED_ALLOC;


typedef struct
{
  UWORD8  usf_table[8];
  BOOL    usf_granularity;
}
T_DYNAMIC_ALLOC;


typedef struct
{
  UWORD8  tfi;
  UWORD8  timeslot_alloc;
}
T_DL_RESSOURCE_ALLOC;


typedef struct
{
  UWORD8           tfi;
  UWORD8           timeslot_alloc;
  T_DYNAMIC_ALLOC  dynamic_alloc;
  T_FIXED_ALLOC    fixed_alloc;
}
T_UL_RESSOURCE_ALLOC;

typedef struct
{
  UWORD8  p0;
  BOOL    bts_pwr_ctl_mode;
  BOOL    pr_mode;
}
T_DL_POWER_CTL;

typedef struct
{
  UWORD8                assignment_id;
  UWORD8                assignment_command;
  UWORD8                multislot_class;
  BOOL                  interf_meas_enable;
  BOOL                  pc_meas_chan;
  BOOL                  access_burst_type;
  T_PACKET_TA           packet_ta;
  T_DL_POWER_CTL        dl_pwr_ctl;
  UWORD8                tsc;
  T_PACKET_FREQ_PARAM   freq_param;
  T_TBF_STI             tbf_sti;
  UWORD8                mac_mode;
  T_DL_RESSOURCE_ALLOC  dl_ressource_alloc;
  T_UL_RESSOURCE_ALLOC  ul_ressource_alloc;
}
T_MPHP_ASSIGNMENT_REQ;


typedef struct
{
  BOOL       repeat_allocation;
  UWORD8     ts_override;
  T_TBF_STI  tbf_sti;
}
T_MPHP_REPEAT_UL_FIXED_ALLOC_REQ;


typedef struct
{
  UWORD8               assignment_id;
  UWORD8               purpose;
  BOOL                 pc_meas_chan;
  BOOL                 access_burst_type;
  T_PACKET_TA          packet_ta;
  T_DL_POWER_CTL       dl_pwr_ctl;
  UWORD8               tsc;
  T_PACKET_FREQ_PARAM  freq_param;
  T_TBF_STI            tbf_sti;
  UWORD8               timeslot_number;
  UWORD8               data_array[23];
}
T_MPHP_SINGLE_BLOCK_REQ;

typedef struct
{
  UWORD8               assignment_id;
  UWORD8               purpose;
  UWORD8               status;
  BOOL                 dl_error_flag;
  UWORD8               data_array[23];
}
T_MPHP_SINGLE_BLOCK_CON;

typedef struct
{
  UWORD8  assignment_id;
  UWORD8  timeslot_available;
}
T_MPHP_PDCH_RELEASE_REQ;


typedef struct
{
  UWORD8      assignment_id;
  T_PACKET_TA packet_ta;
}
T_MPHP_TIMING_ADVANCE_REQ;


typedef struct
{
  UWORD8  tbf_type;
}
T_MPHP_TBF_RELEASE_REQ;

/****************************************************************/
/* Structure definition for L3 <-> L1 messages                  */
/****************************************************************/

// Packet Channel Description: arfcn/Hopping description + TN + TSC
typedef struct
{
  T_CHN_SEL chan_sel;
  UWORD8    timeslot_no;
  UWORD8    tsc;
} 
T_PACKET_CHANNEL_DESC;

typedef struct
{
  UWORD16                 imsimod; 
  UWORD16                 kcn; 
  UWORD16                 split_pg_cycle;
  UWORD8                  bs_pag_blks_res;
  UWORD8                  bs_pbcch_blks;
  UWORD8                  pb;
  UWORD8                  page_mode;
  T_PACKET_CHANNEL_DESC   packet_chn_desc;
  T_MOBILE_ALLOCATION     frequency_list;  
}
T_MPHP_START_PCCCH_REQ;

typedef struct
{
  UWORD16        radio_freq;
  UWORD8         l2_channel;
  BOOL           error_flag;
  T_RADIO_FRAME  l2_frame;
  UWORD8         relative_position;
  WORD8          pccch_lev;
  UWORD32        fn;
}
T_MPHP_DATA_IND;

typedef struct
{
  UWORD8                  nbr_psi;
  UWORD8                  bs_pbcch_blks;
  UWORD8                  pb;
  UWORD8                  psi1_repeat_period;
  UWORD8                  relative_position_array[20];
  T_PACKET_CHANNEL_DESC   packet_chn_desc;
  T_MOBILE_ALLOCATION     frequency_list;
}
T_MPHP_SCELL_PBCCH_REQ;

typedef struct
{
  UWORD8  nb_carrier;
  UWORD16 radio_freq_no[33];
  UWORD8  list_id;
}
T_MPHP_CR_MEAS_REQ;

typedef struct
{
  WORD8   rxlev;
}
T_NCELL_MEAS;

typedef struct
{
  UWORD8        nmeas;
  T_NCELL_MEAS  ncell_meas[33];
  UWORD8        list_id;
  UWORD16       reporting_period;
}
T_MPHP_CR_MEAS_IND;

typedef struct
{
  WORD16        acc_level [33];
  UWORD8        acc_nbmeas[33];
  UWORD8        list_id;
}
T_MPHP_TCR_MEAS_IND;

typedef struct
{
  T_PACKET_FREQ_PARAM     packet_intm_freq_param;
  UWORD8                  carrier_id;
  UWORD8                  tn;
  UWORD8                  multislot_class;
}
T_MPHP_INT_MEAS_REQ;

typedef struct
{
  WORD8      rxlev[2];
}
T_INT_MEAS;

typedef struct
{
  T_INT_MEAS  int_meas[8];
  UWORD8      id;
}
T_MPHP_INT_MEAS_IND;

typedef struct
{
  WORD8       rxlev[8];
  UWORD32     fn;
  UWORD8      position;
  UWORD8      meas_bitmap;
} T_L1P_ITMEAS_IND;

typedef struct
{
  UWORD8                  bs_pbcch_blks;
  UWORD8                  pb;
  UWORD8                  psi1_repeat_period;
  UWORD8                  relative_position;
  T_PACKET_CHANNEL_DESC   packet_chn_desc;
  T_MOBILE_ALLOCATION     frequency_list;
  UWORD16                 bcch_carrier;
  UWORD32                 fn_offset;
  UWORD32                 time_alignment;
}
T_MPHP_NCELL_PBCCH_REQ;

typedef struct
{
  UWORD8  pb;
  BOOL    access_burst_type;
}
T_MPHP_UPDATE_PSI_PARAM_REQ;

typedef struct
{
  BOOL        released_all;
  UWORD8      tbf_type;
  WORD8       tn_difference;
} T_L1P_TBF_RELEASED;

typedef struct
{
  UWORD8 tbf_type;
} T_MPHP_TBF_RELEASE_CON;

typedef struct
{
  UWORD8      assignment_id;
} T_MPHP_PDCH_RELEASE_CON;

typedef struct
{
  UWORD8      assignment_id;
  WORD8       tn_difference;
  BOOL        Transfer_update;    // Detect transition IDLE => Packet Transfer
} T_L1P_TRANSFER_DONE;

typedef struct
{
  UWORD8      assignment_id;
  WORD8       tn_difference;
} T_L1P_PDCH_RELEASE_CON;

typedef struct
{
  WORD8       tn_difference;
} T_L1P_REPEAT_ALLOC_DONE;

typedef struct
{
  WORD8       tn_difference;
} T_L1P_ALLOC_EXHAUST_DONE;

typedef T_MPHP_DATA_IND          T_MPHP_NCELL_PBCCH_IND; 
typedef T_MPHP_CR_MEAS_IND       T_L1P_CR_MEAS_DONE;
typedef T_MPHP_TCR_MEAS_IND      T_L1P_TCR_MEAS_DONE;
typedef T_MPHP_CR_MEAS_REQ       T_MPHP_TCR_MEAS_REQ;
typedef T_MPHP_PDCH_RELEASE_CON  T_MPHP_TIMING_ADVANCE_CON;
typedef T_MPHP_PDCH_RELEASE_CON  T_MPHP_ASSIGNMENT_CON;

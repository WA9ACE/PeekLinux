/************* Revision Controle System Header *************
 *                  GSM Layer 1 software
 * L1TM_DEFTY.H
 *
 *        Filename l1tm_defty.h
 *  Copyright 2003 (C) Texas Instruments
 *
 ************* Revision Controle System Header *************/


/*****************************************
 * Selection of Testmode operating mode: *
 ****************************************/

  #define ETM_MODE        0              // ETM_MODE DISABLED

/****************************************/
  
  #if (ETM_MODE == 0)
    #define ETM_PROTOCOL    0            // PCTM-TM3 protocol active
  #elif (ETM_MODE == 1)
    #define ETM_PROTOCOL    1            // ETM protocol active
  #else
    #define ETM_PROTOCOL    0            // PCTM-TM3 protocol active
  #endif

#if (L1_TPU_DEV == 1)
#define NB_TPU_TIMINGS 32

//Flexi ABB Delays
#define NB_ABB_DELAYS 32
#endif

// Max size of data portion of a testmode packet
#if (ETM_PROTOCOL == 1)

  // MID, STATUS, CID, INDEX  and CHECKSUM
//  #define TM_UPLINK_PACKET_OVERHEAD 5
  // MID, STATUS and CHECKSUM
  #define TM_UPLINK_PACKET_OVERHEAD 5                                        // (MID+CID+INDEX+CKSUM+STATUS)
  #define TM_PAYLOAD_UPLINK_SIZE_MAX     (256+3-TM_UPLINK_PACKET_OVERHEAD)   //(256 + STATUS + MID + CKSUM - (MID+CID+INDEX+CKSUM))

  #define TM_DOWNLINK_PACKET_OVERHEAD 4                                      // (MID+CID+INDEX+CKSUM)
  #define TM_PAYLOAD_DOWNLINK_SIZE_MAX   (256+2-TM_DOWNLINK_PACKET_OVERHEAD) //(256 + MID + CKSUM - (MID+CID+INDEX+CKSUM))

  #define ETM_RF    0x05                                                     // MID for RF DLL
  #define ETM_CORE  0x01                                                     // MID for RF DLL
    
#else

// CID, STATUS and CHECKSUM
#define TM_UPLINK_PACKET_OVERHEAD 3
  #define TM_PAYLOAD_UPLINK_SIZE_MAX   128

  #define TM_DOWNLINK_PACKET_OVERHEAD 3
  #define TM_PAYLOAD_DOWNLINK_SIZE_MAX   128

#endif
/*  Add Repeated FACCH mode flag. */
#if (FF_REPEATED_DL_FACCH == 1)
/* Repeated Mode setting: 0=>OFF, 1=>ON */
#define REPEATED_FACCHDL_ENABLE 1
#endif

#if (FF_REPEATED_SACCH == 1 )
  #define  REPEATED_SACCH_ENABLE  1
#endif /*  FF_REPEATED_SACCH */



typedef struct
{
  UWORD32   toa_sum;
  UWORD32   toa_sq_sum;
  UWORD32   toa_recent;
  UWORD32   pm_sum;
  UWORD32   pm_sq_sum;
  UWORD16   pm_recent;
  WORD16    angle_sum;
  UWORD32   angle_sq_sum;
  WORD16    angle_min;
  WORD16    angle_max;
  WORD16    angle_recent;
  UWORD32   snr_sum;
  UWORD32   snr_sq_sum;
  UWORD32   snr_recent;
  UWORD8    rssi_fifo[4];
  UWORD8    rssi_recent;
  WORD32    loop_count;  //Must be signed for divide operation in statistics!!!
  WORD32    flag_count;  //Must be signed for divide operation in statistics!!!
  UWORD32   flag_recent;
  UWORD8    bsic;
  UWORD32   fn;
  UWORD32   qual_acc_full;      // Fullset: accu. rxqual meas.
  UWORD32   qual_nbr_meas_full; // Fullset: nbr meas. of rxqual.
  #if L1_GPRS
   UWORD8    nb_dl_pdtch_slots;
   //stats bler
   UWORD16 bler_crc[4];
   UWORD16 bler_total_blocks;
  #endif
}
T_TM_STATS;

typedef struct
{
  UWORD8  dedicated_active;
  UWORD32 rx_counter;
  UWORD16 num_bcchs;
  #if L1_GPRS
    BOOL packet_transfer_active;
  #endif
}
T_TM_STATE;

typedef struct
{
  UWORD16 prbs1_seed;
  // UWORD16 prbs2_seed; // for future use
} T_TM_PRBS;


#if ((L1_STEREOPATH == 1) && (OP_L1_STANDALONE == 1))
 typedef struct
 {
  WORD8*   stereopath_pattern;             // sinusoide or silence
  UWORD16  stereopath_nb_samples;          // nb of valid samples in the pattern
  UWORD16  stereopath_current_sample;
  UWORD8   stereopath_buffer_number;
  UWORD16  stereopath_source_timeout;
  UWORD16  stereopath_dest_timeout;
  UWORD16  stereopath_drop;
  UWORD16  stereopath_frame;
  UWORD16  stereopath_block;
  UWORD16  stereopath_half_block;
 }
 T_TM_STEREOPATH_DRV_TASK;
#endif


// Global TM variable
typedef struct
{
  BOOL       tm_msg_received;
  T_TM_STATS tmode_stats;
  T_TM_STATE tmode_state;
  T_TM_PRBS  tmode_prbs;
  #if ((L1_STEREOPATH == 1) && (OP_L1_STANDALONE == 1))
    T_TM_STEREOPATH_DRV_TASK stereopath;
  #endif
}
T_L1TM_GLOBAL;

typedef struct
{
  UWORD16     bcch_arfcn;
  UWORD16     tch_arfcn;
  UWORD16     mon_arfcn;
  #if L1_GPRS
    UWORD16     pdtch_arfcn;
    UWORD8      multislot_class;
  #endif
  UWORD8      down_up;
  UWORD8      channel_type;
  UWORD8      subchannel;
  UWORD8      tmode_continuous;
  UWORD8      reload_ramps_flag;
  BOOL        mon_report; //Used to determine wether RX stats are done in Monitor channel or TCH channel 
  BOOL        mon_tasks;  //Used to enable tasks associated with Monitor Channel
  #if ( L1_TPU_DEV == 1)
   WORD16 *p_rf_flexi_abb_delays; //Initialize this to point to rf_rx_tpu_timings Array
  #endif
}
T_TM_RF_PARAMS;

typedef struct
{
  UWORD8      slot_num;
  WORD8       agc;
  WORD8       lna_off;
  UWORD8      number_of_measurements;
  UWORD8      place_of_measurement;
  BOOL        pm_enable;
  UWORD8      rssi_band;
  #if L1_GPRS
    UWORD8      timeslot_alloc;
    UWORD8      coding_scheme;
  #endif
  #if (L1_TPU_DEV == 1)
    WORD16 *p_rf_rx_tpu_timings; //Initialize to point to rf_tx_tpu_timings Array
  #endif
}
T_TM_RX_PARAMS;

typedef struct
{
  UWORD8      tsc;
  UWORD8      txpwr;
  UWORD8      txpwr_skip;
  UWORD8      timing_advance;
  UWORD8      burst_type;
  UWORD8      burst_data;
  #if L1_GPRS
    UWORD8      timeslot_alloc;
    UWORD8      txpwr_gprs[8];
    UWORD8      coding_scheme;
    UWORD8      rlc_buffer_size;
    UWORD16     rlc_buffer[27];
  #endif
  #if (L1_TPU_DEV == 1)
    WORD16 *p_rf_tx_tpu_timings; //Initialize to point to rf_tx_tpu_timings Array
  #endif
    #if ((REL99 == 1) && (FF_BHO == 1))
      BOOL        power_reduction_enable;  // uplink power reduction feature
    #endif

}
T_TM_TX_PARAMS;

typedef struct
{
  UWORD32     num_loops;
  UWORD32     auto_result_loops;
  UWORD32     auto_reset_loops;
  UWORD8      stat_type;
  UWORD16     stat_bitmask;
  #if L1_GPRS
    UWORD8      stat_gprs_slots;
  #endif
}
T_TM_STATS_CONFIG;

typedef struct 
{
  T_TM_RF_PARAMS    rf_params;
  T_TM_RX_PARAMS    rx_params;
  T_TM_TX_PARAMS    tx_params;
  T_TM_STATS_CONFIG stats_config;
}
T_TM_PARAMS;

typedef struct
{
#if (ETM_PROTOCOL == 1)
  UWORD8  mid;
#endif
  UWORD8  cid;
  UWORD8  index;
  UWORD8  status;
  UWORD16 size;  // size of result[] array
  UWORD8  result[TM_PAYLOAD_UPLINK_SIZE_MAX];
}
T_TM_RETURN;

typedef struct
{
#if (ETM_PROTOCOL == 1)
  UWORD8  mid;
#endif
  UWORD8  cid;
  UWORD8  index;
  UWORD8  status;
  UWORD16 size;  // size of result[] array
  UWORD8  result[5];
}
T_TM_RETURN_ABBREV;

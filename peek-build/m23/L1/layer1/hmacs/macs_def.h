/************* Revision Control System Header *************
 *                  GSM Layer 1 software
 * MACS_DEF.H
 *
 *        Filename macs_def.h
 *  Copyright 2003 (C) Texas Instruments  
 *
 ************* Revision Control System Header *************/

 /*********************************************************/
 /* Type definition for MAC-S static variables            */
 /*********************************************************/

/*---------------------------------------------------*/
/* MAC-S static parameters                           */
/*---------------------------------------------------*/

#if FF_TBF
  //number of buffer used to memorize NDB buffer: data is then available
  //over (NBR_SHARED_BUFFER_RLC-1) block period for RLC layer
  #define NBR_SHARED_BUFFER_RLC     3
#endif
typedef struct {

/* General TBF parameters processed by MAC-S */
/*********************************************/
    UWORD8                old_synchro_ts;        // Synchro timeslot when data was received on the slots indicated in last_rx_allocation

// Dynamic allocation mode parameters
    UWORD8                first_monitored_ts;    // First Tx slot allocated by the network
    UWORD8                last_monitored_ts;     // Last Tx slot allocated by the network

// Fixed allocation mode parameters
    UWORD8                dl_ctrl_ts_mask;       // Mask used to verify if conditions are met to map the downlink control timeslot
    UWORD32               sti_block_id;          // Block ID of the starting time

/* Ressources allocated by MAC-S             */
/*********************************************/
    UWORD8                rx_allocation;         // Slots allocated for downlink blocks reception
    UWORD8                tx_allocation;         // Slots allocated for uplink blocks transmission
    UWORD8                tx_data;               // Slots allocated for uplink data block transfer only
    UWORD8                tx_prach_allocation;   // Slots allocated for poll response in PRACH burst format
    UWORD8                pwr_allocation;        // Slots allocated for power measurements
    UWORD8                last_rx_alloc;         // Downlink resources allocated during the last block period
    #if L1_EDA
      UWORD8            monitored_ts[4];       // Array containing the timeslots to monitor for USF 
                                               // (ts may not have contiguous values)
      UWORD8            rx_monitored;          // Slots allocated for monitoring
      UWORD8            lowest_poll_ts;        // Lowest poll timeslot
      UWORD8            last_rx_monitored;     // Last monitored timeslots in previous block period
    #endif
  #if FF_L1_IT_DSP_USF
    UWORD8              dsp_usf_interrupt;     // DSP interrupt for USF decoding needed
  #endif

/* MAC-S global parameters */
/***************************/
    UWORD32               rx_blk_period;         // Set to BLOCK_ID if DL blocks were received, FFFFFFFF if no DL blocks
    UWORD8                rlc_blocks_sent;       // Last sent uplink RLC/MAC block ID
    UWORD8                rx_no;                 // Number of received blocks to report to RLC
    UWORD8                last_poll_response;    // Status of the poll responses requested by RLC the block period before
    UWORD8                ul_buffer_index[8];    // UL buffer index table loaded after RLC UL allocation check   

// Dynamic allocation mode parameters     
    UWORD8                usf_vote_enable;       // This bitmap is used to set on which timeslot the DSP USF vote must be enabled
    UWORD8                usf_good;              // Bit set to 1 corresponds to invalid or valid and good USF values
    UWORD32               next_usf_mon_block[2]; // For each uplink block, memorize the next block_id when the USF will be monitored
                                                 // Note: for type 1 MS, max. 2 Tx can be done !!!

// Fixed allocation mode parameters
    UWORD8                tra_gap;               // Number of free Tx slots at the end of the frame
    BOOL                  fix_alloc_exhaust;     // Set to 1 if the fixed allocation bitmap has exhausted

  #if MACS_STATUS
    BOOL                  bcch_monitoring;       // When MAC-S calls RLC, set to TRUE if BCCH monitoring was done during the frame before
  #endif

  #if FF_TBF
    T_A_DL_XGPRS          rlc_dbl_buffer[NBR_SHARED_BUFFER_RLC];
    UWORD8                dl_buffer_index;
    UWORD8                tdma_delay;
    UWORD8                dl_missing_payload;
  #endif
} T_MACS_STATIC;

/*---------------------------------------------------*/
/* MS Class parameters                               */
/*---------------------------------------------------*/

typedef struct {
    UWORD8 rx;
    UWORD8 tx;
    UWORD8 sum;
    UWORD8 tta;
    UWORD8 ttb;
    UWORD8 tra;
    UWORD8 trb;
} T_MSCLASS_PARAMETERS;

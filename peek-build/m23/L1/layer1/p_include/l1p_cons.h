/************* Revision Controle System Header *************
 *                  GSM Layer 1 software
 * L1P_CONS.H
 *
 *        Filename l1p_cons.h
 *  Copyright 2003 (C) Texas Instruments  
 *
 ************* Revision Controle System Header *************/

// TBF allocations...
#define  DL_TBF           0
#define  UL_TBF           1
#define  BOTH_TBF         2
#define  SINGLE_BLOCK_DL  3
#define  SINGLE_BLOCK_UL  4
#define  TWO_PHASE_ACCESS 5
#define  NO_TBF           6

// MAC modes...
#define  DYN_ALLOC         0
#define  EXT_DYN_ALLOC     1
#define  FIX_ALLOC_NO_HALF 2
#define  FIX_ALLOC_HALF    3

//added
#define  TBF_MODE_GPRS     0
// First task after the Idle frame...
#define  RX_TASK           1
#define  TX_TASK           2

// Status for interference measurement frame
#define ANY_IDLE_FRAME     0
#define PTCCH_FRAME        1
#define SEARCH_FRAME       2

// No measurement status
#define NO_MEAS      0x80

// Multislot bit of BBCTRL ABB register to set the multislot mode
#if (ANLG_FAM == 1)
  #define B_MSLOT (0x40<<6)
#endif
#if ((ANLG_FAM == 2) || (ANLG_FAM == 3))
  #define B_MSLOT (0x20<<6)
#endif
#if FF_TBF  
    extern  API* const A_DD_XGPRS[1][4];
    extern  API* const A_DU_XGPRS[1][4];
#endif

#if FF_L1_IT_DSP_USF
  #define USF_AVAILABLE     0
  #define USF_AWAITED       1
  #define USF_IT_DSP        2
#endif

//----------------------------------------
// LAYER 1 Asynchronous processes names...
//----------------------------------------
#define NBR_L1PA_PROCESSES       11

#define PI_SCP                   0   // l1pa_idle_paging_process(msg)
#define TRANSFER                 1   // l1pa_transfer_process(msg)
#define P_ACC                    2   // l1pa_access_process(msg)
#define P_POLL                   3   // l1pa_idle_packet_polling_process(msg)
#define SCPB                     4   // l1pa_serving_cell_pbcch_read_process(msg)
#define CR_MEAS                  5   // l1pa_cr_meas_process(msg)
#define TCR_MEAS                 6   // l1pa_tcr_meas_process(msg)
#define PI_INT_MEAS              7   // l1pa_idle_interference_meas_process(msg)
#define PT_INT_MEAS              8   // l1pa_transfer_interference_meas_process(msg)
#define NCPB                     9   // l1pa_neighbor_cell_pbcch_read_process(msg)
#define PI_SMSCB                10   // l1pa_idle_smscb_process(msg)

// Constants for PRACH
#if !FF_TBF
  // moved to l1_const.h for EGPRS as also needed for RACH handling
#define ACC_BURST_8              0
#define ACC_BURST_11             1
#endif

#define DYN_PRACH_ALLOC        1
#define FIX_PRACH_ALLOC        2

// DSP CS types (CHED)
//TABLE/ UL CS
#define CS_NONE_TYPE    0  //NAME/ No block
#define CS_AUTO_DETECT  1  //NAME/ N/A
#define CS1_TYPE_DATA   2  //NAME/ CS1
#define CS1_TYPE_POLL   3  //NAME/ Poll NB
#define CS2_TYPE        4  //NAME/ CS2
#define CS3_TYPE        5  //NAME/ CS3
#define CS4_TYPE        6  //NAME/ CS4
#define CS_PAB8_TYPE    7  //NAME/ PRACH 8bit
#define CS_PAB11_TYPE   8  //NAME/ PRACH 11bit
//END_TABLE/
#if FF_TBF
#define CS_GPRS_MASK     0x000F

  #define CS_EGPRS_MASK     0x1F

  //Value used for acknowledge of the RLC block
  #define RLC_BLOCK_ACK   0xff

  //Number of buffers
  #define NBR_BUFFER_GPRS    4

  //all size are expressed in API words
  //size of "header" in NDB
  #define SIZE_DSP_HEADER_GPRS        4
  //size of payload depending on coding scheme
  #define SIZE_PAYLOAD_CS1           12
  #define SIZE_PAYLOAD_CS2           17
  #define SIZE_PAYLOAD_CS3           20
  #define SIZE_PAYLOAD_CS4           27
  #if L1_EGPRS
    #define SIZE_PAYLOAD_MCS1          14
    #define SIZE_PAYLOAD_MCS2          17
    #define SIZE_PAYLOAD_MCS3          21
    #define SIZE_PAYLOAD_MCS4          25
    #define SIZE_PAYLOAD_MCS5          32
    #define SIZE_PAYLOAD_MCS6          41
    #define SIZE_PAYLOAD_MCS7          61
    #define SIZE_PAYLOAD_MCS8          73
    #define SIZE_PAYLOAD_MCS9          79
  #endif
  //Max size of each buffer in API word
  //GPRS
  #define SIZE_GPRS_DL_BUFF         (SIZE_PAYLOAD_CS4+SIZE_DSP_HEADER_GPRS)     //(27+4=31)
  #define SIZE_GPRS_UL_BUFF         29
  #define SIZE_GPRS_PL_BUFF         15
  #ifdef MACS_C
  #else
    extern UWORD8 SIZE_PAYLOAD[];
  #endif // MACS
#endif // FF_TBF
  #define IR_OUT_OF_MEMORY       0

// USF decoding for PRACH
#define USF_INVALID              0
#define USF_GOOD                 1
#define USF_BAD                  2
#define USF_FREE                 7

// DSP tasks
#define DL_PDSP_TASK       2  // Downlink task (Normal burst or Prach).
#define UL_PDSP_TASK       2  // Uplink task (Normal burst or Prach).
#define PB_PDSP_TASK       3  // Power measurement Burst task.

// DSP tasks used in d_task_md
#define INTERF_DSP_TASK    100  // Interference measurements
#define INTERF1_DSP_TASK   101  // 1 Interference measurement
#define INTERF2_DSP_TASK   102  // 2 Interference measurement
#define INTERF3_DSP_TASK   103  // 3 Interference measurement
#define INTERF4_DSP_TASK   104  // 4 Interference measurement
#define INTERF5_DSP_TASK   105  // 5 Interference measurement
#define INTERF6_DSP_TASK   106  // 6 Interference measurement
#define INTERF7_DSP_TASK   107  // 7 Interference measurement
#define INTERF8_DSP_TASK   108  // 8 Interference measurement
#define PTCCHD_DSP_TASK    109  // PTCCH DL
#define PTCCHU_DSP_TASK    110  // PTCCH UL
#define PTCCHDU_DSP_TASK   111  // PTCCH DL and UL

//---------------------------------------------
// PTCCH activities
//---------------------------------------------
#define PTCCH_DL_BIT          0  // PTCCH DL bit position
#define PTCCH_UL_BIT          1  // PTCCH UL bit position

#define PTCCH_DL              (TRUE_L << PTCCH_DL_BIT)
#define PTCCH_UL              (TRUE_L << PTCCH_UL_BIT)

//---------------------------------------------
// SINGLE activities
//---------------------------------------------
#define ALL_SINGLE          0xFF
#define SINGLE_DL_BIT          0  // SINGLE DL bit position
#define SINGLE_UL_BIT          1  // SINGLE UL bit position

#define SINGLE_DL              (TRUE_L << SINGLE_DL_BIT)
#define SINGLE_UL              (TRUE_L << SINGLE_UL_BIT)

#define SINGLE_DL_MASK         ALL_TASK ^ SINGLE_DL
#define SINGLE_UL_MASK         ALL_TASK ^ SINGLE_UL

//---------------------------------------------
// Status for MPHP_SINGLE_BLOCK_CON
//---------------------------------------------
#define SINGLE_UL_DONE     0
#define SINGLE_STI_PASSED  1
#define SINGLE_NO_TA       2
#define SINGLE_DL_DONE     3


//---------------------------------------------
// MCU-DSP bit-field bit position definitions
//---------------------------------------------
// d_task_u_gprs...
#define B_ACCESS_PRACH   13
#define B_PTCCH_UL       14

// d_task_d_gprs...
#define B_PTCCH_DL       14

// d_sched_mode_gprs...
#define B_SWITCH         0   // Bit 0: switch to GPRS, Bit 1: switch to GSM.
#define B_MAC_MODE       2
#define B_RIF_RX_MODE    5

// a_ctrl_abb_gprs or d_ptcchu_ctrl_abb_gprs...
#define B_RAMP_GPRS      0
#define B_APCDEL2_GPRS   2
#define B_APCDEL1_GPRS   3
#define B_AFC_GPRS       4
#define B_RAMP_NB_GPRS   5
#define B_MS_RULE        8  // set an additionnal interrupt for the DSP

#if FF_L1_IT_DSP_USF
    // d_usf_vote_enable
    #define B_USF_IT           15
  #endif

//---------------------------------------------
// LAYER 1 PACKET PERIODIC MEASUREMENT TASKS...
//---------------------------------------------
#define P_CRMS      0                             // Packet Periodic Measurements task in Idle mode.
#define P_TCRMS     1                             // Neighbour Measurement in Packet Transfer mode.

#define P_CRMS_MEAS  (TRUE_L << P_CRMS)           // Set Packet Periodic Measurements task
#define P_TCRMS_MEAS (TRUE_L << P_TCRMS)          // Set Neighbour Measurement Packet Transfer task

#define P_CRMS_MEAS_MASK  ALL_TASK ^ P_CRMS_MEAS  // Mask Packet Periodic Measurement task
#define P_TCRMS_MEAS_MASK ALL_TASK ^ P_TCRMS_MEAS // Mask Neighbour Measurement Packet Transfer task

//--------------------------------------------
// Paging macro definition
//--------------------------------------------
//-- Paging States used for PPCH reading blocks
#define PPCH_POS_NOT_COMP 0
#define PPCH_POS_COMP     1

//-- Maximum Number of Packet Paging Blocks
#define MAX_NBR_PG_BLKS   11

//-- Paging Block index max
#define MAX_PG_BLKS_INDEX 10

//--------------------------------------------
// PBCCH macro definition
//--------------------------------------------
//-- Maximum Number of PBCCH Blocks
#define MAX_NBR_PB_BLKS   4

//-- PBCCH index max
#define MAX_PB_BLKS_INDEX 3

/*--------------------------------------------------------*/
/* Position of different blocs in a MF52.                 */
/*--------------------------------------------------------*/
#define PCCCH_0            0
#define PCCCH_1            4
#define PCCCH_2            8
#define PCCCH_3           13
#define PCCCH_4           17
#define PCCCH_5           21
#define PCCCH_6           26
#define PCCCH_7           30
#define PCCCH_8           34
#define PCCCH_9           39
#define PCCCH_10          43
#define PCCCH_11          47

//-- PBCCH block position
#define B0_POSITION        0L
#define B11_POSITION      47L

// Power measurement constants
// mode for power measurements
#define PACKET_IDLE         1
#define PACKET_TRANSFER     2
// number of meas
#define NB_MEAS_PACKET_IDLE 4  // Normal case 1RX + 3PW, if no RX=> 4PW

// TX burst types
#define TX_NB_BURST    0
#define TX_RA_BURST    1

// No power control packet transfer AGC algorithm phases
#define SEARCH         0
#define TRACK          1

// Stats BLER
#define B_CRC_BLOCK_ERROR 8

/*--------------------------------------------------------*/
/* API addresses.........................                 */
/*--------------------------------------------------------*/
#define DSP_API_ADDRESS_BASE          0x00000800L   //
#define ARM_API_ADDRESS_BASE          0xFFD00000L   //

// Herebelow we define the MCU/DSP interface addresses as seen
// by the DSP (DSP address space) considering address 0 basis.

#if ((DSP == 33) || (DSP == 34) || (DSP == 35) || (DSP == 36) || (DSP == 37) || (DSP == 38) || (DSP == 39))
  #define DSP_MAP_DB_W_PAGE_0_GPRS      0x00000050L   //
  #define DSP_MAP_DB_W_PAGE_1_GPRS      0x00000064L   //
  #define DSP_MAP_DB_R_PAGE_0_GPRS      0x00000078L   //
  #define DSP_MAP_DB_R_PAGE_1_GPRS      0x0000009CL   //
  #define DSP_MAP_NDB_ADR_GPRS          0x000001AEL   //
  #define DSP_MAP_PARAM_ADR_GPRS        0x00000480L   //
#else
  #define DSP_MAP_DB_W_PAGE_0_GPRS      0x000004ADL   //
  #define DSP_MAP_DB_W_PAGE_1_GPRS      0x000004C1L   //
  #define DSP_MAP_DB_R_PAGE_0_GPRS      0x000004D5L   //
  #define DSP_MAP_DB_R_PAGE_1_GPRS      0x000004F9L   //
  #define DSP_MAP_NDB_ADR_GPRS          0x00000056L   //
  #define DSP_MAP_PARAM_ADR_GPRS        0x000001F1L   //
#endif

// Herebelow we define the MCU/DSP interface addresses as seen
// by the MCU (ARM address space) considering .

#define DB_W_PAGE_0_GPRS   (ARM_API_ADDRESS_BASE + (DSP_MAP_DB_W_PAGE_0_GPRS * 2))   //
#define DB_W_PAGE_1_GPRS   (ARM_API_ADDRESS_BASE + (DSP_MAP_DB_W_PAGE_1_GPRS * 2))   //
#define DB_R_PAGE_0_GPRS   (ARM_API_ADDRESS_BASE + (DSP_MAP_DB_R_PAGE_0_GPRS * 2))   //
#define DB_R_PAGE_1_GPRS   (ARM_API_ADDRESS_BASE + (DSP_MAP_DB_R_PAGE_1_GPRS * 2))   //
#define NDB_ADR_GPRS       (ARM_API_ADDRESS_BASE + (DSP_MAP_NDB_ADR_GPRS * 2))       //
#define PARAM_ADR_GPRS     (ARM_API_ADDRESS_BASE + (DSP_MAP_PARAM_ADR_GPRS * 2))     //

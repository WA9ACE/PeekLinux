/************* Revision Control System Header *************
 *                  GSM Layer 1 software
 * MACS_CST.H
 *
 *        Filename macs_cst.h
 *  Copyright 2003 (C) Texas Instruments  
 *
 ************* Revision Control System Header *************/

 /*********************************************************/
 /*  Copyright 2003 (C) Texas Instruments  */
 /*********************************************************/

//#define MACS_VERSION      0x01       // MAC-S validated version for 1 Rx - 1 Tx (no polling) - Power measurements in Dynamic mode
//#define MACS_VERSION      0x02       // MAC-S validated version for multi-slot - poll responses in dynamic mode
//#define MACS_VERSION      0x03       // TFI filtering, fixed allocation mode for 1 Rx - 1 Tx (no PDCH release, no repeat allocation) 
//#define MACS_VERSION      0x04       // Problem of MAC-S Read phase during Idle frames corrected - new MCU/DSP and new RLC/MAC interfaces (S921_bis v 0.7)
//#define MACS_VERSION      0x05       // Fixed allocation mode in multi-slot, control timeslot monitoring, PDCH release
//#define MACS_VERSION      0x06       // Tra parameter always respected -> no power between Rx and Tx
//#define MACS_VERSION      0x07       // PR field extracting in downlink blocks
//#define MACS_VERSION      0x08       // Fixed mode: Repeat allocation and allocation bitmap exhaustion management - BSIC setting in PRACH polling uplink blocks
#define MACS_VERSION      0x09       // Dynamic mode / USF granularity = 4 blocks rework

 /*********************************************************/
 /* Constants used by MAC-S                               */
 /*********************************************************/

/* Data processing */
/*-----------------*/

/* Invalid value */
#define INVALID           0xFF       // Non-significative value

/* No received DL blocks */
#define NO_DL_BLK         0xFFFFFFFF

/* USF management */
#define MASK_2SLOTS       0x3        // Mask used to read the d_usf_updated_gprs value

/* Timeslot test mask */
#define MASK_SLOT0        128        // Mask used to isolate slot 0 in a 8 bit word
#define MASK_ALL_SLOTS    0xFF       // Mask used to remove several timeslots

/* Multi-slot */
/*------------*/

/* Number of timeslot */
#define TS_NUMBER         8          // Number of timeslots
#define MAX_TS_NB         7          // Highest timeslot number

/* USF granularity */
#define FOUR_BLOCKS       1          // USF monitored on blocks B0, B4, B8
#define BLOCK             0          // USF monitored on each block

/* RX - TX delay */
#define RXTX_DELAY        3          // 3 timeslot dealy between downlink and uplink transfer window


/* RLC/MAC header */
/*----------------*/

/* Payload and AC field reading */
#define PAYLOAD_SHIFT     6          // Shift number used to isolate the payload type in the RLC/MAC header
#define PAYLOAD_MASK      0x0003
#define AC_MASK           0x0100     // Mask used to isolate the AC value in the RLC/MAC control block header

/* Payload type values for downlink RLC/MAC blocks */
//TABLE/ PAYLOAD
#define DATA_BLOCK        0 //NAME/ DATA
#define CTRL_NO_OPTIONAL  1 //NAME/ CTRL OPT
#define CTRL_OPTIONAL     2 //NAME/ CTRL NO OPT
#define RESERVED          3 //NAME/ RESERVED
//END_TABLE/

/* TFI field in data blocks */
#define DATA_TFI_MASK     0x3e00     // Mask used to isolate the TFI field in downlink data block after one right shift
#define DATA_TFI_SHIFT    9          // Shift number used to isolate the TFI field in downlink data block header

/* TFI field in control blocks */
#define CTRL_TFI_MASK     0x003e     // Mask used to isolate the TFI and D fields
#define CTRL_TFI_SHIFT    1          // Shift number used to isolate the TFI field in downlink control block header
#define MASK_D            0x0001     // Mask used to isolated the D bit

/* PR field extracting */
#define DATA_PR_SHIFT     14         // Data blocks
#define CTRL_PR_SHIFT     6          // Control blocks
#define PR_MASK           3          // PR <-> 2 bits

/* TFI filtering result */
/*----------------------*/

#define TFI_BIT_MASK      0xf9ff
#define TFI_BIT_SHIFT     9          // Shift number used to access the TFI filter bit in the block header stored in the NDB

//TABLE/ TFI RESULT
#define TFI_NOT_FILTERED  0 //NAME/ NO FILTERING
#define TFI_NOT_PRESENT   1 //NAME/ NO TFI   
#define TFI_GOOD          2 //NAME/ MS     
#define TFI_BAD           3 //NAME/ NOT MS   
//END_TABLE/

/* "time" values used when processing the power measurement gap */
#define RX_SLOT                 0xF0
#define TX_SLOT                 0xF1

#define MAX_CLASS         12

 /*********************************************************/
 /* Tables                                                */
 /*********************************************************/

#ifdef L1P_ASYN_C

  /* Multi-slot class parameters */
  /*-----------------------------*/
  const T_MSCLASS_PARAMETERS MS_CLASS[MAX_CLASS+1] =
  {
  /*-------------------------------------------------------------*/
  /*   RX    TX   SUM   TTA   TTB   TRA   TRB                    */
  /*-------------------------------------------------------------*/
      {0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0},    /* Unused      */
      {1  ,  1  ,  2  ,  3  ,  2  ,  4  ,  2},    /* MS Class 1  */
      {2  ,  1  ,  3  ,  3  ,  2  ,  3  ,  1},    /* MS Class 2  */
      {2  ,  2  ,  3  ,  3  ,  2  ,  3  ,  1},    /* MS Class 3  */
      {3  ,  1  ,  4  ,  3  ,  1  ,  3  ,  1},    /* MS Class 4  */
      {2  ,  2  ,  4  ,  3  ,  1  ,  3  ,  1},    /* MS Class 5  */
      {3  ,  2  ,  4  ,  3  ,  1  ,  3  ,  1},    /* MS Class 6  */
      {3  ,  3  ,  4  ,  3  ,  1  ,  3  ,  1},    /* MS Class 7  */
      {4  ,  1  ,  5  ,  3  ,  1  ,  2  ,  1},    /* MS Class 8  */
      {3  ,  2  ,  5  ,  3  ,  1  ,  2  ,  1},    /* MS Class 9  */
      {4  ,  2  ,  5  ,  3  ,  1  ,  2  ,  1},    /* MS Class 10 */
      {4  ,  3  ,  5  ,  3  ,  1  ,  2  ,  1},    /* MS Class 11 */
      {4  ,  4  ,  5  ,  2  ,  1  ,  2  ,  1}     /* MS Class 12 */
  };

  const UWORD8 PR_CONVERSION[2][4] =
  {
  /*-----------------------------------------------*/
  /* PR field                 00   01   10   11    */
  /*-----------------------------------------------*/     
  /* BTS_PWR_CTL_MODE A */  { 1  , 5  , 9  , 0  },
  /* BTS_PWR_CTL_MODE B */  { 3  , 11 , 19 , 27 }
  };

  const UWORD8 USF_BLOCK_GRANULARITY[2] =
  {
    1,  // One  block  granularity (granularity = 0)
    4   // Four blocks granularity (granularity = 1)
  };

#else
  extern T_MSCLASS_PARAMETERS MS_CLASS[MAX_CLASS+1];
  extern UWORD8 PR_CONVERSION[2][4];
  extern UWORD8 USF_BLOCK_GRANULARITY[2];
#endif
 
/* Error type */
/*------------*/

#if MACS_STATUS
  #define NEW_SET_NOT_ON_TDMA3     1 // The new_set have been setted to 1 by Layer 1 during a
                                     // TDMA frame that isn't situated just before a new block period

  #define MS_CLASS_TIME_ERROR      2 // Allocated resources doesn't respect the Trb or Ttb
                                     // MS Class parameter
  #define MS_CLASS_SUM_ERROR       3 // Allocated resources doesn't respect the Sum, Rx or
                                     // Tx MS Class parameter
  #define NO_MEAS_MAPPED           4 // No mesurement gap has been found
  #define POLL_ERROR_MS_CLASS      5 // A poll response isn't transmitted because of a MS
                                     // class parameter not respected or measurement gap
                                     // not mapped
  #define POLL_ERROR_DISCONTINUITY 6 // The poll responses have not been transmitted because
                                     // of BCCH monitoring / SYNCHRO. change done by Layer 1
  #define NO_RX_MONITORED          7 // NO timeslot can be monitored according to MS Class and
                                     // power measurements
#endif


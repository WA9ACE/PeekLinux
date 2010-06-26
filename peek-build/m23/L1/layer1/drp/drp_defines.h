/**********************************************************************************
drp_defines.h
-- Copyright (c) 2004, Texas Instruments, Inc.
-- Author: Sumeer Bhatara, Shrinivas Gadkari and Pradeep P

General drp defines
***********************************************************************************
                               Change History
************************************************************************************

Version     Date           Author(s)        Comments
-------------------------------------------------------------------------------------
Ver 0.0     25 Nov 2004    SG, SB and PP    Locosto version based on DRP ver 0.23
Ver 1.01    29 Dec 2004    SG               Based on DRP Ref SW 1.0, with addresses
                                            of srm variables and tables aligned to
                                            locosto scripts version 129.03
                                            Added LOCOSTO_VALID flag.
************************************************************************************/

#ifndef __DRP_DEFINES_H__
#define __DRP_DEFINES_H__

/***********************************************************************************/
/*                                   DRP-SW Manged Definitions go here                                                       */
/***********************************************************************************/

// #define LOCOSTO_VALID         0        // LOCOSTO_VALID = 0 for Uppcosto + Calypso+
#define LOCOSTO_VALID            1        // LOCOSTO_VALID = 1 for Locosto Silicon

#define DRP1_1                   1
#define DRP1_0                   0

#define DRP_SILICON_1P1          0
#define DRP_SILICON_1P2          1

#define LOCOSTO_1_0              3        //DRP2_ID in LocostoPG1.0
#define LOCOSTO_2_0              4        //DRP2_ID in LocostoPG2.0

#define DRP_STANDALONE_BUILD     1        //=1, when developing and tesing calibration in a non-automated mode

#define DRP_TEST_SW              0


#if DRP_TEST_SW
  // One of the foll 4 should be 1 at a time.
  #define DRP_BENCH_SW           0
  #define DRP_TDL_SW             0        // DRP_TDL_SW = 1 for TDL generation, skips MCU init
  #define DRP_TDL_DFT            0        // TDL for BIST routines
  #define DRP_BURNIN             0        // TDL for burnin SW

  #if DRP_BURNIN
    #define DRP_BURNIN_SIMULATE  0        // Burnin test SW simulating on board
    #define DRP_BURNIN_HTOL      0        // To prevent voltage change from 1.4 to 1.6
  #endif                                  // Needed for VR1 and FREF=26MHz

  #if DRP_TDL_DFT
    #define DRP_TDL_DFT_SIMULATE 0        // Needed for VR1
  #endif

  #define DRP_TDL_SW_DEBUG       1        // FFT debug token (can be skipped)

  #define FIXEDPOINT_FFT         0

  #if DRP_TDL_SW
    #define DRP_TDL_GPIO         0        // DRP_TDL_GPIO = 1 for GPIO input
  #endif //#if DRP_TDL_SW

  #if DRP_TDL_GPIO
    #define DRP_TDL_GPIO_SIMULATE  0
  #endif //#if DRP_TDL_GPIO

  // BENCH, RFCHAR, DFT and BURNIN builds will continue to use old style of
  // coding where the DRP2 functions are called directly instead of an API
  #define DRP_FW_BUILD           0

#endif //#if DRP_TEST_SW

#define DRP_FLYBACK_CALIB        1

#define DRP_PHASE2_CALIBRATION   0

#define DRP_FILES_GENERATION_BUILD 0

#define DRP_LOCOSTO_DEBUG        0

// DRP REG, API and SRM_DATA offsets from DRP base address
#define DRP_REGS_BASE_OFF        (0x00000000)
#define DRP_SRM_API_OFF          (0x00002FB0)
#if DRP_BURNIN
#define DRP_SRM_DATA_OFF         (0x00002EB0)
#else
#define DRP_SRM_DATA_OFF         (0x00002C00)
#endif
#define DRP_CALC_BUFFER_OFF      (0x00002300)

#define DRP_PHE_BUFFER_OFF       (0x00002380)

// DRP_RX_BUFFER is 32 samples at rate 2X from CALC_BUFFER start
#define DRP_RX_BUFFER_OFF        (DRP_CALC_BUFFER_OFF + (16*4))
#define DRP_EXT_MEM_START_ADD    (0x00010000)

// This will be used only by the Calibration Test SW on ATE
#define DRP_SRM_SW_DATA_COPY_OFF  (0x00002300)

// This token is to be turned on when the calibration routines which will be tested in phase 2 are
// verified. Currently this token includes
// 1. Temp sensor calibration
// 2. DCXO center frequency calibration
// 3. RX Gain calibration
// 4. DCO Ibias calibration
// 5. Fly back delay calibration
#define PHASE2_CALIBRATION       0
#define DRP_FLYBACK_IBIAS_CALIB  0

  #if DRP_BURNIN
    #define DRP_FLYBACK_IBIAS_CALIB 0
  #endif

#define NUM_BANDS                4
#define NUM_FLYBACK_SUBBANDS     3

#define EGSM_BAND                0
#define GSM_BAND                 1
#define DCS_BAND                 2
#define PCS_BAND                 3

#define TX_MODE                  0
#define RX_MODE                  1

#define LOW_BAND                 0
#define HIGH_BAND                1

#define NUM_ABE_GAINS            14
#define NUM_AFE_GAINS            28

#define NUM_ABE_GAINS_LOCOSTO    9
#define NUM_AFE_GAINS_LOCOSTO    2

#define NUM_HB_FCW_COMP_VALUES      8
#define NUM_LB_FCW_COMP_VALUES      4

#define NUM_TEMPERATURE             32
#define DSPMCU_DIVIDER_TABLE_LENGTH 6

#define NUM_FCU_DAC_COMPARE_VALUES  4

#define NUM_DCXO_IB_COMP_VALUES    4

#define NUM_ARX_VALUES             34

#define SYMBOL_DELAY_160         591        // 160 GSM symbol delay in us
#define SYMBOL_DELAY_8           30         // 8 GSM symbol delay in us

#define PARAM_NB                 8

#define TX_BUFFER_LENGTH         10

// The following is the RX burst capture buffer in the Test SW - No of IQ samples
#define RX_BUFFER_SIZE           (FFT_BUF_SIZE * 2)

// These are the start frequencies of the different Tx and Rx bands
#define EGSM_TX_START            8800
#define EGSM_RX_START            9250
#define GSM_TX_START             8240
#define GSM_RX_START             8690
#define DCS_TX_START             17100
#define DCS_RX_START             18050
#define PCS_TX_START             18500
#define PCS_RX_START             19300

// center channel number of each band.
#define EGSM_CENTER_CH           87
#define GSM_CENTER_CH            62
#define DCS_CENTER_CH            187
#define PCS_CENTER_CH            150

// PCB Configurations

#if DRP_TEST_SW

//LNA Specific BAND Index Settings
#define RF_QUADBAND              0 // Default Setting 850, EGSM,DCS,PCS
#define RF_EU_TRIBAND            1 // EGSM,DCS,PCS
#define RF_EU_DUALBAND           2 // EGSM,DCS
#define RF_US_TRIBAND            3 // 850,DCS,PCS
#define RF_US_DUALBAND           4 // 850, PCS
#define RF_PCS1900_900_DUALBAND  5 // EGSM, PCS
#define RF_DCS1800_850_DUALBAND  6 // 850, DCS

#define RF_BAND_SYSTEM_INDEX RF_QUADBAND //for other PCB's Please redefine here.

#endif

/*-----------------**
** SCRIPTS CONTROL **
**-----------------*/

/*  Scripts Numbers Definition */

//#define DRP_REG_ON         (0x0000)

//#define DRP_TX_ON           (0x0001)
//#define DRP_TX_ON_LB            (0x0001)
//#define DRP_TX_ON_HB            (0x0001)

//#define DRP_RX_ON                  (0x0002)
//#define DRP_RX_ON_EGSM       (0x0002)
//#define DRP_RX_ON_GSM         (0x0002)
//#define DRP_RX_ON_DCS          (0x0002)
//#define DRP_RX_ON_PCS          (0x0002)

//#define DRP_TEMP_CONV   (0x0003)
//#define DRP_REG_OFF        (0x0007)

//#define DRP_AFC                (0x000D)   /* TBD */
//#define DRP_IDLE               (0x000F)

//#define DRP_ROC                (0xFFFF)   /* TBD */

//#define SCRIPT_EN             (0x0080)

/* Macros Definition */

//#define START_SCRIPT(script_nb) (SCRIPT_EN  | script_nb)
//#define STOP_SCRIPT(script_nb)  (!(SCRIPT_EN) & script_nb)


/*-------------------------**
** AGC Settings Definition **
**-------------------------*/

/* Gain Compensation Enable/Disable */

//#define GAIN_COMP_DISABLE    (0x0)  //Default
//#define GAIN_COMP_ENABLE   (0x1)


/* AFE Gains Definition */

//#define AFE_HIGH_GAIN     (0x1)       // 38 dB
//#define AFE_LOW_GAIN      (0x0)       // 11 dB

/* ABE Gains Definition */

//#define ABE_0_DB          (0x0)
//#define ABE_2_DB          (0x1)
//#define ABE_5_DB          (0x2)
//#define ABE_8_DB          (0x3)
//#define ABE_11_DB         (0x4)
//#define ABE_14_DB         (0x5)
//#define ABE_17_DB         (0x6)
//#define ABE_20_DB         (0x7)

/* Macros Definition */

//#define SET_AGC(afe_gain, abe_gain) ((afe_gain<<3) | abe_gain)

/* Switched Cap Filter Corner Freq Defition */

//#define SCF_170KHZ 0
//#define SCF_270KHZ 1

/*------------------**
** RETIMING CONTROL **
**------------------*/

//#define RETIM_DISABLE          (0x0000)
//#define RETIM_TX_ONLY          (0x0001)
//#define RETIM_RX_ONLY          (0x0002)
//#define RETIM_FULL             (0x0003)











#endif //#ifndef __DRP_DEFINES_H__


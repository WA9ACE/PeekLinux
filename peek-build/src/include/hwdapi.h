
#ifndef _HWDAPI0_H_
#define _HWDAPI0_H_

#include "exeapi.h"
#include "hwddefs.h"
#include "sysapi.h"
#include "sysdefs.h"

#include "custbtc.h"

/*----------------------------------------------------------------------------
 Global typedefs
----------------------------------------------------------------------------*/

typedef uint32 HwdRegT;

#endif /* _HWDAPI0_H_ */

/*
 * This section is deliberately outside of the include protection to allow the
 * register trace mode to be selected on file by by file basis.
 */

/*-----------------**
** Register Access **
**-----------------*/
#ifdef SYS_TARGET_SIM
 #ifdef HWD_TRACE_REG
   #include "monapi.h"

   #undef HwdWrite
   #define HwdWrite(Reg, Data) \
             do { \
                *((volatile HwdRegT*) (Reg)) = (Data); \
                MonPrintf("HWD: [0x%08X]  %-25s < 0x%04X\n", (Reg), #Reg, (Data) ); \
                } while (0)
   
 #else

   #undef HwdWrite
   #define HwdWrite(Reg, Data) \
               *((volatile HwdRegT*) (Reg)) = (Data)
   
 #endif
 
#else
   #undef HwdWrite
   #define HwdWrite(Reg, Data) \
               *((volatile HwdRegT*) (Reg)) = (Data)
#endif


#undef HwdRead
#define HwdRead(Reg) \
            (*((volatile uint16*) (Reg)))

#undef HwdSetBit
#define	HwdSetBit( Reg, BitMask )	\
	*( (volatile uint32 *)(Reg) ) = *((volatile uint16 *)(Reg)) | (uint32)(BitMask)

#undef HwdResetBit
#define	HwdResetBit( Reg, BitMask ) \
	*( (volatile uint32 *)(Reg) ) = *((volatile uint16 *)(Reg)) & ~((uint32)(BitMask))

/* 32-Bit Register MACROS - used by USB */
#undef HwdWrite32
#define HwdWrite32(Reg, Data) \
	HwdWrite(Reg,Data)

#undef HwdRead32
#define HwdRead32(Reg) \
           (*((volatile uint32*) (Reg)))

#undef HwdSetBit32
#define	HwdSetBit32( Reg, BitMask )	\
	*( (volatile uint32 *)(Reg) ) = *((volatile uint32 *)(Reg)) | ((uint32)(BitMask))

#undef HwdResetBit32
#define	HwdResetBit32( Reg, BitMask ) \
	*((volatile uint32 *)(Reg)) = *((volatile uint32 *)(Reg)) & ~((uint32)(BitMask))	

/* 8-Bit Register MACROS - used by EBI */
#undef HwdWrite8
#define HwdWrite8(Reg, Data) \
	*((volatile uint8*) (Reg)) = (Data)

#undef HwdRead8
#define HwdRead8(Reg) \
   (*((volatile uint8*) (Reg)))

#undef HwdSetBit8
#define	HwdSetBit8( Reg, BitMask )	\
	*( (volatile uint8 *)(Reg) ) = *((volatile uint8 *)(Reg)) | ((uint8)(BitMask))

#undef HwdResetBit8
#define	HwdResetBit8( Reg, BitMask ) \
	*((volatile uint8 *)(Reg)) = *((volatile uint8 *)(Reg)) & ~((uint8)(BitMask))	

	/* multiple include protection restarts here */   
#ifndef _HWDAPI_H_
#define _HWDAPI_H_

/*----------------------------------------------------------------------------
 Include Files
----------------------------------------------------------------------------*/
#include "sysdefs.h"
#include "hwddefs.h"       /* Hardware defines                              */
#include "hwdint.h"        /* Interrupt handling                            */
#include "hwdrfapi.h"      /* RF Driver API */
#include "sysparm.h"      /* CP & DSP Shared definitions */

/*----------------------------------------------------------------------------
 Global Defines and Macros
----------------------------------------------------------------------------*/
#define HWD_MAILBOX      EXE_MAILBOX_1_ID    /* Command Mailbox Id          */
#define HWD_CAL_MAILBOX  EXE_MAILBOX_2_ID    /* Calibration Mailbox Id      */

/* defines for message interface */
#define HWD_CALIB_INIT_MODE_NVRAM   0     /* use calibration values from NVRAM */
#define HWD_CALIB_INIT_MODE_DEFAULT 1     /* use default coded calibration values  */
#define HWD_GET_CAL_DATA_TIMEOUT    5000

#define HWD_SLEEP_WAKEUP_FLAG         MON_DEEP_SLEEP_HWD_1_FLAG       /* for uart activity (1 sec timer) */
#define HWD_SLEEP_AUDIOPATH_FLAG   MON_DEEP_SLEEP_HWD_2_FLAG     /* for audio path in use */
#define HWD_SLEEP_SERDATA_FLAG       MON_DEEP_SLEEP_HWD_3_FLAG       /* for SERIAL data activity */
#define HWD_SLEEP_UIM_FLAG                MON_DEEP_SLEEP_HWD_4_FLAG            /* for UIM activity */
#define HWD_SLEEP_USBDATA_FLAG     MON_DEEP_SLEEP_HWD_5_FLAG       /* for USB data activity */
#define HWD_SLEEP_VIBRATE_FLAG     MON_DEEP_SLEEP_HWD_6_FLAG        /* for Vibrate */
#define HWD_GPINT_SLEEP_FLAG         MON_DEEP_SLEEP_HWD_7_FLAG          /* For GPINT */
#define MON_DEEP_SLEEP_HWD_I2C_FLAG   MON_DEEP_SLEEP_HWD_8_FLAG/* For I2C Lock */
#define HWD_HEADSETKEY_SLEEP_FLAG   MON_DEEP_SLEEP_HWD_9_FLAG    /* For HeadsetKey Debounce */
#define HWD_SLEEP_MMAPPS_FLAG      MON_DEEP_SLEEP_HWD_10_FLAG  /* for multi-media Apps */

#define HWD_STARTUP_SIGNAL   EXE_SIGNAL_1
#define HWD_MIDI_BUFFER_EMPTY_SIGNAL        EXE_SIGNAL_2
#define HWD_MIDI_EOF_SIGNAL                 EXE_SIGNAL_3

#define HWD_RX_FILT_BW_CAL_DONE_SIGNAL      EXE_SIGNAL_12  /* 0x00010000 */
#define HWD_RX_IQ_GAIN_CAL_DONE_SIGNAL      EXE_SIGNAL_13  /* 0x00020000 */


#define TBD 0xDEAD

#define HWD_TX_RX_FIXED_DLY  1    /* Desired delay between TX and RX under
                                   * normal operation.
                                   */
                                      
/* defines for the calibration parameter messages and calibration parameter usage */
/* note: number of regions in all cases includes one "extra" region to store the rightmost endpoint */
/* These must match the shared definitions for compatibility with the DSPM SW */
#define HWD_NUM_HYST_STATES_TXAGC         SYS_MAX_NUM_HYST_STATES_TXAGC
#define HWD_NUM_HYST_STATES_RXAGC         SYS_MAX_NUM_HYST_STATES_RXAGC

#define HWD_NUM_GAIN_POINTS_TXAGC         SYS_MAX_NUM_GAIN_POINTS_TXAGC
#define HWD_NUM_GAIN_POINTS_RXAGC         SYS_MAX_NUM_GAIN_POINTS_RXAGC

#define HWD_NUM_TEMP_CALIBR_POINTS        8
#define HWD_NUM_BATT_CALIBR_POINTS        8
#define HWD_NUM_BATT_PDM_CALIBR_POINTS    8

#define HWD_NUM_TEMP_POINTS_TXAGC         8
#define HWD_NUM_TEMP_POINTS_RXAGC         8
#define HWD_NUM_TEMP_POINTS_TX_LIM        8

#define HWD_NUM_FREQ_POINTS_TXAGC         16
#define HWD_NUM_FREQ_POINTS_TX_LIM        16
#define HWD_NUM_FREQ_POINTS_RXAGC         16

#define HWD_NUM_BATT_POINTS_TXAGC         8
#define HWD_NUM_BATT_POINTS_TX_LIM        8

#define HWD_NUM_GAIN_POINTS_TX_PWR_DET    8
#define HWD_NUM_TEMP_POINTS_TX_PWR        8
#define HWD_NUM_FREQ_POINTS_TX_PWR        16
#define HWD_NUM_MAX_BATT_POINTS_TX_PWR    8

#define HWD_MAX_NUM_DIGITAL_GAIN_STATES   8
#define HWD_NUM_DEFAULT_GAIN_POINTS       2    /* Used for software calibration defaults */

#ifdef BAND_CLASS_5_SUPPORTED
#define HWD_NUM_SUPPORTED_BAND_CLASS      4    /* For Moss, SW currently supports PCS,Cellular CDMA, Cellular AMPS and 450Mhz bands */
#else
#define HWD_NUM_SUPPORTED_BAND_CLASS      3    /* SW currently supports PCS,Cellular CDMA and Cellular AMPS bands */
#endif 

#define HWD_NUM_SUPPORTED_PDM             5    /* CBP4.0 Supports 5 PDMs */

#define HWD_NUM_DCXO_CALIBR_POINTS        8
#define TBD_HWD_IAQ_OFFSET                0
/*----------------------------------------------------------------------------
     Message IDs
----------------------------------------------------------------------------*/
typedef enum
{
   /*-----------------------------------------------------------------
    * NOTE: Messages, which IDs start with HWD_TST are for testing only;
    *       they shouldn't be used in normal operation !!!
    *----------------------------------------------------------------*/

   HWD_INIT_MSG=0,
   HWD_RESET_MSG,

   
   HWD_TST_TX_ON_OFF_MSG=10,
   HWD_TST_RX_ON_OFF_MSG,
   HWD_TST_PLL_CONFIG_MSG,
   HWD_TST_PLL_CHANNEL_CONFIG_MSG,
   HWD_TST_PLL_CHANNEL_GET_MSG,
   HWD_TST_TX_FLTR_TEST_MSG,
   HWD_TST_TX_ON_CONTROL_MSG,
   HWD_TST_TX_ON_READ_MSG,
   HWD_TST_RF_ON_CONTROL_MSG,
   HWD_TST_RF_ON_READ_MSG,
   HWD_TST_ADC_CDMA_TEST_REQUEST_MSG=20,
   HWD_TST_SET_CLOCK3_MUX_MSG,
   HWD_TST_PROC_PLL_REPROGRAM_MSG,
   HWD_TST_PLL_LOCK_DETECT_TEST_MSG,
   HWD_TST_PWR_SAVINGS_CONFIG_MSG,
   HWD_TST_PWR_SAVINGS_HW_STATUS_GET_MSG, 
   HWD_TST_PWR_SAVINGS_GET_MODE_MSG,
   HWD_TST_READ_CURRENT_TEMP_MSG,
   HWD_TST_READ_CURRENT_BATT_MSG,
   HWD_TST_READ_CURRENT_TX_POWER_MSG,
   HWD_TST_PDM_CONFIG_MSG=30,
   HWD_TST_PDM_GET_VALUE_MSG,
   HWD_TST_LNA_CONFIG_MSG,
   HWD_TST_PA_CONFIG_MSG,
   HWD_TST_PWM_CONFIG_MSG,
   HWD_TST_RF_RX_REG_READ_MSG,
   HWD_TST_RF_RX_REG_WRITE_MSG,   
   HWD_TST_RF_TX_REG_READ_MSG,
   HWD_TST_RF_TX_REG_WRITE_MSG,
   HWD_TST_BURST_FLASH_CTRL,
   HWD_TST_ADC_RF_VERSION_RESPONSE_MSG=40,
   HWD_TST_PHILIPS_RX_GAIN_CTRL_MSG,
   HWD_TST_AUDIO_OUTPUT_SEL_MSG,
   HWD_TST_AUDIO_LOOPBACK_CONFIG_MSG,
   HWD_TST_RF_CHANGE_CONSTANTS,
   HWD_TST_GET_RF_CONSTANTS,
   HWD_TST_SET_RXTX_IQ_SWAP,
   HWD_TST_GET_RXTX_IQ_SWAP,
   HWD_TST_SET_MIC_VOLUME,
   HWD_TST_SET_SPKR_VOLUME,
   HWD_TST_AUDIO_TONE_GEN_MSG=50,
   HWD_TST_AUDIO_TONE_GEN_CFG_MSG,
   HWD_TST_GPIO_CTRL_MSG,
   HWD_TST_GPIO_FUNC_SWITCH_MSG,
   HWD_TST_I2C_READ_MSG,
   HWD_TST_I2C_WRITE_MSG,
   HWD_TST_SHARED_MEM_TEST_START_MSG,
   HWD_TST_SHARED_MEM_TEST_STOP_MSG,
   HWD_TST_SHARED_MEM_DATA_MSG,
   HWD_TST_PHILIPS_DCXO_CTRL_MSG = 59,

   HWD_CAL_INITIALIZE_MSG=100,
   HWD_CAL_AFC_TEMP_ADJ_MSG,   
   HWD_CAL_AFC_PARMS_MSG,
   HWD_CAL_IAFC_VALUE_MSG,
   HWD_CAL_IAFC_VALUE_ACK_MSG,
   HWD_RX_DC_BIAS_CAL_MSG,           
   HWD_TX_DC_BIAS_CAL_MSG,  
   HWD_CAL_INIT_NVRAM_MSG,
   HWD_CAL_TEMPERATURE_PARMS_MSG,
   HWD_CAL_TEMP_OFFSET_PARMS_MSG,
   HWD_CAL_BATTERY_PARMS_MSG = 110,
   HWD_CAL_BATT_PDM_PARMS_MSG,
   HWD_CAL_BANDGAP_PARMS_MSG,
   HWD_CAL_TX_FILT_PARMS_MSG,
   HWD_CAL_PCS_TXAGC_MSG,
   HWD_CAL_PCS_TXAGC_FREQ_ADJ_MSG,
   HWD_CAL_PCS_TXAGC_TEMP_ADJ_MSG,
   HWD_CAL_PCS_TXAGC_BATT_ADJ_MSG,
   HWD_CAL_PCS_TX_LIM_FREQ_ADJ_MSG,
   HWD_CAL_PCS_TX_LIM_TEMP_ADJ_MSG,
   HWD_CAL_PCS_TX_LIM_BATT_ADJ_MSG = 120,
   HWD_CAL_PCS_TX_PWR_DET_TBL_MSG,
   HWD_CAL_PCS_TX_PWR_DET_FREQ_ADJ_MSG,
   HWD_CAL_PCS_TX_PWR_DET_TEMP_ADJ_MSG,
   HWD_CAL_PCS_TX_PWR_DET_MAX_BATT_ADJ_MSG,
   HWD_CAL_PCS_RXAGC_MSG,
   HWD_CAL_PCS_RXAGC_FREQ_ADJ_MSG,
   HWD_CAL_PCS_RXAGC_TEMP_ADJ_MSG,
   HWD_CAL_CELL_TXAGC_MSG,
   HWD_CAL_CELL_TXAGC_FREQ_ADJ_MSG,
   HWD_CAL_CELL_TXAGC_TEMP_ADJ_MSG = 130,
   HWD_CAL_CELL_TXAGC_BATT_ADJ_MSG,
   HWD_CAL_CELL_TX_LIM_FREQ_ADJ_MSG,
   HWD_CAL_CELL_TX_LIM_TEMP_ADJ_MSG,
   HWD_CAL_CELL_TX_LIM_BATT_ADJ_MSG,
   HWD_CAL_CELL_TX_PWR_DET_TBL_MSG,
   HWD_CAL_CELL_TX_PWR_DET_FREQ_ADJ_MSG,
   HWD_CAL_CELL_TX_PWR_DET_TEMP_ADJ_MSG,
   HWD_CAL_CELL_TX_PWR_DET_MAX_BATT_ADJ_MSG,
   HWD_CAL_CELL_RXAGC_MSG,
   HWD_CAL_CELL_RXAGC_FREQ_ADJ_MSG = 140,
   HWD_CAL_CELL_RXAGC_TEMP_ADJ_MSG,
   HWD_CAL_AMPS_TXAGC_MSG,
   HWD_CAL_AMPS_TXAGC_FREQ_ADJ_MSG,
   HWD_CAL_AMPS_TXAGC_TEMP_ADJ_MSG,
   HWD_CAL_AMPS_TXAGC_BATT_ADJ_MSG,
   HWD_CAL_AMPS_TX_PWR_DET_TBL_MSG,
   HWD_CAL_AMPS_TX_PWR_DET_FREQ_ADJ_MSG,
   HWD_CAL_AMPS_TX_PWR_DET_TEMP_ADJ_MSG,
   HWD_CAL_AMPS_RXAGC_MSG,
   HWD_CAL_AMPS_RXAGC_FREQ_ADJ_MSG = 150,
   HWD_CAL_AMPS_RXAGC_TEMP_ADJ_MSG,
   HWD_CAL_AMPS_OVERRIDE_MSG = 152,

   /* Added new 450Mhz band support for Moss.  yanliu */ 	
   HWD_CAL_450M_TXAGC_MSG = 153,
   HWD_CAL_450M_TXAGC_FREQ_ADJ_MSG,
   HWD_CAL_450M_TXAGC_TEMP_ADJ_MSG,
   HWD_CAL_450M_TXAGC_BATT_ADJ_MSG,
   HWD_CAL_450M_TX_LIM_FREQ_ADJ_MSG,
   HWD_CAL_450M_TX_LIM_TEMP_ADJ_MSG,
   HWD_CAL_450M_TX_LIM_BATT_ADJ_MSG,
   HWD_CAL_450M_TX_PWR_DET_TBL_MSG = 160,
   HWD_CAL_450M_TX_PWR_DET_FREQ_ADJ_MSG,
   HWD_CAL_450M_TX_PWR_DET_TEMP_ADJ_MSG,
   HWD_CAL_450M_TX_PWR_DET_MAX_BATT_ADJ_MSG,
   HWD_CAL_450M_RXAGC_MSG,
   HWD_CAL_450M_RXAGC_FREQ_ADJ_MSG,
   HWD_CAL_450M_RXAGC_TEMP_ADJ_MSG = 166,

   HWD_ADC_MEAS_REQUEST_MSG = 200,
   HWD_ADC_MEAS_RESPONSE_MSG,
   HWD_ADC_POLL_START_MSG,
   HWD_ADC_POLL_STOP_MSG = 203,
   
   HWD_SERIALIZER_CONFIG_MSG = 210,
   HWD_SERIALIZER_WRITE_MSG,
   HWD_PLL_CHANNEL_SET_MSG,
   HWD_AMPS_SWITCHOVER_MSG,
   HWD_CP_DSPM_ALIVE_MSG,
   HWD_CP_DSPV_ALIVE_MSG,
   HWD_CP_DSPM_AFC_UPDATE_OFT_MSG,
   HWD_PWR_SAVINGS_MODE_CONFIG_MSG = 217,
  
   HWD_CP_DCB_SCALING_PARMS_MSG = 220,
   HWD_32BIT_REG_READ_MSG,
   HWD_32BIT_REG_WRITE_MSG,
   HWD_EDAI_ENABLE_MSG,
   HWD_EDAI_DISABLE_MSG,
   HWD_INIT_NVRAM_ACK_MSG = 225,

   /*-----------------------------------------------------------------
    * MIDI RINGER Handler
    *----------------------------------------------------------------*/
   HWD_MIDI_STATUS_MSG = 230,                 /* From DSPV to indicate when MIDI ringer off */
   HWD_MIDI_AUDIO_CFG_COMPLETE_MSG,     /* From DSPV after configuring audio */
   HWD_TST_MIDI_PARMS_MSG,              /* Debug message: MIDI test parms */
   HWD_TST_MIDI_CONFIG_MSG,             /* Debug message: Set Midi config */
   HWD_AUDIO_PATH_CFG_RESP_MSG = 234,   

   /*-----------------------------------------------------------------
    * MULTIMEDIA APPLICATION Handler
    *----------------------------------------------------------------*/
   HWD_MMAPPS_CHAN_DATA_MSG = 240,            /* Application data from IPC */
   HWD_MMAPPS_MODE_RSP_MSG,             /* Application Mode resp from DSPV */
   HWD_MMAPPS_AUDIO_CFG_RESP_MSG,       /* MM-Apps audio config response from DSPV */
   HWD_MMAPPS_AUDIO_CLEANUP_RESP_MSG,   /* MM-Apps audio cleanup response from DSPV */
   HWD_MMAPPS_MUSIC_STOP_MSG,           /* Stop music from MMApps code */
   HWD_MMAPPS_APP_STATUS_MSG,           /* Application status from DSPV */
   HWD_MMAPPS_BUFF_RELOAD_MSG,          /* Reload music buffer from MMApps code */

   HWD_TST_MMAPPS_TEST_START_MSG,       /* Loopback MM-Apps test from ETS */
   HWD_TST_MMAPPS_TEST_STOP_MSG,        /* Loopback MM-apps test stop from ETS */

   HWD_TST_MMAPPS_VOCODER_DNLD_MSG,     /* MM-Apps Vocoder Download request from ETS */


   HWD_USB_CABLE_MSG = 250,
   HWD_USB_CABLE_CONNECTED,
   HWD_USB_CABLE_REMOVED, 
   HWD_MSC_READ_MSG,
   HWD_MSC_WRITE_MSG = 254,

   HWD_HEADSET_MSG = 260,
   HWD_HEADSETKEY_MIC_BIAS_MSG,
   HWD_VIBRATOR_MSG = 262,

   HWD_DCBIAS_CONFIG_MSG,

#ifdef DENALI_EXTENDED_CUST_ETS
   HWD_SW_VERSION_MSG = 264,
   HWD_HW_VERSION_MSG = 265,
#endif /*DENALI_EXTENDED_CUST_ETS*/

   HWD_MAX_MSG
} HwdCmdIdT;


/*-----------------**
** Message Formats **
**------------------*/

/* Calibration Messages */
/* AFC calibration parameters -    HWD_CAL_AFC_PARMS_MSG */
typedef PACKED struct
{
   int16    AfcHwvInterc;           /* AFC DAC intercept. 
                                     * This is the default DAC value required 
                                     * to set the VCO to the correct frequency (0 PPB deviation).
                                     */
   int16    AfcSlopeStepPerPpm;    /* slope to convert PPM to HW val, with Q = HWD_TRANS_FUNC_SLOPE_Q */
} HwdAfcCalibParmT;    /* typedef for the message */

/* IAFC Value -    HWD_CAL_IAFC_VALUE_MSG */
typedef PACKED struct
{
   int16    IAfcValue;              /* AFC value calculated from IIR operation */
} HwdIAfcValueT;    /* typedef for the message */

/* DCXO AFC calibration parameters -    HWD_CAL_AFC_TEMP_ADJ_MSG */
typedef PACKED struct
{
    uint16 CtrlVal;	/*digital Control bits*/
    int16 FreqPpm;	/*Crystal Oscillator frequency change PPM*/
} HwdDcxoCalibrPointT;

typedef PACKED struct
{
   uint16  DcxoOfst; 	/* Best Digital Ctrl Value, setting XO frequency to 19.2Mhz*/
   HwdDcxoCalibrPointT  DcxoCalibrTable[HWD_NUM_DCXO_CALIBR_POINTS];
} HwdDcxoAfcCalibParmT;


/* HWD_CAL_TEMPERATURE_PARMS_MSG calibration message */
typedef PACKED struct
{
    int8    TempCelsius;
    uint16  AuxAdcValue;
} HwdTempCalibrPointT;

typedef PACKED struct
{
    HwdTempCalibrPointT TempCalibrTable[HWD_NUM_TEMP_CALIBR_POINTS];
} HwdTempCalibrationMsgT;

typedef PACKED struct
{
    int16  TempOffset;
} HwdTempOffsetCalMsgT;

/* HWD_CAL_TEMP_OFFSET_PARMS_MSG calibration message */
typedef PACKED struct
{
   int16    AuxAdcOffset;    /* offset to basic temperature curve */
} HwdTempOffsetCalibParmT;    /* typedef for the message */


/* HWD_CAL_BATTERY_PARMS_MSG calibration message */
typedef PACKED struct
{
    int16   BattVoltage; /* mV */
    uint16  AuxAdcValue;
} HwdBattCalibrPointT;

typedef PACKED struct
{
    HwdBattCalibrPointT BattCalibrTable[HWD_NUM_BATT_CALIBR_POINTS];
} HwdBattCalibrationMsgT;

/* HWD_CAL_BATT_PDM_PARMS_MSG calibration message */
typedef PACKED struct
{
    uint16  PdmValue;
    int16   BattVoltage; /* mV */
} HwdBattPdmCoordinateT;

typedef PACKED struct
{
   HwdBattPdmCoordinateT PdmBattCharger[HWD_NUM_BATT_PDM_CALIBR_POINTS];
} HwdBatteryPdmCalibDataT;           /* typedef for the message */

/* HWD_CAL_BANDGAP_PARMS_MSG */
typedef PACKED struct
{
   uint8 CourseTrimBits;      /* Bandgap trim setting for cf11[3:1] */
   uint8 FineTrimBits;        /* Bandgap trim setting for cf12[6:4] */
} HwdBandgapCalibDataT;       /* typedef for the message */    

/* HWD_CAL_TX_FILT_PARMS_MSG */
typedef PACKED struct
{
   uint8 TuneValue;
} HwdTxFilterCalibDataT;       /* typedef for the message */    

/* TX/RX AGC calibration messages */

/* Tx power calibration (baseline, without temp and freq channel adjustment) */
typedef PACKED struct
{
   int16 TxPwrDbm;        /* Tx power coordinate. Units: dBm with Q=IPC_DB_Q */
   int16 TxAgcDacSetting; /* Tx AGC DAC setting coordinate. Units: DAC steps */
} HwdTxPwrCoordinateT;    /* format matches HwdCoordinateT */

typedef PACKED struct
{
   uint16                TxAgcPAGainStates;       /* 2 or 3 PA Hysteresis Gain states */
   int16                 TxAgcHystHighThresh1Dbm; /* Threshold at which to switch from first hyst state
                                                   * to second state - Tx power in dBm, Q = IPC_DB_Q */   
   int16                 TxAgcHystLowThresh1Dbm;  /* Threshold at which to switch from second hyst state
                                                   * to first state - Tx Power in dBm, Q = IPC_DB_Q */ 
   int16                 TxAgcHystHighThresh2Dbm; /* Threshold at which to switch from second hyst state
                                                   * to third state - Tx Power in dBm, Q = IPC_DB_Q */
   int16                 TxAgcHystLowThresh2Dbm;  /* Threshold at which to switch from thried hyst state
                                                   * to second state - Tx Power in dBm, Q = IPC_DB_Q */
   HwdTxPwrCoordinateT   TxPwrCoordinate[HWD_NUM_HYST_STATES_TXAGC][HWD_NUM_GAIN_POINTS_TXAGC];
} HwdTxPwrCalibDataT;    /* typedef for the message */

/* Tx AGC temperature adjustment */
typedef PACKED struct
{
   int8 TempCelsius;     /* Temperature coordinate. Units: celsius */
   int16 AdjDb;           /* Tx gain adjustment coordinate. Units: dB with Q=IPC_DB_Q */
} HwdGainTempAdjCoordinateT;    /* format matches HwdCoordinateT */

typedef PACKED struct
{
   HwdGainTempAdjCoordinateT   TxTempAdjCoordinate[HWD_NUM_HYST_STATES_TXAGC][HWD_NUM_TEMP_POINTS_TXAGC];
} HwdTxTempAdjDataT;    /* typedef for the message */

/* Tx AGC frequency channel adjustment */
typedef PACKED struct
{
   int16 FreqChanNum;     /* frequency channel number coordinate. Units: channel number */
   int16 AdjDb;           /* Tx gain adjustment coordinate. Units: dB with Q=IPC_DB_Q */
} HwdGainFreqAdjCoordinateT;     /* format matches HwdCoordinateT */

typedef PACKED struct
{
   HwdGainFreqAdjCoordinateT   TxFreqAdjCoordinate[HWD_NUM_HYST_STATES_TXAGC][HWD_NUM_FREQ_POINTS_TXAGC];
} HwdTxFreqAdjDataT;    /* typedef for the message */

/* Tx battery voltage adjustment */
typedef PACKED struct
{
   uint16 BattVoltage;     /* battery voltage  - mV */
   int16  AdjDb;           /* Tx pwr adjustment coordinate. Units: dB with Q=IPC_DB_Q */
}HwdBatteryAdjCoordinateT;

/* Tx Agc battery voltage adjustment */
typedef PACKED struct
{
   HwdBatteryAdjCoordinateT   TxBatteryAdjCoor[HWD_NUM_BATT_POINTS_TXAGC];
} HwdTxBatteryAdjT;    /* typedef for the message */

/* Tx Power Limit Freq Adj */
typedef PACKED struct
{
   HwdGainFreqAdjCoordinateT   TxFreqAdjCoordinate[HWD_NUM_FREQ_POINTS_TX_LIM];
} HwdTxLimitFreqAdjDataT;    /* typedef for the message */

/* Tx Limit temperature adjustment */
typedef PACKED struct
{
   HwdGainTempAdjCoordinateT   TxTempAdjCoordinate[HWD_NUM_TEMP_POINTS_TX_LIM];
} HwdTxLimitTempAdjDataT;    /* typedef for the message */

/* Tx Limit battery voltage adjustment */
typedef PACKED struct
{
   HwdBatteryAdjCoordinateT   TxBatteryAdjCoor[HWD_NUM_BATT_POINTS_TX_LIM];
} HwdTxLimitBatteryAdjT;    /* typedef for the message */

/* Rx power calibration (baseline, without temp and freq channel adjustment) */
typedef PACKED struct
{
   int16 RxAgcDacSetting; /* Rx AGC DAC setting coordinate. Units: DAC steps */
   int16 RxGainDb;        /* Rx gain coordinate. Units: dB with Q=IPC_DB_Q */
} HwdRxPwrCoordinateT;     /* format matches HwdCoordinateT */

typedef PACKED struct
{
   uint16                RxAgcLNAGainStates;      /* 2 or 3 LNA Hysteresis Gain states */
   int16                 RxAgcHystHighThresh1Db; /* Threshold at which to switch from first hyst state
                                                  * to second state - Tx power in dB, Q = IPC_DB_Q */   
   int16                 RxAgcHystLowThresh1Db;  /* Threshold at which to switch from second hyst state
                                                  * to first state - Tx Power in dB, Q = IPC_DB_Q */ 
   int16                 RxAgcHystHighThresh2Db; /* Threshold at which to switch from second hyst state
                                                  * to third state - Tx Power in dB, Q = IPC_DB_Q */
   int16                 RxAgcHystLowThresh2Db;  /* Threshold at which to switch from thried hyst state
                                                  * to second state - Tx Power in dB, Q = IPC_DB_Q */
   int16                 RxAgcHystDelayThreshDb; /* Theshold at which to switch from highest hyst state
                                                  * to the next lower state if Rx gain stays below it  
                                                  * long enough (HystDelayCount PCGs) */
   uint16                RxAgcHystDelayCount;    /* Number of PCGs to satisfy the delay threshold */
   HwdRxPwrCoordinateT   RxGainCoordinate[HWD_NUM_HYST_STATES_RXAGC][HWD_NUM_GAIN_POINTS_RXAGC];
} HwdRxPwrCalibDataT;    /* typedef for the message */

/* Digital Rx AGC power calibration (baseline, without temp and freq channel adjustment) */
typedef PACKED struct
{
   int16 LowSwitchThresh;      /* Low Switch Threshold, Units: dB with Q=IPC_DB_Q */
   int16 HighSwitchThresh;     /* High Switch Threshold, Units: dB with Q=IPC_DB_Q */
} HwdRxDagcSwitchThreshT;

typedef PACKED struct
{
   int16  GainStepDelta;       /* Gain Step, Units: dB with Q=IPC_DB_Q */
   uint16 LowDelayCount;       /* Delay Counts when switching to a lower gain state */
   uint16 HighDelayCount;      /* Delay Counts when switching to a higher gain state */
} HwdRxDagcGainParamsT;

typedef PACKED struct
{
   int8  DigiBitSel;
   int8  DigiGain;
} HwdRxDagcRefBitsT;

typedef PACKED struct
{
   int16  HystThresh;      /* Threshold at which to switch from the highest hyst state
                           ** to the next lower state if Rx gain stays below its value
                           ** long enough (HystDelayCount PCGs) */                           
   uint16 HystDelayCount;  /* Number of PCGs to satisfy the delay threshold */

} HwdRxDagcHystParamsT;

typedef PACKED struct
{
   uint16                 RxAgcDigitalGainStates; /* 1 to 8 possible Digital Gain states */
   int16                  RxRefLeveldB;
   uint8                  RxRefGainState;
   HwdRxDagcRefBitsT      RxRefBitSettings;
   HwdRxDagcSwitchThreshT RxSwitchThresh[HWD_MAX_NUM_DIGITAL_GAIN_STATES];
   HwdRxDagcGainParamsT   RxGainParams[HWD_MAX_NUM_DIGITAL_GAIN_STATES-1];
   HwdRxDagcHystParamsT   RxHystParams;

} HwdRxDagcCalibDataT;    /* typedef for the message */

/* Rx AGC temperature adjustment */
typedef PACKED struct
{
   HwdGainTempAdjCoordinateT   RxTempAdjCoordinate[HWD_NUM_TEMP_POINTS_RXAGC];
} HwdRxTempAdjDataT;    /* typedef for the message */

/* Rx AGC frequency channel adjustment */
typedef PACKED struct
{
   HwdGainFreqAdjCoordinateT   RxFreqAdjCoordinate[HWD_NUM_FREQ_POINTS_RXAGC];
} HwdRxFreqAdjDataT;    /* typedef for the message */

/* Tx power detector expected measurement */
typedef PACKED struct
{
   uint16 TxPwrDetAdcMeas; /* Tx ADC reading */
   int16 TxPwrDbm;        /* Tx power coordinate, units: dBm Q= IPC_DB_Q */
} HwdTxPwrDetCoordT;      /* format matches HwdCoordinateT */

/* Tx power detect expected measurement */
typedef PACKED struct
{
   HwdTxPwrDetCoordT   HwdTxPwrDetCoordinate[HWD_NUM_GAIN_POINTS_TX_PWR_DET];
} HwdTxPwrDetTableT;    /* typedef for the message */

/* Tx Pwr detect temperature adjustment */
typedef PACKED struct
{
   HwdGainTempAdjCoordinateT   TxPwrDetTempAdjCoor[HWD_NUM_TEMP_POINTS_TX_PWR];
} HwdTxPwrDetTempAdjT;    /* typedef for the message */

/* Tx Pwr detect frequency adjustment */
typedef PACKED struct
{
   HwdGainFreqAdjCoordinateT   TxPwrDetFreqAdjCoor[HWD_NUM_FREQ_POINTS_TX_PWR];
} HwdTxPwrDetFreqAdjT;    /* typedef for the message */

/* Tx Pwr detect Max power limit battery voltage adjustment */
typedef PACKED struct
{
   HwdBatteryAdjCoordinateT   TxPwrDetBattMaxPwrAdjCoor[HWD_NUM_MAX_BATT_POINTS_TX_PWR];
} HwdTxPwrDetBattMaxAdjT;    /* typedef for the message */


typedef PACKED struct 
{
   int16           ModeNum;
   uint16          Decim;
   uint16          DecimMsgExp;
   uint16          CpAvgNumExp;
   int16           Atten;
} HwdRxFiltBwCalMsgT;  

typedef PACKED struct 
{
   int16           ModeNum;
   uint16          Decim;
   uint16          DecimMsgExp;
   uint16          CpAvgNumExp;
   int16           CorrectionCoef;
} HwdRxIqGainCalMsgT;


/* RF board version information */
typedef PACKED struct
{
   uint8  MemAllocApiVerNum;   /* version number of NVRAM memory allocation */
   uint8  RfBoardVersionNum;   /* version number of the RF board design     */
   uint8  RfBoardSerNum;       /* serial number of the RF board             */
   uint8  MixSigVerNum;        /* mixed signal version of the calibration parms */
} HwdRfVersionInfoT;

/* calibration init message type */
typedef PACKED struct
{
   ExeRspMsgT  RspInfo;             /* cmd/rsp info for acknowlegment */
   uint8       CalibMode;
} HwdCalibInitMsgT;

typedef enum
   {
   HWD_ENABLE = 0,
   HWD_DISABLE,
   HWD_LOAD
   } HwdCtrlModeT;

/* define power saving hardware blocks */
typedef enum
{
   HWD_PWR_SAVING_ALL = 0,
   HWD_PWR_SAVINGS_CLK32,
   HWD_PWR_SAVINGS_CLK1,
   HWD_PWR_SAVINGS_CLK9TXI,
   HWD_PWR_SAVINGS_CLK9GFN,
   HWD_PWR_SAVINGS_CLK9TX,
   HWD_PWR_SAVINGS_CLK9RX,
   HWD_PWR_SAVINGS_CLK19,
   HWD_PWR_SAVINGS_CLK9UIM,
   HWD_PWR_SAVINGS_CLK39,
   HWD_PWR_SAVINGS_CLK9GPCTL,
   HWD_PWR_SAVINGS_CLKSRCHR,
   HWD_PWR_SAVINGS_CLKVDDM,
   HWD_PWR_SAVINGS_CLK360,
   HWD_PWR_SAVINGS_CLK80,
   HWD_PWR_SAVINGS_CLK8K,
   HWD_PWR_SAVINGS_CLK9GSPI,
   HWD_PWR_SAVINGS_CLK39RX,
   HWD_PWR_SAVINGS_CLKRHA,
   HWD_PWR_SAVINGS_CLKTHA,
   HWD_PWR_SAVINGS_CLKDM,
   HWD_PWR_SAVINGS_CLKDV,
   HWD_PWR_SAVINGS_CLK19A,
   HWD_PWR_SAVINGS_CLK2,
   HWD_PWR_SAVINGS_BGREFVOL,
   HWD_PWR_SAVINGS_AUXADC,
   HWD_PWR_SAVINGS_MSBIAS,
   HWD_PWR_SAVINGS_RXADC,
   HWD_PWR_SAVINGS_TXDACI,
   HWD_PWR_SAVINGS_TXDACQ,
   HWD_PWR_SAVINGS_PDMDE2,
   HWD_PWR_SAVINGS_PDMDE3,
   HWD_PWR_SAVINGS_PDMDE4,
   HWD_PWR_SAVINGS_PDMSE0,
   HWD_PWR_SAVINGS_PDMSE1,
   HWD_PWR_SAVINGS_PDMSE2,
   HWD_PWR_SAVINGS_PDMSE3,
   HWD_PWR_SAVINGS_AMPSCOMPS,
   HWD_PWR_SAVINGS_SERCTRL,
   HWD_PWR_SAVINGS_EXT_TX,
   HWD_PWR_SAVINGS_EXT_RX,
   HWD_PWR_SAVINGS_JITTER1,
   HWD_PWR_SAVINGS_JITTER2,
   HWD_PWR_SAVING_VCODECDIGPATH,
   HWD_PWR_SAVINGS_VDAC,
   HWD_PWR_SAVINGS_VADC,
   HWD_PWR_SAVINGS_MIC,
   HWD_PWR_SAVINGS_VDACAMP1,
   HWD_PWR_SAVINGS_VDACAMP2,
   HWD_PWR_SAVINGS_MXS_TX_CLK,
   HWD_PWR_SAVINGS_MXS_RX_SD_ACK,
   HWD_PWR_SAVINGS_MXS_RX_SD_CDMA_DCK,
   HWD_PWR_SAVINGS_MXS_AMPS_CTRL,
   HWD_PWR_SAVINGS_MXS_PDM_CLK
   
} HwdPwrSaveHwBlkTypeT;

/* power saving modem modes */
typedef	enum
{
   HWD_PWRSAVING_PWRUP = 0,
   HWD_PWRSAVING_PWRUP_QP,
   HWD_PWRSAVING_CDMA_RX,
   HWD_PWRSAVING_CDMA_RX_TX,
   HWD_PWRSAVING_DEEPSLEEP,
   HWD_PWRSAVING_AMPS_RX,
   HWD_PWRSAVING_AMPS_RX_TX,
   HWD_PWRSAVING_AMPS_STNDBY,
   HWD_PWRSAVING_PWRUP_NO_DSP,
   HWD_PWRSAVING_CDMA_RX_HHO,
   HWD_PWRSAVING_NUM_OF_STATES
} HwdPwrSaveModesT;

/* clk source options */
typedef enum
{	
   HWD_CLK_OPTION_32K_INT_OSC = 0,
   HWD_CLK_OPTION_32K_BYPASS,
   HWD_CLK_OPTION_32K_DIV_TCXO
}HwdClkOption32KT; 

/* define power saving config msg command */
typedef	PACKED struct
{
   HwdCtrlModeT    CtrlMode;          /* Auto/Disable/Manual */
   HwdPwrSaveHwBlkTypeT	HwType;	      /* hardware block type */
   bool		HwState;				  /* hardware state
   									   * TRUE = on
   									   * FALSE = off */	
} HwdPwrSaveConfigMsgT;

/* define power saving mode set msg command */
typedef PACKED struct
{
    HwdCtrlModeT        CtrlMode;          /* Auto/Disable/Manual */
    HwdPwrSaveModesT    PwrSaveMode;
    uint16              VcPwrSaveMode;
} HwdPwrSaveModeConfigMsgT;

/* define power saving mode read msg command */
typedef	PACKED	struct
{
   ExeRspMsgT     RspInfo;    
} HwdPwrSaveGetModeMsgT;

/* define power saving mode send msg command */
typedef PACKED struct
{
    HwdPwrSaveModesT    PwrSaveMode;
} HwdPwrSaveModeRspMsgT;


/* define power saving hardware status read msg command */
typedef	PACKED	struct
{
   ExeRspMsgT     RspInfo;    
} HwdPwrSaveHwStatusGetMsgT;

/* define power saving hardware status response */
typedef	PACKED	struct
{
   bool HwdClk32Enable;
   bool HwdClk1Enable;
   bool HwdClk9TxiEnable;
   bool HwdClk9GfnEnable;
   bool HwdClk9TxEnable;
   bool HwdClk9RxEnable;
   bool HwdClk19Enable;
   bool HwdClk9UimEnable;
   bool HwdClk39Enable;
   bool HwdClk9GpctlEnable;
   bool HwdClkSrchrEnable;
   bool HwdClkVdDmEnable;
   bool HwdClk360Enable;
   bool HwdClk80Enable;
   bool HwdClk8KEnable;
   bool HwdClk9GspiEnable;
   bool HwdClk39RxEnable;
   bool HwdClkRhaEnable;
   bool HwdClkThaEnable;
   bool HwdClkDspMEnable;
   bool HwdClkDspVEnable;
   bool HwdClk19AEnable;
   bool HwdClk2Enable;
   bool HwdBandGapCtrl;
   bool HwdAuxAdcCtrl;
   bool HwdMSBiasCtrl;
   bool HwdRxAdcCtrl;
   bool HwdTxDacICtrl;
   bool HwdTxDacQCtrl;
   bool HwdPdmDe2Ctrl;
   bool HwdPdmDe3Ctrl;
   bool HwdPdmDe4Ctrl;
   bool HwdPdmSe0Ctrl;
   bool HwdPdmSe1Ctrl;
   bool HwdPdmSe2Ctrl;
   bool HwdPdmSe3Ctrl;
   bool HwdAmpsCompsCtrl;
   bool HwdSerCtrl;
   bool HwdJitter1Ctrl;
   bool HwdJitter2Ctrl;
   bool HwdMxsTxClkCtrl;
   bool HwdMxsRxSdAckCtrl;
   bool HwdMxsRxSdCdmaDckCtrl;
   bool HwdMxsAmpsCtrl;
   bool HwdMxsPdmCtrl;
   bool HwdVcodecDigCtrl;
   bool HwdVoiceDacCtrl;
   bool HwdVoiceAdcCtrl;
   bool HwdVoiceMicCtrl;
   bool HwdVoiceDacAmp1Ctrl;
   bool HwdVoiceDacAmp2Ctrl;
} HwdPwrSaveHwStatusGetRspT;


/* Mix signal read  command */
typedef	PACKED	struct
{
   ExeRspMsgT     RspInfo;
   uint16         Address;    /* Mix signal register address */    
} HwdTstMsReadMsgT;

/* Mix signal read response */
typedef	PACKED	struct
{
   uint16         Data;       /* Data read from mix signal register */
} HwdTstMsReadRspT;

/* Mix signal write command */
typedef	PACKED	struct
{
  uint16         Address;     /* Mix signal register address */
  uint16         Data;        /* Data to write to mix signal register */
} HwdTstMsWriteMsgT;

typedef PACKED struct 
{
   HwdRfPllBandT PllBand;
   uint32        Channel;
   ExeTaskIdT    RspTaskId;
   ExeSignalT    RspSignal;
}HwdPllChannelSetMsgT;   
 
/* clock 3 mux programming msg */ 
typedef PACKED struct 
{
   uint16        Mux1Select;
   uint16        Mux2Select;
}HwdClock3MuxSetMsgT;   

/* processor pll re-programming msg */
typedef PACKED struct 
{
   uint16        M_Value;
   uint16        N_Value;
   uint16        Offset_Value;
   uint16        Const_Value;
}HwdPllReConfigMsgT;   

/* PDM config msg */
typedef PACKED struct 
{
   HwdCtrlModeT  CtrlMode;          /* Auto/Disable/Manual */
   uint16        PdmNumber;
   uint16        PdmWriteValue;
}HwdTstPdmConfigMsgT;   

/* get last written PDM value msg */
typedef PACKED struct 
{
   ExeRspMsgT    RspInfo;
   uint16        PdmNumber;
}HwdTstGetLastPdmValueMsgT;   

/* get last written PDM value msg */
typedef PACKED struct 
{
   uint16        PdmValue;
}HwdTstGetLastPdmValueRspT;   

/* LNA configuration */
typedef PACKED struct 
{
   uint8        GainState;
}HwdTstLnaConfigMsgT;   

/* PA configuration */
typedef PACKED struct 
{
   uint8        GainState;
}HwdTstPaConfigMsgT;   

/* Calibration init NVRAM message type */
typedef PACKED struct
{
   ExeRspMsgT  RspInfo;             /* cmd/rsp info for acknowlegment */
} HwdCalInitNvramMsgT;

/*----------------------------------------------------------------------------
 Global Typedefs
----------------------------------------------------------------------------*/
/* generic XY coordinated type */
typedef struct
{
   int16 X;     /* horizontal (input) coordinate */
   int16 Y;     /* vertical (output) coordionate */
} HwdCoordinateT;

/* generic line segment type */
typedef PACKED struct
{
   int16 BoundaryLeft;    /* left boundary of segment */
   int16 Slope;           /* slope of segment */
   int16 Intercept;       /* y-intercept of segment */
} HwdLineSegmentT;


/* Mixed Signal device selection */
typedef enum
{
    HWD_MS_AFC,
    HWD_MS_AGC
} HwdMsTypeT;

/* RX AGC Type */
typedef enum
{
   HWD_AGC_RX_IMMED,
   HWD_AGC_RX_TIMED
} HwdRxAgcTypeT;

/* Possible frame lengths for frame synchronized Rx AGC gain changes */
typedef enum
{
   HWD_RXAGC_FRAME_LEN_20,          /* 20ms frame length       */
   HWD_RXAGC_FRAME_LEN_26           /* 26.66667ms frame length */
} HwdRxAgcFrameLenT;

/* Hardware calibration values to be read from the EEPROMs */
typedef struct
{
    int16    AfcHwvInterc;			/* AFC DAC intercept. This is the default DAC value required 
                                     * to set the VCO to the correct frequency (0 PPM deviation).
                                     */
    int16    AfcSlopeStepPerPpm;	/* slope to convert PPM to HW val, with Q = HWD_TRANS_FUNC_SLOPE_Q */

    int16    TempOffset;            /* Temperature Offset */

    uint16   RxAgcLNAGainStates[HWD_NUM_SUPPORTED_BAND_CLASS];

    int16    RxAgcHystHighThresh1Db[HWD_NUM_SUPPORTED_BAND_CLASS];  
                                      /* threshold at which to switch the Rx AGC first hyst state to medium gain,
                                       * Rx gain in dB with Q = IPC_DB_Q
                                       */
    int16    RxAgcHystLowThresh1Db[HWD_NUM_SUPPORTED_BAND_CLASS];   
                                      /* threshold at which to switch the Rx AGC first hyst state to low gain,
                                       * Rx gain in dB with Q = IPC_DB_Q
                                       */
    int16    RxAgcHystHighThresh2Db[HWD_NUM_SUPPORTED_BAND_CLASS];  
                                      /* threshold at which to switch the Rx AGC second hyst state to high gain,
                                       * Rx gain in dB with Q = IPC_DB_Q
                                       */
    int16    RxAgcHystLowThresh2Db[HWD_NUM_SUPPORTED_BAND_CLASS];   
                                      /* threshold at which to switch the Rx AGC second hyst state to medium gain,
                                       * Rx gain in dB with Q = IPC_DB_Q
                                       */
    int16    RxAgcHystDelayThreshDb[HWD_NUM_SUPPORTED_BAND_CLASS];  
                                      /* Theshold at which to switch from highest hyst state
                                       * to the next lower state if Rx gain stays below it  
                                       * long enough (HystDelayCount frames) */
    uint16   RxAgcHystDelayCount[HWD_NUM_SUPPORTED_BAND_CLASS];     
                                      /* Number of frames to satisfy the delay threshold */
    uint16   TxAgcPAGainStates[HWD_NUM_SUPPORTED_BAND_CLASS];

    int16    TxAgcHystHighThresh1Dbm[HWD_NUM_SUPPORTED_BAND_CLASS]; 
                                      /* threshold at which to switch the Tx AGC first hyst state to high gain,
                                       * Tx power in dBm with Q = IPC_DB_Q
                                       */
    int16    TxAgcHystLowThresh1Dbm[HWD_NUM_SUPPORTED_BAND_CLASS];  
                                      /* threshold at which to switch the Tx AGC first hyst state to low gain,
                                       * Tx power in dBm with Q = IPC_DB_Q
                                       */
    int16    TxAgcHystHighThresh2Dbm[HWD_NUM_SUPPORTED_BAND_CLASS]; 
                                      /* threshold at which to switch the Tx AGC second hyst state to high gain,
                                       * Tx power in dBm with Q = IPC_DB_Q
                                       */
    int16    TxAgcHystLowThresh2Dbm[HWD_NUM_SUPPORTED_BAND_CLASS];  
                                      /* threshold at which to switch the Tx AGC second hyst state to low gain,
                                       * Tx power in dBm with Q = IPC_DB_Q    */
    
    uint16   RxDcBiasIOffset;         /* Rx I ADC dc bias power up offset value */
    uint16   RxDcBiasQOffset;         /* Rx Q ADC dc bias power up offset value */
    
    int16    TxDcBiasIOffset;         /* Tx I ADC dc bias power up offset value */
    int16    TxDcBiasQOffset;         /* Tx Q ADC dc bias power up offset value */
    int16    IAfcValue;               /* AFC value calculated from IIR operation */
} HwdCalibrParmT;

typedef PACKED struct
{
   uint8  BandClass;
   int8   TempCelsius;
} HwdTempMeasSimT;

/* Ms calib types */
typedef PACKED struct 
{
   uint16          Decim;
   uint16          DecimMsg;
   int16           BiasShiftUp;
   uint16          ZeroAdj;
} HwdRxDcBiasParmSetMsgT;

/* Ms calib types */
typedef PACKED struct 
{
  int16     ModeNum;
  int16     DcBiasI;
  int16     DcBiasQ;
} IpcDcDcBiasMsgT;

/* HwdRxDcBiasBitSelMsgT */
typedef PACKED struct 
{
   int16 BitSel;
   int16 Gba1I;
   int16 Gba2I;
   int16 Gba1Q;
   int16 Gba2Q;
} HwdRxDcBiasBitSelMsgT; 

/* HWD_CAL_AMPS_OVERRIDE_MSG */
typedef PACKED struct
{
    uint8 UseNvramCal;
    int16 RxAgcSlope;

} HwdAmpsRxCalOverrideT;

/* HWD_PWM_CONFIG_MSG */
typedef enum
{
   HWD_PWM_KEYPAD_BACKLIGHT = 0,
   HWD_PWM_LCD_CONTRAST,
   HWD_PWM_LCD_DISPLAY_BACKLIGHT
} HwdPwmSelectT;

typedef enum
{
   HWD_PWM_OUTPUT_DISABLE = 0,
   HWD_PWM_OUTPUT_ENABLE
} HwdPwmOutputT;

typedef PACKED struct
{
   uint8  PwmSelect;
   uint8  PwmOutputEnable;
   uint8  DutyCycle;
} HwdPwrSavePwmSetMsgT;

/* HWD_32BIT_REG_READ_MSG */
typedef PACKED struct
{
   ExeRspMsgT  RspInfo;             /* cmd/rsp info for acknowlegment */
   uint32      RegAddr;

} Hwd32BitRegReadMsgT;

typedef PACKED struct
{
   uint32      RegAddr;
   uint32      RegData;

} Hwd32BitRegReadRspT;

/* HWD_32BIT_REG_WRITE_MSG */
typedef PACKED struct
{
   ExeRspMsgT  RspInfo;             /* cmd/rsp info for acknowlegment */
   uint32      RegAddr;
   uint32      RegData;

} Hwd32BitRegWriteMsgT;

typedef PACKED struct
{
   uint32      RegAddr;

} Hwd32BitRegWriteRspT;

/* HWD_EDAI_ENABLE_MSG */
typedef PACKED struct {
   uint16      CompandingFormat;
   uint16      FSyncMode0;
   int16       SpkrVolAdjust;
} HwdAudioEdaiEnableMsgT;

typedef struct
{
   ExeRspMsgT  RespInfo;
   bool        AckRequired;
}HwdResetReqMsgT;

/* HWD_AUDIO_LOOPBACK_CONFIG_MSG */
typedef PACKED struct
{
   uint8  LoopbackMode;
   uint8  SpeechOption;
   uint8  SpeechRate;
} HwdAudioLoopbackConfigMsgT;

/* HWD_BURST_FLASH_CTRL */
typedef PACKED struct
{
   uint8        BurstEn;
}HwdBurstFlashMsgT;

/* HWD_SET_RXTX_IQ_SWAP */
typedef PACKED struct
{
   uint8        Band;
   uint8        RxSwap;
   uint8        TxSwap;
}HwdSetRxTxIQSwapMsgT;

/* HWD_GET_RXTX_IQ_SWAP */
typedef PACKED struct
{
   ExeRspMsgT   RspInfo;
   uint8        Band;
}HwdGetRxTxIQSwapMsgT;

typedef PACKED struct
{
   uint8        RxSwap;
   uint8        TxSwap;
}HwdGetRxTxIQSwapRspT;

/*  HWD_SET_MIC_VOLUME */
typedef PACKED struct
{
   ExeRspMsgT   RspInfo;
   HwdCtrlModeT CtrlMode;          /* Auto/Disable/Manual */
   uint8        TableIdx;
}HwdAudioMicVolumeMsgT;

typedef PACKED struct
{
   uint8        Index;
   uint8        AnalogGain;
   uint16       DspSetting;
   uint8        LnaEnabled;
   int16        TotalGain; 
}HwdAudioMicVolumeRspT;

/*  HWD_SET_SPKR_VOLUME */
typedef PACKED struct
{
   ExeRspMsgT   RspInfo;
   HwdCtrlModeT CtrlMode;          /* Auto/Disable/Manual */
   uint8        TableIdx;
}HwdAudioSpkrVolumeMsgT;

typedef PACKED struct
{
   uint8        Index;
   uint8        AnalogGain;
   uint16       DspSetting;
   uint16       LoudSpkr;
   int16        TotalGain; 
}HwdAudioSpkrVolumeRspT;

 /* HWD_AUDIO_TONE_GEN_MSG */
 typedef enum
 {
    TONE_OUT_HANDSET,
    TONE_OUT_MONO_HEADSET,
    TONE_OUT_HANDSFREE_EDAI,
    TONE_OUT_HANDSFREE_DAC,
    TONE_OUT_CDS,
    TONE_OUT_LOUDSPKR_PDM_DIFF,
    TONE_OUT_LOUDSPKR_I2S,
    TONE_OUT_STEREO_HEADSET,
    TONE_OUT_RNGR_PDM_DIFF,
    TONE_OUT_RNGR_I2S
 } BalAudioToneOutputT;
 
 typedef PACKED struct
 {
    uint16              ToneFreq;
    uint16              ToneVolume;
    BalAudioToneOutputT ToneOutput;
 } HwdAudioToneGenMsgT;
 
/* HWD_AMPS_SWITCHOVER_MSG */
typedef struct
{
   bool AmpsMode;        /* TRUE if switching to AMPS */
} HwdAmpsSwitchoverMsgT;

/*====== HWD I2C READ/WRITE Message Definitions ======*/
/* Define HWD I2C Write msg */
#define HWD_I2C_BUF_SIZE_MAX  64

typedef PACKED struct
{
   uint16      DevAddr;
   uint16      GenDevAddr;
   uint32      SubAddr;
   uint16      NumBytes;
   uint8       Data[HWD_I2C_BUF_SIZE_MAX];
} HwdI2cWriteMsgT;

/* Define HWD I2C Read msg */
typedef PACKED struct
{
   ExeRspMsgT  RspInfo;
   uint16      DevAddr;
   uint16      GenDevAddr;
   uint32      SubAddr;
   uint16      NumBytes;
} HwdI2cReadMsgT;

/* Define HWD I2C Read Response msg */
typedef PACKED struct
{
   uint16      NumBytes;
   uint8       Data[HWD_I2C_BUF_SIZE_MAX];
} HwdI2cReadRspMsgT;

/* HWD_TST_GPIO_CTRL_MSG */
typedef PACKED struct
{
   uint8    Manual;
   uint8    Action;
	uint16   GpioNum;
} HwdTstGpioControlMsgT;

/* HWD_TST_GPIO_FUNC_SWITCH_MSG */
typedef PACKED struct
{
	uint16   GpioNum;
   uint8    Mode;
} HwdTstGpioFuncMsgT;

typedef struct
{
   bool VibratorEnable;        /* TRUE if Enable the vibrator */
} HwdVibratorMsgT;

#ifdef DENALI_EXTENDED_CUST_ETS

#define SW_VERSION_LEN 20
#define HW_VERSION_LEN 4

/* Define HWD SW version response msg */
typedef PACKED struct 
{
   uint8       SWVerInfo[SW_VERSION_LEN];
} HwdSWVersionRspMsgT;

/* Define HWD HW version response msg */
typedef PACKED struct 
{
   uint8       HWVerInfo[HW_VERSION_LEN];
} HwdHWVersionRspMsgT;
#endif /*DENALI_EXTENDED_CUST_ETS*/

/*----------------------------------------------------------------------------
 Global Function Prototypes
----------------------------------------------------------------------------*/

/* Initialization */
extern void HwdInit(void);
extern void	HwdTask(uint32 argc, void *argv);

/* watchdog functions */
extern void HwdWatchdogInit(void);

/* power saving functions */
extern bool HwdPwrSaveDspmClkState(void);
extern bool HwdPwrSaveDspvClkState(void);

/* Power detect enable/disable function */
extern void HwdTxPwrDetectEnable(bool Enable);

/* Calibration */
extern void HwdCalResetBandChan(void);

/* Build configuration */
bool HwdCDS4(void);

/* whether or not the chip is SCH enabled */
bool HwdSCHEnabled(void);

/* CLK functions */
extern void HwdSwitch32kClkSrc (HwdClkOption32KT New32kClkSource);
extern void HwdSet32KClock(void);
extern void HwdPowerDown32KClock(void);

/* PWMs */
#ifdef __cplusplus
extern "C" {
#endif
void  HwdWatchdogKick(void);
void  HwdPwmKeypadBacklightLevelSet(uint8 Level);
uint8 HwdPwmKeypadBacklightLevelGet(void);
void  HwdPwmKeypadBacklightBypassSet(bool Enable, uint8 Level);
void  HwdPwmKeypadBacklightBypassGet(bool *Enable, uint8 *Level);
void  HwdPwmLcdBacklightLevelSet(uint8 Level);
uint8 HwdPwmLcdBacklightLevelGet(void);
void  HwdPwmLcdBacklightBypassSet(bool Enable, uint8 Level);
void  HwdPwmLcdBacklightBypassGet(bool *Enable, uint8 *Level);
void  HwdPwmLcdContrastLevelSet(uint8 Level);
uint8 HwdPwmLcdContrastLevelGet(void);
void  HwdPwmLcdContrastBypassSet(bool Enable, uint8 Level);
void  HwdPwmLcdContrastBypassGet(bool *Enable, uint8 *Level);
#ifdef __cplusplus
}
#endif

#endif

#ifndef  _L1DAMPSAPI_H_
#define  _L1DAMPSAPI_H_


/*----------------------------------------------------------------------------
 Include Files
----------------------------------------------------------------------------*/
#include "sysapi.h"
#include "ipcapi.h"
#include "hwddefs.h"
#include "pswapi.h"

#ifdef SYS_OPTION_AMPS
/*----------------------------------------------------------------------------
 Global Defines And Macros
----------------------------------------------------------------------------*/
/* L1D AMPS Related Signals */
#define L1D_AMPS_STARTUP_SIGNAL       EXE_SIGNAL_1
#define L1D_AMPS_SSTIMER_SIGNAL       EXE_SIGNAL_2
#define L1D_AMPS_TUNE_COMPLETE_SIGNAL EXE_SIGNAL_3
#define L1D_AMPS_ACQ_DELAY_SIGNAL     EXE_SIGNAL_4

/* Mailbox IDs */
#define L1D_AMPS_MAILBOX              EXE_MAILBOX_1_ID   /* L1d AMPS task mailbox id */

/* Bit fields for use with ExeEventWait() */
#define L1D_AMPS_MAILBOX_BF           EXE_MAILBOX_1      /* L1d AMPS task mailbox */

extern const uint16 L1D_AMPS_FM_RSSI_FRAME_COUNT;
/*----------------------------------------------------------------------------
 Global Typedefs
----------------------------------------------------------------------------*/
/* Message ID's */
typedef enum
{
   L1D_AMPS_TX_AUDIO_CONTROL_MSG = 0,
   L1D_AMPS_RX_AUDIO_CONTROL_MSG,
   L1D_AMPS_SEND_RECC_MSG,
   L1D_AMPS_SEND_RVC_MSG,
   L1D_AMPS_SAT_CONTROL_MSG,
   L1D_AMPS_ST_CONTROL_MSG,
   L1D_AMPS_SELECT_STREAM_MSG,
   L1D_AMPS_SET_CHANNEL_FORMAT_MSG,
   L1D_AMPS_POWER_CONTROL_MSG,
   L1D_AMPS_TUNE_RADIO_MSG,
   L1D_AMPS_CARRIER_CONTROL_MSG,
   L1D_AMPS_SET_POWER_LEVEL_MSG,
   L1D_AMPS_GET_RSSI_MSG,
   L1D_AMPS_SCC_STATUS_MSG,
   L1D_AMPS_VOICE_STATUS_MSG,
   L1D_AMPS_SAT_SWITCHED_MSG,
   L1D_AMPS_TX_COMPLETE_MSG,
   L1D_AMPS_TST_VECTOR_MSG,
   L1D_AMPS_TST_KEY_RADIO_MSG,
   L1D_AMPS_RX_POLARITY_MSG,
   L1D_AMPS_CHANNEL_SEARCHER_MSG,
   L1D_AMPS_SHUTDOWN_MSG,
   L1D_AMPS_HWD_SETUP_MSG,
   L1D_AMPS_RSSI_RESPONSE_MSG,
   L1D_AMPS_DC_OFFSET_STATUS_MSG,
   L1D_AMPS_CFG_TIMERS_RSP_MSG,
   L1D_AMPS_CFG_POLARITY_RSP_MSG,
   L1D_AMPS_CFG_TX_VOICE_MODE_RSP_MSG,
   L1D_AMPS_CFG_RANDOM_DIST_RSP_MSG,
   L1D_AMPS_CFG_RX_WORD_SYNC_RSP_MSG,
   L1D_AMPS_CFG_RX_RSSI_RSP_MSG,
   L1D_AMPS_CFG_TX_LEVELS_RSP_MSG,
   L1D_AMPS_AFC_CONTROL_MSG,
   L1D_AMPS_SCC_CONTROL_MSG,
   L1D_AMPS_COMPRESSOR_CTRL_MSG,
   L1D_AMPS_SET_AFC_PARAMETERS_MSG,
   L1D_AMPS_WIDE_BAND_DATA_MSG,
   L1D_AMPS_BIS_RSP_MSG,
   L1D_AMPS_RX_DUTY_CYCLE_MSG,
   L1D_AMPS_NAM_RSP_MSG,
   L1D_AMPS_IDLE_MODE_ENABLE_MSG,
   L1D_AMPS_RX_CALIBRATION_MSG,
   L1D_AMPS_TX_CALIBRATION_MSG,
   L1D_AMPS_FOCC_IDLE_MODE_CONTROL_MSG,
   L1D_AMPS_RSSI_RESET_MSG,
   L1D_AMPS_SET_MODE_MSG,
   L1D_AMPS_INIT_NVRAM_ACK_MSG,
   L1D_AMPS_SET_TEST_MODE_MSG,
   L1D_AMPS_BUSY_IDLE_STATUS_TEST_MSG,
   L1D_AMPS_BUSY_IDLE_STATUS_RSP_MSG,
   L1D_AMPS_MODE_RSP_MSG,
   L1D_AMPS_MAX_NUM_MSG
} L1dAmpsCmdIdT;

/*-------------------------**
** Message Data Structures **
**-------------------------*/
typedef enum
{
   L1dAmpsIdleModeDisable = 0,
   L1dAmpsIdleModeEnable
} IdleModeControlT;

typedef PACKED struct
{
   IdleModeControlT IdleModeControl;
} L1dAmpsFoccIdleModeControlMsgT;

typedef PACKED struct
{
   uint16     Band;
   uint16     Channel;
   uint8      PowerLevel;
   bool       TxTestFlag;
   bool       SatTone;
   uint16     OffsetCarrierEn;
   uint16     OffsetCarrierFreq;
} L1dAmpsTxCalibrationMsgT;

typedef PACKED struct
{
   ExeRspMsgT RspInfo;
   uint16     Band;
   uint16     Channel;
   uint16     FrameCount;
} L1dAmpsRxCalibrationMsgT;

typedef PACKED struct
{
   uint8 On;
} L1dAmpsIdleModeEnableMsgT;

typedef PACKED struct
{
   uint32 FoccSlotIndex;
} L1dAmpsRxDutyCycleMsgT;

typedef enum
{
   AmpsMode = 0,
   CdmaMode,
   Other
} L1dAmpsHwdSetupT;

typedef PACKED struct
{
   uint16 HwdMode;
} L1dAmpsHwdSetupMsgT;
typedef PACKED struct
{
   bool On;
} L1dAmpsCompressorCtrlMsgT;

typedef IpcCpAmpBusyIdleRspMsgT          L1dAmpsBusyIdleRspMsgT;
typedef IpcCpAmpFwdChDataRspMsgT         L1dAmpsWideBandDataMsgT;
typedef IpcCpFmSignalStrengthRspMsgT     L1dAmpsRssiRspFromDspMsgT;
typedef IpcCpAmpFwdBusyIdleStatusRspMsgT L1dAmpsBusyIdleStatusRspMsgT;

typedef PACKED struct
{
   uint16 Repeats[3];
   int16  Count[3];
   int32  RxTxOffset[3];
   int16  ScaleFactor[3];
   int16  DutyCycle[3];
} L1dAmpsSetAfcParametersMsgT;

typedef PACKED struct
{
   bool SccOn;
} L1dAmpsSccControlMsgT;

typedef PACKED struct
{
   bool   AfcOn;
   uint8  AfcMode;
   uint8  ChannelFormat;
} L1dAmpsAfcControlMsgT;

typedef PACKED struct
{
   int32 CurrentDcOffset;
} L1dAmpsDcOffsetStatusMsgT;

typedef PACKED struct
{
  uint16 ChannelLow;
  uint16 ChannelHigh;
} L1dAmpsChannelSearcherMsgT;

typedef enum
{
   Normal = 0,
   Inverted
} L1dAmpsPolarityT;

typedef PACKED struct
{
   L1dAmpsPolarityT RxPolarity;
} L1dAmpsRxPolarityMsgT;

typedef PACKED struct
{
   bool KeyRadio;
}L1dAmpsTstKeyRadioMsgT;

typedef PACKED struct
{
   bool TxAudioOn;
} L1dAmpsTxAudioControlMsgT;

typedef PACKED struct
{
   bool RxAudioOn;
} L1dAmpsRxAudioControlMsgT;

#define L1D_MAX_RECC_WORD_SIZ 7

/* RECC word is 36 bits */
typedef PACKED struct
{
   uint8 Bytes[5];							/* Last 4 bits are not used */
} L1dAmpsReccWordT;

#define L1D_AMPS_RECC_NORMAL_MODE    0
#define L1D_AMPS_RECC_CONTINUOUS_ON  1
#define L1D_AMPS_RECC_CONTINUOUS_OFF 2
#define L1D_AMPS_RECC_BURST_MODE     3
typedef PACKED struct
{
   uint8  ReccMode;                         /* Mode of Operation */
   uint16 MaxBusy;							/* Access attempt parameter */
   uint16 MaxSzTr;							/* Access attempt parameter */
   bool ChkBis;								/* Check busy/idle option */
   uint8 Dcc;								/* Digital Colour Code */
   uint16 Size;								/* Number of RECC words */
   L1dAmpsReccWordT Word[L1D_MAX_RECC_WORD_SIZ];	/* RECC Data */
} L1dAmpsSendReccMsgT;

#define L1D_MAX_RVC_WORD_SIZ 4

/* RVC word is 36 bits */
typedef PACKED struct
{
   uint8 Bytes[5];							/* Last 4 bits are not used */
} L1dAmpsRvcWordT;

typedef PACKED struct
{
   uint16 Size;								/* Number of RVC words */
   L1dAmpsRvcWordT Word[L1D_MAX_RVC_WORD_SIZ];		/* RVC Data */
} L1dAmpsSendRvcMsgT;

typedef PACKED struct
{
   bool SatOn;
} L1dAmpsSatControlMsgT;

typedef PACKED struct
{
   bool StOn;
} L1dAmpsStControlMsgT;

typedef enum
{
   L1D_AMPS_STREAM_A = 0,
   L1D_AMPS_STREAM_B
} L1dAmpsStreamT;

typedef PACKED struct
{
   L1dAmpsStreamT Stream;
   uint32 Esn;
} L1dAmpsSelectStreamMsgT;

typedef enum
{
   L1D_AMPS_CHANNEL_FORMAT_IDLE = 0,
   L1D_AMPS_CHANNEL_FORMAT_CONTROL,
   L1D_AMPS_CHANNEL_FORMAT_VOICE,
   L1D_AMPS_CHANNEL_FORMAT_NONE
} L1dAmpsChannelFormatT;

typedef PACKED struct
{
   L1dAmpsChannelFormatT ChannelFormat;
} L1dAmpsSetChannelFormatMsgT;

typedef PACKED struct
{
  uint8 ChannelNumber;
} L1dAmpsChannelNumberMsgT;

typedef PACKED struct
{
   bool CarrierOn;
} L1dAmpsCarrierControlMsgT;

typedef PACKED struct
{
   uint16 Band;
   uint16 Channel;
} L1dAmpsTuneRadioMsgT;

typedef PACKED struct
{
   ExeRspMsgT RspInfo;
   uint16 FrameCount;
} L1dAmpsGetRssiMsgT;


typedef PACKED struct
{
   uint16 SampleCount;
} L1dAmpsBusyIdleStatusTestMsgT;


typedef PACKED struct
{
   uint16 RssiVal;
   uint16 RxAgcPdmVal;
   int16  CalibRssiVal;
} L1dAmpsRssiRspT;

typedef enum
{
   L1D_AMPS_POWER_LEVEL_0 = 0,
   L1D_AMPS_POWER_LEVEL_1,
   L1D_AMPS_POWER_LEVEL_2,
   L1D_AMPS_POWER_LEVEL_3,
   L1D_AMPS_POWER_LEVEL_4,
   L1D_AMPS_POWER_LEVEL_5,
   L1D_AMPS_POWER_LEVEL_6,
   L1D_AMPS_POWER_LEVEL_7
} L1dAmpsPowerLevelT;

typedef PACKED struct
{
   L1dAmpsPowerLevelT PowerLevel;
} L1dAmpsSetPowerLevelMsgT;

typedef PACKED struct
{
   uint16 Scc;
} L1dAmpsSccMsgT;

typedef PACKED struct
{
   uint16 NewScc;
} L1dAmpsSatSwitchedMsgT;

typedef enum
{
   L1D_AMPS_VOICE_INACTIVE = 0,
   L1D_AMPS_VOICE_ACTIVE
} L1dAmpsVoiceStatusT;

typedef PACKED struct
{
   uint16 VoiceStatus;
} L1dAmpsVoiceStatusMsgT;

typedef PACKED struct
{
  uint32 MsDelay;
} L1dAmpsTestVectorMsgT;

/********************************************************/
/* The Following Data Structures manage the Calibration */
/* and configuration data that is fetched from DBM.     */
/* These typedef's should match with the ETS templates. */
/********************************************************/
/* CP DB L1D AMPS CFG TIMERS */
#define RECC_TO_MSEC_TABLE_SIZ 7
typedef struct
{
   uint16 DspLatency;
   uint16 RxPllLockTime;
   uint16 AdcConversionTime;
   uint16 SetRfPowerTime;
   uint16 KeyRadioTime;
   uint16 Wait56BitTime;
   uint16 Wait104BitTime;
   uint16 ReccToMsecTable[RECC_TO_MSEC_TABLE_SIZ];
} L1dAmpsCfgTimersT;

typedef PACKED struct
{
   uint16 DspLatency;
   uint16 RxPllLockTime;
   uint16 AdcConversionTime;
   uint16 SetRfPowerTime;
   uint16 KeyRadioTime;
   uint16 Wait56BitTime;
   uint16 Wait104BitTime;
   uint16 ReccToMsecTable[RECC_TO_MSEC_TABLE_SIZ];
} L1dAmpsCfgTimersPkdT;
 
/* CP DB L1D AMPS CFG POLARITY */
typedef struct
{
   int8 TxPolarity;
   int8 RxPolarity;
} L1dAmpsCfgPolarityT;

typedef PACKED struct
{
   int8 TxPolarity;
   int8 RxPolarity;
} L1dAmpsCfgPolarityPkdT;

/* CP DB L1D AMPS CFG TX VOICE MODE */
typedef struct
{
   uint16 TxVoiceMode;
} L1dAmpsCfgTxVoiceModeT;

typedef PACKED struct
{
   uint16 TxVoiceMode;
} L1dAmpsCfgTxVoiceModePkdT;

/* CP DB L1D AMPS CFG RANDOM DIST */
typedef struct
{
   uint16 RandomDistFactor;
} L1dAmpsCfgRandomDistT;

typedef PACKED struct
{
   uint16 RandomDistFactor;
} L1dAmpsCfgRandomDistPkdT;

/* CP DB L1D AMPS CFG RX WORD SYNC */
typedef struct
{
   uint16 LostWsyncLimit;
} L1dAmpsCfgWordSyncT;

typedef PACKED struct
{
   uint16 LostWsyncLimit;
} L1dAmpsCfgWordSyncPkdT;

/* CP DB L1D AMPS CFG RX RSSI */
typedef struct
{
   int16 RssiFilterLen;
   int16 RssiFilterDivisor;
} L1dAmpsCfgRxRssiT;

typedef PACKED struct
{
   int16 RssiFilterLen;
   int16 RssiFilterDivisor;
} L1dAmpsCfgRxRssiPkdT;

/* CP DB L1D AMPS CAL TX AGC */
#define NUM_TX_LEVELS_ENTRY 8
typedef PACKED struct
{
   int8  LevelNumber;
   int16 TxPowerdBm;
} L1dAmpsTxLevelsEntryT;

typedef PACKED struct
{
   L1dAmpsTxLevelsEntryT Entry[NUM_TX_LEVELS_ENTRY];
} L1dAmpsTxLevelsT;

/* CP DB L1D AMPS CAL RX RSSI */
typedef struct
{
   uint8 RawValue;
   int8  CalibValue;
} L1dAmpsLimitT;

typedef struct
{
	L1dAmpsLimitT UpperLimit;
	int8          Slope;
	int16         YIntercept;
	L1dAmpsLimitT LowerLimit;
} L1dAmpsCalRxRssiT;

typedef PACKED struct
{
   uint8 RawValue;
   int8  CalibValue;
} L1dAmpsLimitPkdT;

typedef PACKED struct
{
	L1dAmpsLimitPkdT UpperLimit;
	int8             Slope;
	int16            YIntercept;
	L1dAmpsLimitPkdT LowerLimit;
} L1dAmpsCalRxRssiPkdT;

/* L1D_AMPS_SET_MODE_MSG */
typedef PACKED struct
{
   uint16 DspvMode;
} L1dAmpsSetModeMsgT;

/* L1D_AMPS_MODE_RSP_MSG */
typedef PACKED struct
{
   uint16 DspvMode;    /* Use IpcAmpModeT */
} L1dAmpsModeRspMsgT;

typedef PswIs95NamT L1dAmpsNamMsgT;

/*----------------------------------------------------------------------------
* L1a global APIs
*----------------------------------------------------------------------------*/

extern void L1dProcessAmpsSwitchover(void);
extern void L1dAmpsCalibInitFromCode(void);
extern void L1dAmpsCalibInitNvram(ExeRspMsgT *RspInfoP);
extern void L1dAmpsInitializeNvramSection(void);
extern void L1dAmpsCalibInitialize(void);

#endif




#endif

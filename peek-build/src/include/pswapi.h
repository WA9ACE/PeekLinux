#ifndef _PSWAPI_H_
#define _PSWAPI_H_


/*----------------------------------------------------------------------------
 Include Files
----------------------------------------------------------------------------*/
#include "sysparm.h"
#include "pswnam.h"
#include "sysdefs.h"
#include "exeapi.h"
#include "pswcustom.h"
#include "pswsmstune.h"
#include "hlwapi.h"
#include "pswnam.h"
#include "lmdapi.h"
#include "pdeapi.h"


/*----------------------------------------------------------------------------
 Global Defines and Macros
----------------------------------------------------------------------------*/
#define PSW_STARTUP_SIGNAL   EXE_SIGNAL_1

#define PSW_MAILBOX_CMD      EXE_MAILBOX_1_ID
#define PSW_MAILBOX2_CMD     EXE_MAILBOX_2_ID

#define PSW_SYNC_DATA_SIZE   4

/* These set list constants have been
   promoted to sysdefs.h
#define PSW_MAX_ACTIVE_MEASUREMENTS     6
#define PSW_MAX_CANDIDATE_MEASUREMENTS  10
#define PSW_MAX_NEIGHBOR_SET_SIZE       40
*/
#define PSW_MAX_REMAINING_MEASUREMENTS  20
#define PSW_MAX_SMS_SIZE                             256
#define PRM_MAX_SIZE                                     256

/* For LMD_REVERSE_ACCESS_SIG_MSG the amount of time from the current system */
/* time that the access message can be scheduled to be sent */
#define PSW_MAX_DELTA_TIME_TO_SEND_ACCESS_MSG 500

#define PSW_NUM_MUX1_COUNTER_INDEX 14
#define PSW_NUM_MUX2_COUNTER_INDEX 26
/* NAM defines */

/* Number of supported Modes */
/* 4 bands supported: Cellular, PCS, Skip, T53 */
#define PSW_NAM_NUM_BANDS_SUPPORTED 5

/* Operating Modes (OP_MODEp) settings */
#define PSW_NAM_OP_MODE0 0x80
#define PSW_NAM_OP_MODE1 0x40
#define PSW_NAM_OP_MODE2 0x20
#define PSW_NAM_OP_MODE3 0x10
#define PSW_NAM_OP_MODE4 0x08

/* Preferred Band Settings */
#define PSW_NAM_ENG_PREF_BAND_0       0
#define PSW_NAM_ENG_PREF_BAND_1       1
#define PSW_NAM_ENG_PREF_BAND_0_ONLY  2
#define PSW_NAM_ENG_PREF_BAND_1_ONLY  3
/* Amala K. 08/14/01 */ /* T53 */
#define PSW_NAM_ENG_PREF_BAND_3_ONLY  4
#define PSW_NAM_ENG_PREF_BAND_4_ONLY  5

/* Preferred Mode Settings */
#define PSW_NAM_ENG_PREF_MODE_ANALOG       0
#define PSW_NAM_ENG_PREF_MODE_CDMA         1
#define PSW_NAM_ENG_PREF_MODE_ANALOG_ONLY  2
#define PSW_NAM_ENG_PREF_MODE_CDMA_ONLY    3

/* Preferred Serving System - Band 0 */
#define PSW_NAM_ENG_PREF_SYS_A       0
#define PSW_NAM_ENG_PREF_SYS_B       1
#define PSW_NAM_ENG_PREF_SYS_A_ONLY  2
#define PSW_NAM_ENG_PREF_SYS_B_ONLY  3

/* Slotted Mode Bit mask in SCM */
#define PSW_NAM_CP_SLOTTED_MODE_BIT_MASK 0x20

/* Data service page acceptance bit map. */
#define PSW_ASYNC_DATA_PAGE_ACCEPT   1
#define PSW_FAX_PAGE_ACCEPT          2
#define PSW_ASYNCDATA_AND_FAX_PAGE_ACCEPT	3

/* full 36bit system time size in bytes */
#define PSW_SYSTIME_SIZE             5

/* Number of service options in the list can be disabled. */
#define PSW_NUMBER_SO_DISABLE_LIST   29

/* Updated NAM Data Mask Bits Defined (whatChangedInNam) */
#define SLOT_CYCLE_INDEX_UPDATED     0x0001
#define SCM_UPDATED                  0x0002
#define MOB_TERM_HOME_UPDATED        0x0004
#define MOB_TERM_FOR_SID_UPDATED     0x0008
#define MOB_TERM_FOR_NID_UPDATED     0x0010
#define BAND_MODE_UPDATED			 0x0020
#define AKEY_UPDATED				 0x0040

/* Location Services Defines */
#define MAX_NUM_PS_RANGES_P			 64



#define  PSW_FEATURE_TATA  0x80000000
#define  PSW_FEATURE_RELIANCE   0x40000000
#define  PSW_FEATURE_UNICOM   0x20000000
  
#define PSW_FEATURE_UIMSUPPORT   0x00000001
#define PSW_FEATURE_MINLOCK   0x00000002
#define PSW_FEATURE_SIDLOCK   0x000000004       


/****************************************************************************
 * Other Global declarations.
 ***************************************************************************/
extern bool VoicePrivacy;


/*----------------------------------------------------------------------------
 Global Typedefs
----------------------------------------------------------------------------*/

extern const RetrievableAndSettableParametersT parameterIDTable[];

/* Task message Id's */
typedef enum
{
   /* from multiple sources */
   PSW_CMD_STATUS_MSG,

   /* from L1D */
   PSW_PMRM_REQ_MSG,
   PSW_CAND_FREQ_TIMES_MSG,
   PSW_CAND_FREQ_MEASUREMENTS_MSG,
   PSW_SCAN_MEASUREMENTS_MSG,
   PSW_IDLE_HANDOFF_MSG,
   PSW_START_CF_MEASURE_MSG,
   PSW_END_CF_MEASURE_MSG,
   PSW_SET_IHO_THRESHOLDS_MSG,
   PSW_SET_IHO_TIMER_MSG,
   PSW_SET_NEW_IHO_PARAMETERS_MSG,
    /*A. Kulkarni. 10/31/01. CBP3_MERGE*/
   PSW_POWER_REPORT_MSG,
   PSW_SYNC_PILOT_STRENGTH_MSG,
   /*AKK 01-26-01 Added the following for the phase2 forward power control */
   PSW_OUTER_LOOP_REPORT_REQ_MSG,
   /*END--AKK 01-26-01 Added the following for the phase2 forward power control */
   PSW_GET_PS_INFO_MSG,
   PSW_L1D_ACCESS_PREAMBLE_COMPLETE_MSG,
   PSW_L1D_ACCESS_PROBE_COMPLETE_MSG,
#ifdef KDDI_EXTENSIONS
   PSW_L1D_RSSI_RPT_MSG,
#endif

   /* from LMD */
   PSW_FORWARD_SYNC_CHAN_FRAME_MSG,
   PSW_FORWARD_PAGE_CHAN_FRAME_MSG,
   PSW_FORWARD_BCCH_FRAME_MSG,
   PSW_FORWARD_FCCCH_FRAME_MSG,
   PSW_DSCH_CHAN_FRAME_MSG,
   PSW_REVERSE_STATISTICS_PARMS_MSG,
   PSW_SERVICE_OPTION_CONTROL_RESP_MSG,
   PSW_RSCH_REL_IND_MSG,

   /* from MMI */
   PSW_ENABLE_MSG,
   PSW_POWER_MSG,
   PSW_CALL_INITIATE_MSG,
   PSW_CALL_INITIATE_WITH_INFO_REC_MSG,
   PSW_GET_PRL_INFO_MSG,
   PSW_E911_MODE_MSG,
   PSW_CALL_ANSWER_MSG,
   PSW_CALL_HANGUP_MSG,
   PSW_SET_DUAL_BAND_CALIBRATION_MSG,
   PSW_SEND_CONT_DTMF_ON_MSG,
   PSW_SEND_CONT_DTMF_OFF_MSG,
   PSW_SEND_BURST_DTMF_MSG,
   PSW_HOOKFLASH_MSG,
   PSW_VOICE_PRIV_ON_MSG,
   PSW_VOICE_PRIV_OFF_MSG,
   PSW_SET_RETRIEVABLE_PARAMETER,
   PSW_GET_RETRIEVABLE_PARAMETER,
   PSW_INJECT_HANDOFF_MSG,

   PSW_DATA_BURST_MSG,
   PSW_DATA_BURST_REJ_MSG,
   PSW_GET_FULL_SYS_TIME_MSG,
   PSW_SET_SILENT_RETRY_MSG,
   PSW_GET_SILENT_RETRY_STATUS_MSG,
   PSW_SERVICE_OPTION_DISABLE_MSG,
   PSW_GET_NAM_MSG,
   PSW_NAM_CHANGED_MSG,
   PSW_SET_CUSTOM_FEATURE_MSG,

   /* from MMI - SMS related */
   PSW_SMS_PREF_SRV_OPT,
   PSW_SMS_CONNECT_MSG,
   PSW_SMS_DISCONNECT_MSG,
   PSW_SMS_BCAST_CONNECT_MSG,
   PSW_SMS_BCAST_DISCONNECT_MSG,
   PSW_SMS_BCAST_PARMS_MSG,
   PSW_SMS_SUBMIT_PARMS_MSG,
   PSW_SMS_SUBMIT_DATA_MSG,
   PSW_SMS_CANCEL_MSG,
   PSW_SMS_ACK_PARMS_MSG,
   PSW_SMS_ACK_DATA_MSG,
   PSW_SMS_TERMINAL_STATUS_MSG,
   PSW_SMS_TIMER_EXPIRED_MSG,
   PSW_SMS_CAUSE_CODE_STATUS_MSG,
   PSW_SMS_RETX_AMOUNT_MSG,
   PSW_SMS_CAUSE_CODE_SET_MSG,
   PSW_SMS_REPLY_TL_ACK_MSG,
   /* from SCC - SMS related */
   PSW_SMS_TX_RSP_MSG,

   /* from EXE */
   PSW_TIMER_EXPIRED_MSG,
   PSW_TA_TIMER_EXPIRED_MSG,

   /* from DBM */
   PSW_DBM_DATA_NAM_MSG,
   PSW_DBM_DATA_NAM_WRITE_ACK_MSG,
   PSW_DBM_DATA_MSG,
   PSW_DBM_DATA_WRITE_ACK_MSG,
   PSW_DBM_FLUSH_ACK_MSG,
   PSW_DBM_DATA_PRL_MSG,
   PSW_DBM_DATA_PRL_WRITE_MSG,
   PSW_DBM_DATA_MS_CAP_DB_MSG,
   PSW_DBM_DATA_MS_CAP_DB_WRITE_ACK_MSG,
   PSW_DBM_DATA_MISC_MSG,
   PSW_NAM_CHANGED_BY_ETS_MSG,
   PSW_NAM_UNLOCK_ACK_MSG,
   PSW_DBM_DATA_MISC_WRITE_ACK_MSG,

   /* from R-UIM */
   PSW_UIM_GET_NAM_DATA_MSG,
   PSW_UIM_GET_UIM_MEID_FEATURE_MSG,
   PSW_UIM_UPDATE_NAM_DATA_MSG,
   PSW_UIM_STORE_ESN_MSG,
   PSW_UIM_DATA_PRL_MSG,
      
   PSW_UIM_BS_CHALLENGE_ACK_MSG,
   PSW_UIM_CONFIRM_SSD_ACK_MSG,
   PSW_UIM_RUN_CAVE_ACK_MSG,
   PSW_UIM_GENERATE_KEY_VPM_ACK_MSG,
   PSW_UIM_STATUS_NOTIFY_MSG,
   PSW_UIM_DATA_OTAFeature_MSG,
   /*    otasp/otapa     */
    PSW_UIM_MS_KEY_REQUEST_ACK_MSG,                      
    PSW_UIM_KEY_GENERATION_REQUEST_ACK_MSG,         
    PSW_UIM_COMMIT_ACK_MSG,                                  
    PSW_UIM_VALIDATE_ACK_MSG,                                    
    PSW_UIM_CONFIGURATION_REQUEST_ACK_MSG,    
    PSW_UIM_DOWNLOAD_REQUEST_ACK_MSG,    
    PSW_UIM_SSPR_CONFIGURATION_REQUEST_ACK_MSG, 
    PSW_UIM_SSPR_DOWNLOAD_REQUEST_ACK_MSG,  
    PSW_UIM_OTAPA_REQUEST_ACK_MSG,          
    PSW_UIM_UPDATE_BINARY_ACK_MSG,
    PSW_UIM_OTA_RUN_CAVE_ACK_MSG,
    PSW_UIM_STORE_MECONFIG_MSG,
   
   /* from RLP */
   PSW_DATA_BUFFER_STATUS_MSG,

   /* from ets - debug */
   PSW_ACCESS_SIGNAL_ETS_MSG,
   PSW_TEST_MODE_TIMERS_USED_MSG,
   /*AKK 07-15-02 QPCH/PI/CCI/ETS Thresholds Mods*/
   PSW_QPCH_T1_THRESHOLDS_MSG,
   PSW_QPCH_R_THRESHOLDS_MSG,
   PSW_QPCH_SNR_THRESHOLDS_MSG,
   /*END--AKK 07-15-02 QPCH/PI/CCI/ETS Thresholds Mods*/
   PSW_ENGINE_TEST_MODE_MSG,
   PSW_STATUS_REQUEST_MSG,
   PSW_PHONE_STATE_MSG,
   PSW_INIT_NAM_MSG,
   PSW_OUT_SERV_AREA_PARMS_MSG,
   PSW_MS_CAP_DB_WRITE_MSG,
   PSW_HSPD_ACTIVE_CONTROL_HOLD_MSG,
   PSW_RSCH_REQUEST_MSG,
   PSW_GET_P_REV_IN_USE_MSG,
   PSW_CLEAR_MRU_MSG,
   PSW_GET_ACTIVE_NAM_MSG,
   PSW_SELECT_ACTIVE_NAM_MSG,
   PSW_FORCE_ANALOG_MODE_MSG,
   PSW_ENTER_AKEY_MSG,
   PSW_SET_ACCESS_HO_PARAMETERS_MSG,
   PSW_DEBUG_SCREEN_INFO_REQUEST,
   PSW_DEBUG_SCRN_LMD_RSP_MSG,
   PSW_DEBUG_SCRN_L1D_RSP_MSG,
   PSW_EXIT_EMERGENCY_MODE_MSG,

#ifdef SYS_OPTION_AMPS
   /* AMPS related message ID's */
   PSW_AMPS_BB_IND_TX_STAT_MSG,
   PSW_AMPS_BB_IND_CHAN_FORMAT_MSG,
   PSW_AMPS_BB_IND_SAT_CC_MSG,
   PSW_AMPS_BB_IND_FOCC_DATA_MSG,
   PSW_AMPS_BB_IND_FVC_DATA_MSG,

   PSW_AMPS_RELAY_RADIO_TUNE_STAT_MSG,
   PSW_AMPS_RELAY_RADIO_FSU_LOCK_MSG,
   PSW_AMPS_RELAY_RADIO_RSSI_BAL_MSG,
   PSW_AMPS_RELAY_RADIO_CARRIER_STAT_MSG,
   PSW_AMPS_RELAY_RADIO_POWER_LVL_MSG,
   PSW_AMPS_RELAY_RADIO_AFC_COMPLETE_MSG,

   PSW_AMPS_CP_RQ_ENABLE_MSG,
   PSW_AMPS_CP_RQ_POWER_DOWN_MSG,
   PSW_AMPS_CP_RQ_CALL_INITIATE_MSG,
   PSW_AMPS_CP_RQ_CALL_ANSWER_MSG,
   PSW_AMPS_CP_RQ_CALL_HANGUP_MSG,

   PSW_AMPS_AUDIO_VOICE_MSG,

   PSW_AMPS_READ_DATA_ACK_MSG,

   PSW_AMPS_POWER_MSG,

   PSW_AMPS_WBIOS_INIT_COMPLETE_MSG,

   PSW_AMPS_SET_RSSI_TH_MSG,
   PSW_AMPS_WORD_SYNC_RSP_MSG,
   PSW_STATUS_REQUEST_RSSI_RSP_MSG,
#endif

   PSW_DATA_CALL_ANSWER_MSG,
   PSW_PS_CALL_SETUP_MSG,
   PSW_PS_RECONNECT_MSG,
   PSW_DS_CALL_REL_MSG,
   PSW_PS_CALL_REL_MSG,
   PSW_LS_CALL_REL_MSG,
   PSW_DS_ENABLE_MSG,
   PSW_PS_DORMANT_REQ_MSG,
   PSW_DATA_PREF_REQ_MSG,
   PSW_RLP_OPEN_RSP_MSG,
   PSW_RLP_CLOSE_RSP_MSG,
   PSW_RLP_IDLE_IND_MSG,
   PSW_VS_PREF_REQ_MSG,
   PSW_LS_PREF_REQ_MSG,
   PSW_SMS_PREF_REQ_MSG,
   PSW_SMS_REL_REQ_MSG,
   PSW_SMS_TX_REQ_MSG,
   PSW_SMS_PARM_BRCST_MSG,
   PSW_OTASP_RCVD_MSG,
   PSW_OTASP_REL_MSG,
   PSW_OTASP_TX_REQ_MSG,
   PSW_OTASP_SO_PREF_MSG,
   PSW_VS_SWITCH_SO_MSG,
   PSW_VS_DS_PREARANG_MSG,
   PSW_SET_SERVICE_CONFIG_MSG,

   /* Statistics counter retrieve response message */
   PSW_STAT_CNT_MSG,

   PSW_EXP_MOD_MS_RESULT_MSG,
   PSW_EXP_MOD_BS_RESULT_MSG,
   PSW_FILL_SSPR_RSP_MSG,
   PSW_IOTA_IS683_MSG,
   PSW_IOTA_PRL_MSG,
   PSW_CSS_BAL_SYNC_RSP_MSG,

   /* IS801 Location Services */
   PSW_POS_AFLT_MEASUREMENTS_MSG,
   PSW_POS_AFLT_ACK_ABORT_MSG,
   PSW_POS_AFLT_ABORT_REQ_MSG,
   PSW_IS801_TCPB_REV_DATA_RESP_INFO_MSG,
   PSW_IS801_TCPB_FWD_DATA_RESP_INFO_MSG,
   PSW_IS801_SESSION_START_MSG,
   PSW_IS801_TCP_CONNECT_MSG,
   PSW_IS801_MS_ORIG_REQ_MSG,
   PSW_IS801_MS_CANCEL_REQ_MSG,
   PSW_POS_REJECT_MSG,
   PSW_LOC_PREF_REQ_MSG,
   PSW_PDE_SESSION_ABORT_REQ_MSG,
   PSW_PDE_PSEUDORANGE_MSMT_RSP_MSG,
   PSW_PDE_SESSION_END_RSP_MSG,
   PSW_PDE_GPS_INOP_MSG,
   PSW_GPS_ENABLE_REPORTS_MSG,
   PSW_TEST_LOC_PDDM_MSG,
   PSW_TEST_POWER_CONTROL_MSG,
   PSW_L1D_ALTERNATE_PILOTS_MSG,
   PSW_LOCATION_SVC_CFG_MSG,
    PSW_SET_MULTI_SMS_MODE_MSG,
#ifdef OTTS
    PSW_FORWARD_PAGE_CHAN_MSG,
    PSW_FORWARD_FCCCH_MSG,
    PSW_FORWARD_BCCH_MSG,
    PSW_FORWARD_SYNC_CHAN_MSG,
    PSW_DSCH_CHAN_MSG,
#endif
#ifdef SYS_OPTION_MIP_DMU
    PSW_HLW_MIP_KEYS_UPDATE_MSG,
#endif
   PSW_READ_RAW_IMSI_MSG,
   PSW_WRITE_RAW_IMSI_MSG,
   PSW_L1D_SLOTTED_VETO_MSG,
   PSW_NUM_MSG_IDS
} PswMsgIdT;

typedef enum
{
   PSW_PILOT_ACQUIRE,
   PSW_NO_PILOT,
   PSW_SYNC_ACQUIRE_FAIL,
   PSW_SYNC_TO_SYS_TIME,
   PSW_SYNC_TO_SYS_TIME_FAIL,
   PSW_PAGE_MONITOR,
   PSW_PAGE_MONITOR_FAIL,
   PSW_SEND_ACCESS_MSG_FAIL,
   PSW_SEND_TC_MSG,
   PSW_SEND_TC_MSG_FAIL,
   PSW_TC_CONFIG,
   PSW_LONG_CODE_CHANGED,
   PSW_RADIO_TUNED,
   PSW_RADIO_NOT_TUNED,
   PSW_PILOT_LOST,
   PSW_PUF_PROBE_SENT,
   PSW_PUF_PROBE_SENT_MAX_PWR,
   PSW_PUF_PROBE_NOT_SENT,
   PSW_IND_WAKEUP,
   PSW_IND_PILOT_SET,
   PSW_HO_COMPLETE,
   PSW_OOSA_WAKEUP,
   PSW_AMPS_HO_COMPLETE,
   PSW_RSCH_STARTED,
   PSW_FSCH_STARTED,
   PSW_FSCH_TERMINATED,
   PSW_RSR_UPDATED,
   PSW_NUM_CMD_STATUS
} PswCmdStatusT;

typedef enum
{
   PSW_FRAME_CATAGORY_RS1_9600_FULL_PRI = 1,
   PSW_FRAME_CATAGORY_RS1_9600_DIM_BURST_HALF_PRI_SIG = 2,
   PSW_FRAME_CATAGORY_RS1_9600_DIM_BURST_QUARTER_PRI_SIG = 3,
   PSW_FRAME_CATAGORY_RS1_9600_DIM_BURST_EIGHTH_PRI_SIG = 4,
   PSW_FRAME_CATAGORY_RS1_9600_BLANK_BURST_SIG = 5,
   PSW_FRAME_CATAGORY_RS1_4800_FULL_PRI = 6,
   PSW_FRAME_CATAGORY_RS1_2400_FULL_PRI = 7,
   PSW_FRAME_CATAGORY_RS1_1200_FULL_PRI = 8,
   PSW_FRAME_CATAGORY_RS1_9600_FULL_PRI_BIT_ERRORS = 9,
   PSW_FRAME_CATAGORY_RS1_INSUFFICIENT_FRAME_QUALITY = 10,
   PSW_FRAME_CATAGORY_RS1_9600_DIM_BURST_HALF_PRI_SEC = 11,
   PSW_FRAME_CATAGORY_RS1_9600_DIM_BURST_QUARTER_PRI_SEC = 12,
   PSW_FRAME_CATAGORY_RS1_9600_DIM_BURST_EIGHTH_PRI_SEC = 13,
   PSW_FRAME_CATAGORY_RS1_9600_BLANK_BURST_SEC = 14,
   PSW_FRAME_CATAGORY_RS1_9600_DCCH_NULL_FRAME = 15,
   PSW_FRAME_CATAGORY_RS2_14400_FULL_PRI = 1,
   PSW_FRAME_CATAGORY_RS2_14400_DIM_BURST_HALF_PRI_SIG = 2,
   PSW_FRAME_CATAGORY_RS2_14400_DIM_BURST_QUARTER_PRI_SIG = 3,
   PSW_FRAME_CATAGORY_RS2_14400_DIM_BURST_EIGHTH_PRI_SIG = 4,
   PSW_FRAME_CATAGORY_RS2_14400_BLANK_BURST_SIG = 5,
   PSW_FRAME_CATAGORY_RS2_14400_DIM_BURST_HALF_PRI_SEC = 6,
   PSW_FRAME_CATAGORY_RS2_14400_DIM_BURST_QUARTER_PRI_SEC = 7,
   PSW_FRAME_CATAGORY_RS2_14400_DIM_BURST_EIGHTH_PRI_SEC = 8,
   PSW_FRAME_CATAGORY_RS2_14400_BLANK_BURST_SEC = 9,
   PSW_FRAME_CATAGORY_RS2_14400_DIM_BURST_EIGHTH_PRI_SEC_SIG = 10,
   PSW_FRAME_CATAGORY_RS2_7200_FULL_PRI = 11,
   PSW_FRAME_CATAGORY_RS2_7200_DIM_BURST_QUARTER_PRI_SIG = 12,
   PSW_FRAME_CATAGORY_RS2_7200_DIM_BURST_EIGHTH_PRI_SIG = 13,
   PSW_FRAME_CATAGORY_RS2_7200_BLANK_BURST_SIG = 14,
   PSW_FRAME_CATAGORY_RS2_7200_DIM_BURST_QUARTER_PRI_SEC = 15,
   PSW_FRAME_CATAGORY_RS2_7200_DIM_BURST_EIGHTH_PRI_SEC = 16,
   PSW_FRAME_CATAGORY_RS2_7200_BLANK_BURST_SEC = 17,
   PSW_FRAME_CATAGORY_RS2_7200_DIM_BURST_EIGHTH_PRI_SEC_SIG = 18,
   PSW_FRAME_CATAGORY_RS2_3600_FULL_PRI = 19,
   PSW_FRAME_CATAGORY_RS2_3600_DIM_BURST_EIGHTH_PRI_SIG = 20,
   PSW_FRAME_CATAGORY_RS2_3600_BLANK_BURST_SIG = 21,
   PSW_FRAME_CATAGORY_RS2_3600_DIM_BURST_EIGHTH_PRI_SEC = 22,
   PSW_FRAME_CATAGORY_RS2_3600_BLANK_BURST_SEC = 23,
   PSW_FRAME_CATAGORY_RS2_1800_FULL_PRI = 24,
   PSW_FRAME_CATAGORY_RS2_1800_BLANK_BURST_SEC = 25,
   PSW_FRAME_CATAGORY_RS2_INSUFFICIENT_FRAME_QUALITY = 26,
   PSW_FRAME_CATAGORY_RS2_14400_DCCH_NULL_FRAME = 27
} PswFrameCatagoryT;

typedef PACKED struct
{
   bool     Stale;
   uint16   PnPhase;
   uint16   Strength;
} PswPilotMeasurementT;

typedef PACKED struct
{
   uint16   PilotPN;
   bool     Stale;
   uint16   PnPhase;
   uint16   Strength;
} PswRemainPilotMeasurementT;

/*------------------------------------------------------------------------
  Define funtion prototypes
-------------------------------------------------------------------------*/

extern PswIs95NamT* PswNamGet (void);


/*******************/
/* DBM definitions */
/*******************/

/*------------------------------------------------------------------------
* Nam Data Structures
*  The name of the structure is misleading. It should support everything
*  IS95, T53 and AMPS.
*
*  This structure should exactly match the structure "IS95Nam" in the
*   protocol engine header file "namdata.h"
*------------------------------------------------------------------------*/

typedef PACKED struct
{
  BandClass     CdmaBand[MAX_MRU_RECORDS];
  uint16        FreqChan[MAX_MRU_RECORDS];
} PswDbmDataT;

typedef enum
{
   PSW_AYNC_DATA = 1,   /* bit 0 */
   PSW_FAX = 2,         /* bit 1 */
   PSW_UPB_ASYNC = 64   /* bit 6 */
} PswDsEnableBitMapT;

typedef PACKED struct
{
   bool CssSyncValOk;
} PswCssValSyncRspMsgT;

/***********************/
/* Message Definitions */
/***********************/

/*AKK 04-25-02 QPCH/PI/CCI mods*/
typedef enum
{
   PI_ON      = 0,
   PI_OFF     = 1,
   PI_UNDEF   = 2,
   PI_ERASURE = 3
} QpchPiStat;

typedef enum
{
   PSW_PCH   = 0,
   PSW_QPCH1 = 1,
   PSW_QPCH2 = 2,
   PSW_CCI   = 3,
   PSW_FCCCH = 4
} WakeModeT;

typedef PACKED struct
{
   WakeModeT WakeMode;               /* Status of a command previously sent by PS */
   int16    CciPiPwr;
   int16    PilotPwr;
   uint16   PilotEcIoPwr;
   /*AKK 06-26-02 Per Spirent Req*/
   uint16   QpchPilotPn;
   int16    QpchBitIEnergy;
   int16    QpchBitQEnergy;
   int16    QpchPilotIEnergy;
   int16    QpchPilotQEnergy;
   /*AKK 06-03-02 QPCH/PI/CCI mods*/
   uint32   SysTime;
} PswWakeStatusT;

typedef PACKED struct
{
   PswCmdStatusT  CmdStatus;               /* Status of a command previously sent by PS */
   /*AKK 04-25-02 QPCH/PI/CCI mods*/
   PswWakeStatusT WakeStatus;               /* Status of a command previously sent by PS */
} PswCmdStatusMsgT;
/*END--AKK 05-17-02 QPCH/PI/CCI*/

typedef PACKED struct
{
   uint16   PnPhase;
   uint16   Strength;
} PswCandFreqMeasT;

typedef PACKED struct
{
   int16 cfTotalRxPwr;   /* Total Rx Power on CF */
   uint8 NumPilots;                        /* Number of pilot measurements */
   PswCandFreqMeasT CfPilotList[SYS_CP_MAX_NEIGHBOR_LIST_PILOTS];
}PswCandFreqMeasurementsMsgT;

typedef PACKED struct
{
   uint8 TotalOffTimeFwd;                  /* Total time spent off of forward channel */
   uint8 MaxOffTimeFwd;                    /* Max time spent off of forward channel   */
   uint8 TotalOffTimeRev;                  /* Total time spent off of reverse channel */
   uint8 MaxOffTimeRev;                    /* Max time spent off of reverse channel   */
}PswCandFreqTimesMsgT;

/* A. Kulkarni. 10/31/01. CBP3_MERGE. */
typedef PACKED struct
{
   INT8   Power;                           /* TC Frame Power Data */
} PswPowerReportMsgT;

typedef PACKED struct
{
   uint16   PilotPN;                       /* Pilot PN to handoff to */
} PswIdleHandoffMsgT;

typedef PACKED struct
{
   SysSystemTimeT SysTime;                 /* System time the frame was received at */
   uint16         DataSize;                /* Data size in bytes (12 or 24) */
   uint8          Data[1];                 /* Starting address of frame data */
}PswForwardPageChanFrameMsgT;

typedef PACKED struct
{
    SysSystemTimeT      SysTime;            /* System time the frame was received at */
    uint8               MuxPduType;
    uint8               Category;
    uint8               Data[1];            /* Starting address of frame data */
}PswDschFrameMsgT;

typedef PACKED struct
{
    uint8       SchId;
    uint16      SchBadFrames;
    uint16      SchTotFrames;
} PswPrmSchT;

typedef PACKED struct
{
    bool        FchMeasIncl;      /* FCH measurement included */
    uint8       FchBadFrames;     /* BAD_FRAMESs in IS2000A.5 2.6.4.1.1*/
    uint16      FchTotFrames;     /* TOT_FRAMESs in IS2000A.5 2.6.4.1.1*/
    bool        DcchMeasIncl;     /* DCCH measurement included */
    uint8       DcchBadFrames;    /* BAD_FRAMESs in IS2000A.5 2.6.4.1.1*/
    uint16      DcchTotFrames;    /* TOT_FRAMESs in IS2000A.5 2.6.4.1.1*/
    uint8       NumForSch;        /* Number of FSCH to report */
    PswPrmSchT  SchPmr[ SYS_MAX_FSCH ];
}PswPmrMsgT;

/*AKK 01-26-01 Added the following for phase2 Outer Loop Report Message */
typedef PACKED struct
{
    uint8		SchId;
	uint8		FpcSchCurrSetPt;
}PswFpcSchReportT;

typedef PACKED struct
{
	bool		     FchIncl;
	uint8		     FpcFchCurrSetPt;
	bool		     DcchIncl;
	uint8		     FpcDcchCurrSetPt;
	uint8		     NumSup;
	PswFpcSchReportT FpcSch[2];
}PswOuterLoopReportReqMsgT;
/*END--AKK 01-26-01 Added the following for phase2 Outer Loop Report Message */

typedef PACKED struct
{
   SysSystemTimeT SysTime;                 /* System time the frame was received at */
   uint16         DataSize;                /* Data size in bytes (23, 47 or 95)     */
   uint8          Data[1];                 /* Starting address of frame data        */
}PswFCCCHFrameMsgT;

typedef PACKED struct
{
   SysSystemTimeT SysTime;                 /* System time the frame was received at */
   uint16         DataSize;                /* Data size in bytes (93 bytes fixed)   */
   uint8          Data[1];                 /* Starting address of frame data        */
}PswBCCHFrameMsgT;

typedef PACKED struct
{
   SysSystemTimeT FrameNum;                /* Frame number this signaling data */
                                           /* was received. Note: There is no system */
                                           /* time while on the sync channel so a */
                                           /* counter is used for the frame time */
   uint8          Data[PSW_SYNC_DATA_SIZE];  /* This is a fixed 24 bit size */
}PswForwardSyncChanFrameMsgT;

typedef PACKED struct
{
   SysSystemTimeT SysTime;         /* System time the frame was received at */
   uint16         DataSize;        /* Data size in bytes */
   uint8          Data[1];         /* Starting address of frame data */
}PswForwardBCCHFrameMsgT;

typedef PACKED struct
{
   SysSystemTimeT SysTime;         /* System time the frame was received at */
   uint16         DataSize;        /* Data size in bytes */
   uint8          Data[1];         /* Starting address of frame data */
}PswForwardFCCCHFrameMsgT;

typedef PACKED struct
{
   SysSystemTimeT       SysTime;        /* System time the frame was received at */
   SysMultiplexOptionT  MultiplexOpt;   /* multiplex option being used */
                                        /* (i.e. rate set) */
   uint16               Strength;       /* Power of the pilot in Q16 linear units */
   PswFrameCatagoryT    Catagory;       /* Frame category - size of data derived from this */
   uint8                Data[1];        /* Starting address of frame data */
}PswForwardTrafficChanFrameMsgT;


typedef PACKED struct
{
   bool     PowerUp;                       /* TRUE power up, FALSE power down */
} PswPowerMsgT;

typedef PACKED struct
{
   int16  PilotSeq;
   uint16 ReferencePN;
   uint8 NumActive;                        /* Number of active set pilot measurements */
   uint8 NumCandidate;                     /* Number of candidate set pilot measurements */
   uint8 NumNeighbor;                      /* Number of neighbor set pilot measurements */
   PswPilotMeasurementT Active [SYS_CP_MAX_ACTIVE_LIST_PILOTS];
   PswPilotMeasurementT Candidate [SYS_MAX_CANDIDATE_LIST_PILOTS];
   PswPilotMeasurementT Neighbor [SYS_CP_MAX_NEIGHBOR_LIST_PILOTS];
} PswScanMeasurementsMsgT;

#ifdef KDDI_EXTENSIONS
/* MMI_L1D_RSSI_RPT_MSG */
typedef PACKED struct
{
   int16    Rssi;
} PswL1dRssiRptMsgT;
#endif

typedef enum
{
    PSW_CRC,                               /* Enable/Disable CRC Checking */
    PSW_REMAIN_IN_SYNC_STATE,              /* Force MS to stay on Sync Channel */
    PSW_ENABLE_IDLE_HANDOFF,               /* Enable Idle Handoff */
    PSW_ENABLE_MSG_INJECTION,              /* Message Injection Test Mode */
    PSW_RETIEVE_STATISTICS,                /* Retrieve statistics parameters */
    PSW_DUMP_PAGE_STATS,                   /* Dump page channel statistics */
    PSW_DUMP_ACCESS_STATS,                 /* Dump access channel statistics */
    PSW_DUMP_FTC_STATS,                    /* Dump FTC statistics */
    PSW_INVALID_TEST_MODE                  /* WARNING! Must not match any of Isotel TestMode values */
} PswTestModeT;

typedef PACKED struct
{
   bool Enable;                            /* Enable/Disable protocol stack flag */
} PswEnableMsgT;

typedef enum
{
   PSW_REQUEST_MODE_CDMA_ONLY = 1,
   PSW_REQUEST_MODE_WIDE,
   PSW_REQUEST_MODE_CDMA_WIDE,
   PSW_REQUEST_MODE_NARROW_ONLY,
   PSW_REQUEST_MODE_CDMA_NARROW_ONLY,
   PSW_REQUEST_MODE_NARROW_WIDE,
   PSW_REQUEST_MODE_NARROW_WIDE_CDMA
} PswRequestModeT;
typedef enum
{
   PSW_NULL_SERVICE_OPTION    = 0,

   PSW_SERVICE_OPTION_1       = 1,
   PSW_SERVICE_OPTION_2       = 2,
   PSW_SERVICE_OPTION_3       = 3,
   PSW_SERVICE_OPTION_4       = 4,
   PSW_SERVICE_OPTION_5       = 5,
   PSW_SERVICE_OPTION_6       = 6,
   PSW_SERVICE_OPTION_7       = 7,
   PSW_SERVICE_OPTION_9       = 9,
   PSW_SERVICE_OPTION_12      = 12,
   PSW_SERVICE_OPTION_13      = 13,
   PSW_SERVICE_OPTION_14      = 14,
   PSW_SERVICE_OPTION_17      = 17,
   PSW_SERVICE_OPTION_18      = 18,
   PSW_SERVICE_OPTION_19      = 19,
#ifdef SYS_OPTION_EVRCB
   PSW_SERVICE_OPTION_68      = 68,
#endif
   PSW_SERVICE_AMPS           = 0xFFFF,

   PSW_DEFAULT_SERVICE_OPTION = 3
} PswServiceOptionT;

typedef enum
{
   PSW_NT_UNKNOWN        = 0,
   PSW_NT_INTERNATIONAL,
   PSW_NT_NATIONAL,
   PSW_NT_NETWORK_SPECIFIC,
   PSW_NT_SUBSCRIBER,
   PSW_NT_RESERVED,
   PSW_NT_ABBREVIATED    = 6
} PswNumberTypeT;

typedef enum
{
   PSW_NP_UNKNOWN = 0,
   PSW_NP_ISDN_TELEPHONY,
   PSW_NP_RESERVED2,
   PSW_NP_DATA,
   PSW_NP_TELEX,
   PSW_NP_RESERVED5,
   PSW_NP_RESERVED6,
   PSW_NP_RESERVED7,
   PSW_NP_RESERVED8,
   PSW_NP_PRIVATE = 9
} PswNumberPlanT;

typedef enum  /* This section needs to be defined in an order as it appears in SCCGEN.H */
{
   PSW_UNKNOWN_SERVICE_TYPE=0,
   PSW_VOICE_SERVICE_TYPE,
   PSW_ANALOG_VOICE_SERVICE_TYPE,
   PSW_ASYNC_FAX_DATA_SERVICE_TYPE,
   PSW_ASYNC_DATA_SERVICE_TYPE,
   PSW_FAX_DATA_SERVICE_TYPE,
   PSW_UP_ASYNC_DATA_SERVICE_TYPE,
   PSW_PACKET_DATA_SERVICE_TYPE,
   PSW_QNC_PACKET_DATA_SERVICE_TYPE,
   PSW_UP_PACKET_DATA_SERVICE_TYPE,
   PSW_LPBK_SERVICE_TYPE,
   PSW_SMS_SERVICE_TYPE,
   PSW_OTASP_SERVICE_TYPE,
   PSW_TCP_BROWSER_DATA_SERVICE_TYPE,
   PSW_LOC_SERVICE_TYPE
} PswServiceT;

typedef enum
{
  PSW_PI_ALLOWED,
  PSW_PI_RESTRICTED,
  PSW_PI_NUMBER_NA,
  PSW_PI_RSVD
} PswPresentationIndicatorT;

typedef enum
{
  PSW_SI_USER_PROVIDED_NOT_SCREENED,
  PSW_SI_USER_PROVIDED_VERIFIED_PASSED,
  PSW_SI_USER_PROVIDED_VERIFIED_FAILED,
  PSW_SI_NETWORK_PROVIDED
} PswScreeningIndicatorT;

typedef PACKED struct
{
   PswNumberTypeT Type;
   PswNumberPlanT Plan;
   PswPresentationIndicatorT Pi;
   PswScreeningIndicatorT    Si;
   int8 Number[CP_MAX_CALLING_PARTY_NUMBER_SIZE + 1];  /* Inc NULL terminator */
} PswCallingPartyNumberT;

/* move these two for CBP3_MERGE */
typedef enum
{
  PSW_SUBADDRESS_TYPE_NSAP,
  PSW_SUBADDRESS_TYPE_USER_SPECIFIED = 2
} PswSubaddressTypeT;

typedef PACKED struct
{
  bool                Extension;
  PswSubaddressTypeT  Type;
  bool                Indicator;
  int8            Address[CP_MAX_SUBADDRESS_SIZE + 1];/*Inc NULL terminator*/
} PswSubaddressT;


typedef PACKED struct
{
   bool              Otasp;               /* OTASP originated call        */
   uint8             OtaspMode;           /* band or system type used     */
   bool              E911Call;            /* Call is emergency call       */
   PswRequestModeT   RequestMode;         /* requested call type          */
   PswServiceOptionT ServiceOption;       /* requested service option     */
   bool              PrivacyMode;         /* voice privacy indicator      */
   bool              Encoded;
   bool              DigitMode;           /* digit encoding indicator     */
   PswNumberTypeT    NumberType;          /* type of number               */
   PswNumberPlanT    NumberPlan;          /* numbering plan               */
   uint8             NumDigits;           /* number of digits to be dialled       */
   uint8             Digits[CP_MAX_CALLING_PARTY_NUMBER_SIZE +1];/* ascii digits, null terminated string*/
   PswCallingPartyNumberT CPNInfo;         /* Calling Party Number Info    */
   PswServiceT       ServiceType;          /* service types.               */
   bool              MultipleCallingPartyNum; /* MS has > 1 calling party num */
   bool              SendCallingPartySubaddr; /* subaddress included?     */
   PswSubaddressT    CallingPartySubaddr;
   bool              SendCalledPartySubaddr;  /* subaddress included?     */
   PswSubaddressT    CalledPartySubaddr;
} PswCallInitiateMsgT;

typedef PACKED struct
{
  bool enable;
  uint8 systype;
  BandClass band;
  uint16 channel;
}PswSetDualBandCalibrationMsgT;

typedef PACKED struct
{
   uint32 TimerId;                         /* Expired timer Id */
}PswTimerExpiredMsgT;

typedef PACKED struct
{
   uint32 Mux1Stats[PSW_NUM_MUX1_COUNTER_INDEX];
   uint32 Mux2Stats[PSW_NUM_MUX2_COUNTER_INDEX];
}PswRevStatParmsMsgT;

typedef PACKED struct
{
   bool  Encoded;
   uint8 Digit;
} PswSendContDTMFMsgT;

typedef PACKED struct
{
   bool  Encoded;
   uint8 NumDigits;
   uint8 DtmfOnLength;
   uint8 DtmfOffLength;
   uint8 Digits[ CP_MAX_BURST_DTMF_DIGITS + 1 ]; /* Incl NULL terminator */
} PswSendBurstDTMFMsgT;

typedef PACKED struct
{
   PswNumberTypeT Type;
   PswNumberPlanT Plan;
   int8           Number[CP_MAX_CALLED_PARTY_NUMBER_SIZE + 1];/*Inc NULL terminator*/
}  PswCalledPartyNumberT;


typedef PACKED struct
{
   bool   SendFeatureIndicator;
   uint8  Feature;
   bool   SendKeypadFacility;
   int8   KeypadFacility[CP_MAX_DISPLAY_SIZE  +  1];  /* Incl NULL terminator */
   bool   SendCalledPartyNumber;
   PswCalledPartyNumberT CalledPartyNumber;
   bool   SendCallingPartyNumber;
   PswCallingPartyNumberT CallingPartyNumber;
   bool   SendConnectedNumber;
   PswCallingPartyNumberT ConnectedNumber;   /* Same format as CallingPartyNumber */
   bool   Send;
   bool   SendCalledPartySubaddress;
   PswSubaddressT CalledPartySubaddress;
   bool   SendCallingPartySubaddress;
   PswSubaddressT CallingPartySubaddress;
   bool   SendConnectedSubaddress;
   PswSubaddressT ConnectedSubaddress;
} PswHookflashMsgT;

typedef PACKED struct
{
  bool   AckRequired;
  uint8  MsgNumber;
  uint8  BurstType;
  uint8  NumMsgs;
  uint8  NumFields;
  uint8  Data[CP_MAX_DATA_BURST_CHAR];
  bool   Encoded;
  bool   DigitMode;
  uint8  NumDigits;
  uint8   Digits[CP_MAX_BURST_DTMF_DIGITS];
} PswDataBurstMsgT;

typedef PACKED struct
{
   uint8 NumFrames;  /* Number of frames spent off the serving frequency */
} PswEndCFMeasureMsgT;

typedef PACKED struct
{
   uint8 P1;
   uint8 P2;
   uint8 P3;
   uint8 P4;
   uint8 P1Delay;
   uint8 P2Delay;
   uint8 P3Delay;
   uint8 P4Delay;
   uint8 P1Cycles;
   uint8 P2Cycles;
   uint8 P3Cycles;
} PswOutOfServiceAreaParamsMsgT;

/*AKK 07-15-02 QPCH/PI/CCI/ETS Thresholds mods*/
typedef PACKED struct
{
   int16 T11;
   int16 T12;
   int16 T13;
   int16 T14;
   int16 T15;
} PswQpchT1ThresholdsMsgT;

typedef PACKED struct
{
   int16 R11;
   int16 R12;
   int16 R13;
   int16 R14;
   int16 R15;
} PswQpchRThresholdsMsgT;

typedef PACKED struct
{
   int16 SNR11;
   int16 SNR12;
   int16 SNR13;
   int16 SNR14;
   int16 SNR15;
} PswQpchSNRThresholdsMsgT;
/*END--AKK 07-15-02 QPCH/PI/CCI/ETS Thresholds mods*/

typedef PACKED struct
{
   uint16   EnableBits;
} PswDsEnableMsgT;

typedef enum
{
   PSW_MDR_DISABLED = 0,
   PSW_MDR_IF_AVAIL,
   PSW_MDR_ONLY
} PswPktSvcPrefT;

typedef PACKED struct
{
   uint8           pageAcceptMap;
   uint16          ayncDataSOPref;
   uint16          faxSOPref;
   bool            dormantBySCC;
   uint16          ldrSOPrefer;
   uint16          mdrSOPrefer;
   PswPktSvcPrefT  pktSvcPref;
   uint8		   forRc; 
   uint8		   revRc; 
} PswDataPrefReqMsgT;

typedef enum
{
   PSW_QCELP1 = 0,
   PSW_QCELP2,
   PSW_EVRC,
   PSW_QCELP12,
   PSW_QCELP1_EVRC,
   PSW_QCELP2_EVRC
} PswVocoderMapT;

typedef enum
{
   PSW_PREFER_MUX1 = 1,
   PSW_PREFER_MUX2
} PswMuxPrefMapT;

typedef enum
{
   PSW_PREFER_DEFAULT,
   PSW_PREFER_ANALOG,
   PSW_PREFER_EVRC,
   PSW_PREFER_QCELP,

#ifdef SYS_OPTION_EVRCB
   PSW_PREFER_QCELP_MUX2,
   PSW_PREFER_EVRCB = 6,
#else
   PSW_PREFER_QCELP_MUX2,
#endif

   PSW_PREFER_FAX,
   PSW_PREFER_ASYNC_DATA
} PswVocoderPrefT;

typedef PACKED struct
{
   PswVocoderPrefT  VocoderPref;
   PswMuxPrefMapT   MuxOptPref;
   bool             ToNegotiate;
   uint8		    forRc; 
   uint8			revRc; 

} PswVsPrefReqMsgT;

typedef PACKED struct
{
   uint8            pageAccptMap;
   uint16           lpbkSO;
   uint16           markovSO;
   uint16           supplSO;
   bool             ToNegotiate;
   uint8		    forRc; 
   uint8			revRc; 

} PswLsPrefReqMsgT;

typedef PACKED struct
{
  uint16 ServiceOption;
  bool   ToNegotiate;
  uint8  forRc;
  uint8  revRc;
} PswSetServiceConfigMsgT;

/* SNR SR */
typedef PACKED struct
{
   bool  enable;
   uint8 numAttempts;         /* See MAX_SILENT_RETRIES */
   uint8 timeBetweenAttempts; /* See SILENT_RETRY_TIMEOUT */ 
   uint8 timeBeforeAbort;     /* See SILENT_RETRY_RESET_TIMEOUT */ 
} PswSetSilentRetryMsgT;


typedef PACKED struct
{
   uint32 CumtomFeature;
   /*each bit is defined for every cumtomfeafure */   
} PswCumtomFeatureMsgT;


typedef PACKED struct
{
  ExeRspMsgT  RspInfo;         /* Response routing information */
} PswGetSilentRetryStatusMsgT;

/* Get Active NAM response message */
typedef PACKED struct
{
  bool enable;
} PswGetSilentRetryStatusResponseMsgT;

/* Amala K. 01/10/02 - Emergency Mode Processing */
typedef PACKED struct
{
   bool     E911Mode;  /* TRUE=EN, FALSE=DIS */
} PswE911ModeMsgT;

typedef PACKED struct
{
   NamNumber   namNumber;
} PswGetPrlMsgT;

typedef enum
{
   PSW_PAM_DISABLE,
   PSW_PAM_FAX_NEXT_CALL,
   PSW_PAM_FAX_TILL_PWDN,
   PSW_PAM_DATA_NEXT_CALL,
   PSW_PAM_DATA_TILL_PWDN
} DsPreArangT;

typedef PACKED struct
{
  DsPreArangT paType;
} PswVsDsPreArangMsgT;

typedef PACKED struct
{
   uint8  reason;  
} PswPsCallRelMsgT;

typedef PACKED struct
{
   bool Enable[PSW_NUMBER_SO_DISABLE_LIST+1]; /* starts at SO 0 */
} PswServiceOptionDisableMsgT;

typedef PACKED struct
{
   uint8  ConRef;
   uint16 SrvOpt;
   uint8  Length;
   uint8  CtrlRec[1];
} PswServiceOptionControlRespMsgT;

typedef PACKED struct
{
   uint8  RemainDuration;       /* Remaining RSCH duration as assigned by BS */
} PswRschRelIndMsgT;

/* RLP Message Definitions */
typedef struct
{
   uint8        SrId;
   uint16       DataSize;
} PswDataBufferStatusMsgT;

typedef PACKED struct
{
   uint8            SoPref;
   uint8		    forRc; 
   uint8			revRc; 

} PswSmsPrefReqMsgT;

typedef PACKED struct
{
  uint8  DataByte[PSW_MAX_SMS_SIZE];   /* contents of SMS.     */
  uint16 len;        /* the length of data in DataByte[ ]      */
  bool   confirm;    /* confirm-required indocator             */
  uint8  Digits[CP_MAX_CALLING_PARTY_NUMBER_SIZE +1]; /* ascii
                                 digits, null terminated string*/
} PswSmsTxReqMsgT;

typedef PACKED struct
{
  uint32 SvcMask;
  uint32 LanguageMask;
  uint8  Priority;
} PswSmsParmBrcstMsgT;

typedef PACKED struct
{
   bool         Success;
} PswOtaspRcvdMsgT;

typedef PACKED struct
{
   uint16       Length;
   uint8*       Data;
} PswOtaspTxReqMsgT;

typedef PACKED struct
{
   uint16       SoPref;
   uint8		forRc; 
   uint8		revRc; 

} PswOtaspSoPrefMsgT;

typedef PACKED struct
{
   uint16       NewSo;
} PswVsSwitchSoMsgT;

typedef PACKED struct
{
   NamNumber   namNumber;
} PswGetNamMsgT;

typedef PACKED struct
{
   NamNumber   namNumber;
   uint32      whatChangedInNam;
} PswNamChangedByEtsMsgT;

typedef PACKED struct
{
   NamNumber   namNumber;
   bool 	   ScmIncl;
   uint8	   Scm[NUM_BANDS_SUPPORTED];
   bool 	   SlotCycleIncl;
   uint8	   SlotCycleIndex;
   bool		   LckRsnIncl;
   uint8	   LckRsn;
   bool		   MaintRsnIncl;
   uint8	   MaintRsn;
   bool 	   prefServiceIncl;
   uint8 	   prefBand;
   uint8 	   prefMode;
   uint8 	   prefBlockBand1;
   uint8       prefServBand0;
   bool		   AuthParmsIncl;
   uint32	   AKey[2];
   bool	  	   ImsiMIncl;
   bool		   ImsiMValid;
   IMSIType    IMSI_M;
   bool		   SidIncl;
   uint16	   Sid;
   bool		   NidIncl;
   uint16	   Nid;
   bool		   AccolcIncl;
   uint8	   Accolc;
   bool		   TermInfoIncl;
   uint8	   MobTermHome;
   uint8	   MobTermSid;
   uint8	   MobTermNid;
   bool		   SpcIncl;
   uint32	   Spc;
   bool		   MdnIncl;
   uint8	   MdnNumDigits;
   uint8	   Mdn[16];
   bool		   AllowOtapaIncl;
   bool		   AllowOtapa;
   bool		   AnalogParmIncl;
   uint16	   HomeSid;
   uint16	   Sids;
   uint16	   LocaIds;
   bool	 	   Puregs;
   bool		   MobileIPIncl;
   uint32	   MN_HomeIPAddr;
   uint32	   HA_PriIPAddr;
   uint32	   HA_SecIPAddr;
   uint8	      MN_Nai[MN_NAI_MAX_SIZE];
   uint8        MN_AaaPassword[MN_PASSWD_MAX_SIZE];
   uint8        MN_HaPassword[MN_PASSWD_MAX_SIZE];
   uint8        MN_RevTunneling;
   uint32       MN_HaSpi;
   uint32       MN_AaaSpi;
} PswNamChangedMsgT;

typedef PACKED struct
{
	uint8 msResult[64];
	UINT8 NumBytes;
} PswModExpMSResultMsgT;

typedef PACKED struct
{
	uint8 otaK[8];
} PswModExpBSResultMsgT;

typedef PACKED struct
{
    bool SSPRListFillOK;
} PswFillSSPRRspMsgT;

typedef PACKED struct
{
	UINT8 *IS683Msg;
	UINT16 Length;
} PswIOTAIS683MsgT;

typedef PACKED struct
{
	UINT8 *IOTAPrl;
	UINT16 Length;
} PswIOTAPRLMsgT;


#define SYS_OPTION_SMS_ENGINE
/* from MMI - SMS related */
#ifdef SYS_OPTION_SMS_ENGINE

typedef PACKED struct
{
   PswServiceOptionT SrvOpt;
} PswSmsPrefSrvOptMsgT;

typedef PACKED struct
{
   bool  DigitMode;
   bool  NumberMode;
   uint8 NumberType;
   uint8 NumberPlan;
   uint8 NumFields;
   uint8 Char[ SMS_MAX_ADDRESS_CHARS ];
} PswSmsMCAddrT;

typedef PACKED struct
{
   uint8 Type;
   bool  Odd;
   uint8 NumFields;
   uint8 Char[ SMS_MAX_SUBADDRESS_CHARS ];
} PswSmsMCSubaddrT;

typedef PACKED struct
{
   uint16 Category;
   uint8  CategoryResult;
} PswCategoryT;

typedef PACKED struct
{
  uint8 NumCategories;
  PswCategoryT Cat[ SMS_MAX_NUM_SERVICE_CATEGORIES ];
} PswSrvCatProgResultT;

typedef PACKED struct
{
   uint16 TeleSrvId;                       /* Teleservice ID                  */
   PswSmsMCAddrT DefaultDestAddr;          /* Message center address of all   */
                                           /* submitted messages              */
   bool DefaultSubaddrPres;                /* Indicator if subaddress present */
   PswSmsMCSubaddrT DefaultSubaddr;        /* Message center subaddress       */
} PswSmsConnectMsgT;

typedef PACKED struct
{
   uint16 TeleSrvId;                       /* Teleservice ID                  */
} PswSmsDisconnectMsgT;

/*
** PSW_GET_STAT_CNT_MSG,
*/
typedef PACKED struct
{
   StatCounterT   StatCounter;         /* Statistics counter */
   uint8          NumElement;          /* Number of elements in StatCounter   */
   uint32         *MuxStat;
} PswStatCntMsgT;

#define PSW_SMS_MAX_USER_DATA_CHARS 256

typedef PACKED struct
{
   uint8 Year;
   uint8 Month;
   uint8 Day;
   uint8 Hours;
   uint8 Minutes;
   uint8 Seconds;
} PswSmsAbsTimeT;

typedef enum
{
   PSW_CAT_UNKNOWN,
   PSW_CAT_EMERGENCIES,
   PSW_CAT_ADMINISTRATIVE,
   PSW_CAT_MAINTENANCE,
   PSW_CAT_GEN_NEWS_LOCAL,
   PSW_CAT_GEN_NEWS_REGIONAL,
   PSW_CAT_GEN_NEWS_NATIONAL,
   PSW_CAT_GEN_NEWS_INTERNATIONAL,
   PSW_CAT_FINANCE_NEWS_LOCAL,
   PSW_CAT_FINANCE_NEWS_REGIONAL,
   PSW_CAT_FINANCE_NEWS_NATIONAL,
   PSW_CAT_FINANCE_NEWS_INTERNATIONAL,
   PSW_CAT_SPORTS_NEWS_LOCAL,
   PSW_CAT_SPORTS_NEWS_REGIONAL,
   PSW_CAT_SPORTS_NEWS_NATIONAL,
   PSW_CAT_SPORTS_NEWS_INTERNATIONAL,
   PSW_CAT_ENTERTAINMENT_NEWS_LOCAL,
   PSW_CAT_ENTERTAINMENT_NEWS_REGIONAL,
   PSW_CAT_ENTERTAINMENT_NEWS_NATIONAL,
   PSW_CAT_ENTERTAINMENT_NEWS_INTERNATIONAL,
   PSW_CAT_LOCAL_WEATHER,
   PSW_CAT_TRAFFIC,
   PSW_CAT_FLIGHT_SCHEDULE,
   PSW_CAT_RESTAURANTS,
   PSW_CAT_LODGINGS,
   PSW_CAT_RETAIL_DIRECTORY,
   PSW_CAT_ADVERTISEMENT,
   PSW_CAT_STOCK_QUOTES,
   PSW_CAT_EMPLOYMENT_OPPORTUNITIES,
   PSW_CAT_MEDICAL_HEALTH,
   PSW_CAT_TECH_NEWS,
   PSW_CAT_MULTI_CATEGORY
} PSW_SMS_SERVICE_CAT;

typedef enum
{
   PSW_NORMAL,
   PSW_INTERACTIVE,
   PSW_URGENT,
   PSW_EMERGENCY
} PSW_SMS_PRIORITY;

typedef enum
{
   PSW_SMS_NOT_RESTRICTED,
   PSW_SMS_RESTRICTED,
   PSW_SMS_CONFIDENTIAL,
   PSW_SMS_SECRET
} PSW_SMS_PRIVACY;

typedef enum
{
   PSW_SMS_MOBILE_DEFAULT,
   PSW_SMS_LOW_PRIORITY,
   PSW_SMS_MEDIUM_PRIORITY,
   PSW_SMS_HIGH_PRIORITY
} PSW_SMS_ALERT;

typedef enum
{
   PSW_SMS_UNKNOWN,
   PSW_SMS_ENGLISH,
   PSW_SMS_FRENCH,
   PSW_SMS_SPANISH,
   PSW_SMS_JAPANESE,
   PSW_SMS_KOREAN,
   PSW_SMS_CHINESE,
   PSW_SMS_HEBREW
} PSW_SMS_LANG;

typedef PACKED struct
{
  uint8 ErrorClass;
  uint8 MsgStatusCode;
} PSW_MSG_STATUS;

typedef enum
{
   PSW_SMS_DELIVERY_ACK = 4,
   PSW_SMS_USER_ACK,
   PSW_SMS_READ_ACK
} PSW_MSG_TYPE;

typedef PACKED struct
{
   uint16 TeleSrvId;                       /* Teleservice ID                  */
   uint16 SeqNum;                           /* Unique number to coordinate the */
                                           /* parameter and user data messages*/
   bool SendOnTraffic;                     /* Force SMS on Traffic or let CP  */
                                           /* decide                          */

   bool SrvCatPres;                        /* If service catagory is present  */
   PSW_SMS_SERVICE_CAT SrvCat;             /* Service Catagory                */
   uint16 MsgId;                           /* Message ID of submitted message */
   bool HeaderInd;                         /* Header Indicator                */
   uint8 NumUserDataMsgs;                  /* Number of User data messages    */
   bool AbsValPres;                        /* Absolute validity period present*/
   PswSmsAbsTimeT AbsVal;                  /* Absolute validity period        */
   bool RelValPres;                        /* Relative validity period present*/
   uint8 RelVal;                           /* Relative validity period        */
   bool AbsDelPres;                        /* Absolute deferred delivery time */
                                           /* present                         */
   PswSmsAbsTimeT AbsDel;                  /* Absolute deferred delivery time */
   bool RelDelPres;                        /* Relative deferred delivery time */
                                           /* present                         */
   uint8 RelDel;                           /* Relative deferred delivery time */
   bool PriPres;                           /* Priority indicator present      */
   PSW_SMS_PRIORITY Pri;                   /* Priority indicator              */
   bool PrvPres;                           /* Privacy indicator present       */
   PSW_SMS_PRIVACY Prv;                    /* Privacy indicator               */
   bool ReplyOptPres;                      /* If reply option is present      */
   bool UserAck;                           /* User acknowledgement requested  */
   bool DeliverAck;                        /* delivery acknowledgement        */
                                           /* requested from message center   */
   bool ReadAck;                           /* read acknowledgement            */
   bool AlertPres;                         /* Alert on message delivery present*/
   PSW_SMS_ALERT Alert;                    /* Alert on message delivery       */
   bool LangPres;                          /* Language indicator present      */
   PSW_SMS_LANG Lang;                      /* Language indicator              */
   bool CallbackNumPres;                   /* Callback number present         */
   PswSmsMCAddrT CallbackNum;              /* Callback number                 */
   bool OverrideAddrPres;                  /* Override Address Present        */
   PswSmsMCAddrT OverrideAddr;             /* Override Address                */
   bool OverrideSubaddrPres;               /* Override Subaddress Present     */
   PswSmsMCSubaddrT OverrideSubaddr;       /* Override Subaddress             */
   bool MsgDepIndexPres;                   /* Message Deposit Index present   */
   uint16 MsgDepIndex;                     /* Message Deposit Index           */
   bool SrvCatProgResultPres;              /* Service Category Program Results*/
   PswSrvCatProgResultT SrvCatProgResult;  /* Service Category Program Results*/
} PswSmsSubmitParmsMsgT;

typedef PACKED struct
{
   uint16 SeqNum;
   uint8  MsgType;
   uint8  NumMsgs;
   uint8  MsgEncoding;
   uint8  NumFields;
   uint8  Char[ SMS_MAX_USERDATA_LENGTH ]; /* User Data field of message      */
} PswSmsUserDataMsgT;

typedef PACKED struct
{
   uint16 TeleSrvId;                       /* Teleservice ID                  */
   uint16 MsgId;                           /* Message ID of submitted message */
   bool   HeaderInd;                       /* Header Indicator                */
   bool OverrideAddrPres;                  /* Override address present        */
   PswSmsMCAddrT OverrideAddr;             /* Override address                */
   bool SubaddrPres;                       /* Subaddress Present              */
   PswSmsMCSubaddrT Subaddr;               /* Subaddress                      */
} PswSmsCancelMsgT;

typedef PACKED struct
{
   uint16 TeleSrvId;                       /* Teleservice ID                  */
   uint16 SeqNum;                          /* Unique number to cordinate the  */
                                           /* parameters and user data messages*/
   PswSmsMCAddrT Addr;                     /* Message ceneter address         */
   bool SubaddrPres;                       /* Subaddress Present              */
   PswSmsMCSubaddrT Subaddr;               /* Subaddress                      */
   uint16 MsgId;                           /* Message ID of submitted message */
   PSW_MSG_TYPE MsgType;                   /* User/Delivery/Read ack type     */
   bool HeaderInd;                         /* Header Indicator                */
   uint8 NumUserDataMsgs;                  /* Number of User data messages    */
   bool RespCodePres;                      /* User response code present      */
   uint8 RespCode;                         /* User response code              */
	bool TimeStampPres;                     /* Time Stamp present              */
	PswSmsAbsTimeT	TimeStamp;               /* Message Center Time Stamp       */
   bool MsgDepIndexPres;                   /* Message Deposit Index present   */
   uint16 MsgDepIndex;                     /* Message Deposit Index           */
	bool MsgStatusPres;                     /* Message Status present          */
	PSW_MSG_STATUS MsgStatus;               /* Message Status                  */
} PswSmsAckMsgT;

typedef PACKED struct
{
   uint32 SvcMask;
   uint32 LangMask;
   PSW_SMS_PRIORITY Priority;
} PswSmsBCastParmsMsgT;

typedef enum
{
   PSW_NO_TERMINAL_PROBLEMS,
   PSW_DESTINATION_RESOURSE_SHORTAGE,
   PSW_DESTINATION_OUT_OF_SERVICE
}PswTermStatusT;

typedef PACKED struct
{
   PswTermStatusT Status;
} PswSmsTerminalStatusMsgT;

typedef PACKED struct
{
   uint8 SmsTlMaxRetry;
} PswSmsRetxAmountMsgT;

/* SCC to SMS messages - loose coupling */
typedef PACKED struct
{
   bool                TxStatus;
} PswSmsTxRspMsgT;

typedef PACKED struct
{
   uint8 ErrorClass;
   uint8 CasueCode;
} PswSmsCauseCodeMsgT;

typedef PACKED struct
{
   uint16 SeqNum;
   uint8 ErrorClass;
   uint8 CauseCode;
   bool SendTlAck;
} PswSmsReplyTlAckMsgT;
#endif

/*-------------------------------------------------------------------**
  PSW_POS_AFLT_MEASUREMENTS_MSG
  PswPosAfltMeasurementMsgT
  L1D reports various position AFLT pilot set list information needed
  by BS. This list is not necessarily a complete list of position AFLT
  pilot set list but only those measurements that are completed as of
  the time the message is generated.
 *-------------------------------------------------------------------**/

typedef PACKED struct
{
   uint16  PilotPN;
   bool    Stale;
   /* PILOT_PN_PHASE sent in PROVIDE PILOT PHASE MEASUREMENTS */
   int32   PnPhase;
   /* PILOT_STRENGTH sent in PROVIDE PILOT PHASE MEASUREMENTS */
   uint16  Strength;
   /* RMS_ERR_PHASE sent in PROVIDE PILOT PHASE MEASUREMENTS */
} AFLTPilotMeasurementT;

typedef PACKED struct
{
   uint8 SeqNum;
   /* TIME_REF_MS sent in PROVIDE PILOT PHASE MEASUREMENTS */
   uint16 TimeRefMs;
   bool OffsetIncl;
   int16 MobOffset;
   /* REF_PN sent in PROVIDE PILOT PHASE MEASUREMENTS */
   uint16 RefPN;
   /* REF_PILOT_STRENGTH sent in PROVIDE PILOT PHASE MEASUREMENTS */
   uint16 RefPilotStrength;
   /* NUM_PILOTS_P sent in PROVIDE PILOT PHASE MEASUREMENTS */
   /* actual number of pilot measurements in AFLTPilotMeasurement */
   uint8  NumPilots;
   AFLTPilotMeasurementT PosAFLT[SYS_MAX_AFLT_LIST_PILOTS];
} PswPosAfltMeasurementMsgT;

/*-------------------------------------------------------------------**
  PSW_POS_AFLT_ACK_ABORT_MSG
  PswPosAfltAckAbortMsgT
  L1D, after receiving L1D_POS_AFLT_ABORT_MSG, acknowledges its
  position search measurements aborted for given sequence number.
 *-------------------------------------------------------------------**/

typedef PACKED struct
{
  uint16 SeqNum;
} PswPosAfltAckAbortMsgT;

/*-------------------------------------------------------------------**
  PSW_POS_AFLT_ABORT_REQ_MSG
  PswPosAfltAbortReqMsgT
  L1D, requests for position search measurements aborted for given
  sequence number.
 *-------------------------------------------------------------------**/

typedef PACKED struct
{
  uint16 SeqNum;
} PswPosAfltAbortReqMsgT;

/* mclee location services request element messages */
typedef struct
{
	uint8 SessID;
	uint8 RejReqType; /* BS Request Element type */
	bool RejReason;
} PswPosRejectMsgT;

typedef PACKED struct
{
	uint8 RecLen;
	uint8 ProvPseudorangeRec[PRM_MAX_SIZE];
} PswPseudorangeMsmtRspMsgT;

typedef PACKED struct
{
   uint8            SoPref;
   uint8		    forRc; 
   uint8			revRc; 

} PswLocPrefReqMsgT;

typedef PACKED struct
{
   bool 	Enable;
   uint8	N1;
} PswGPSEnableReportsMsgT;

typedef PACKED struct
{
   uint16 Size;
   uint8  *DataP;
} PswIs801TcpbFwdDataRespInfoMsgT;
/* end mclee location services */

/* Amala - GPS 08/07/02 */
#ifdef PDDM_SIMULATION_IN_USE
typedef PACKED struct
{
   uint8 	length;
   uint8	Data[20];
} PswTestLocPDDMMsgT;
#endif /* PDDM_SIMULATION_IN_USE */

typedef PACKED struct
{
  SysSystemTimeT  SendTime;
} PswL1dAccessPreambleCompleteMsgT;

typedef PACKED struct
{
  SysSystemTimeT  finishTime;
} PswL1dAccessProbeCompleteMsgT;

/****************/
/* ETS Messages */
/****************/

typedef PACKED struct
{
   PswTestModeT Mode;                      /* Test mode */
   uint16       Enabled;                   /* Mode Qualifier, typically equivalent to ON/OFF */
   /*AKK 06-06-02 QPCH/PI/CCI/ETS mods*/
   uint16       Value;                     /* Mode value*/
} PswEngineTestModeMsgT;



 typedef enum
  {
    TM_CRC                       = 0,     /* Enable/Disable CRC Checking */
    TM_REMAIN_IN_SYNC_STATE      = 1,     /* force MS to stay on Sync Channel */
    TM_ENABLE_IDLE_HANDOFF       = 2,     /* Enable Idle Handoff */
    /*AKK 03-06-01 swap the following items for the
     Armulator's Enable Message Injection functionality*/
    TM_ENABLE_MSG_INJECTION      = 3,     /* Message Injection Test Mode */
    TM_ENABLE_SLOTTED_MODE       = 4,     /* Enable Slotted Mode */
    TM_ENABLE_REGISTRATION       = 5,     /* enable/disable registration */
    TM_SET_QPCH_C_THRESHOLD      = 15,
    TM_SET_QPCH_DELTA_THRESHOLD  = 16,
    TM_ENABLE_QPCH_MODE          = 17,
	 TM_DISABLE_ACCESS_AUTH_PARMS = 18,	   /* Don't include auth parameters in access msgs */
	 TM_DISPLAY_LOC_DATA          = 19,		/* dump Location Services Data */
    TM_AMPS_ENABLE_SLOTTED_MODE  = 20,    /* Control AMPS Slotted mode operation */
    TM_IGNORE_OTA_AKEY_PREV      = 21,    /* Ignore A_KEY_PREV in MS Key Req Msg */
    TM_QPCH_SET_PI1              = 22,    /* Set Paging Indicator 1 override */
    TM_QPCH_SET_PI2              =23,       /* Set Paging Indicator 2 override */
    TM_SLOTTED_DELAY             = 26     /* Used to delay before going to sleep in slotted paging */
  } TestModeMsgT;

typedef PACKED struct
{
   bool TimersUsed;                        /* Timers turned (on = TRUE) or (off = FALSE) */
} PswTestModeTimersUsedMsgT;

typedef PACKED struct
{
    uint32            MaxCapSize;          /* System time to start sending message */
    uint16            power_correction;    /* Tx power level less mean input power */
    bool              use_curr_mip;        /* Use current mean input power or not  */
    uint8             data_len;            /* Length of data to be sent            */
    uint8             data[30];            /* l3 message data, length, data, crc   */
    bool              repeat_data;         /* Message data is repeated flag        */
    uint8             lc_mask[6];          /* Access channel long code mask        */
    uint8             pam_size;            /* Preamble size in frames              */
    uint16            RN;                  /* Random chip delay : 0 - 511          */
} PswAccessSigMsgT;

/* Status request message - Request current PS status */
typedef PACKED struct
{
   ExeRspMsgT  RspInfo;                    /* Response routing information */
} PswStatusRequestMsgT;

/* Status request, response */
typedef PACKED struct
{
   uint32 	Esn;
   uint8	RFMode;
   uint8    MIN_Digits;
   uint8    Min[15];
   uint8    CPState;
   uint8    CDMAGoodFrames;
   uint16	AMPSGoodFrames;
   uint16	AMPSBadFrames;
   uint8 	CPEntranceReason;
   uint16 	Channel;
   uint16   PilotPN;
   uint16 	CurSID;
   uint16	CurNID;
   uint16	LocAreaID;
// deleted by xipeng
//   int16	RSSI;
   uint8  	PowerLevel;
   bool     Slotted_Mode_Flag;
} PswStatusResponseMsgT;

/* Debug Screen request message - Request debug screen info */
typedef PACKED struct
{
   ExeRspMsgT  RspInfo;                    /* Response routing information */
} PswDebugScrnRequestMsgT;

/* Debug Screen Response */
typedef struct
{
  uint16          sid;
  uint16          nid;
  uint16          badframes;
  uint16          totframes;
  uint16          pilotPN   [SYS_CP_MAX_ACTIVE_LIST_PILOTS];
  uint16          pilotEcIo [SYS_CP_MAX_ACTIVE_LIST_PILOTS];
  int16           txPower;
  int16           rxPower;
} PswDebugScrnResponseT;

/* Exit Emergency Mode Request Message */
typedef struct
{
  bool   value;
} PswExitEmergencyModeMsgT;

/* A. Kulkarni. 02/13/02. */
/* Phone State request message */
typedef PACKED struct
{
   ExeRspMsgT  RspInfo;                    /* Response routing information */
} PswPhoneStateRequestMsgT;

/* Phone State request, response */
typedef PACKED struct
{
   uint8  State;
   uint16 event_counter;
} PswPhoneStateResponseMsgT;

/* Set Retreivable Statistics Parameter Request Type */
typedef PACKED struct
{
    uint16  ParameterID;
/* Hans C. Len is not needed in currect implementation:   uint16 ParameterLen;
 */
   uint32 Value;
} PswSetRetrievableParameterMsgT;

/* Get Retreivable Statistics Parameter Request Type */
typedef PACKED struct
{
   ExeRspMsgT     RspInfo;
   StatCounterT   StatCounter;         /* Statistics counter */
} PswGetRetrievableParameterMsgT;


/* Get Retreivable Statistics Parameter Response Message Type */
typedef PACKED struct
{
    StatCounterT  StatCounter;         /* Statistics counter */
    UINT32        Value[MAX_STAT_ELEMENT];
} GetRetrievableParameterRspMsgT;


typedef PACKED struct
{
   ExeRspMsgT  RspInfo;                    /* Response routing information */
} PswGetFullSysTimeMsgT;

typedef PACKED struct
{
   ExeRspMsgT  RspInfo;                    /* Response routing information */
} PswGetPRevInUseRequestMsgT;

typedef PACKED struct
{
   uint8  P_Rev_In_Use;
} PswGetPRevInUseResponseMsgT;
/* */

/* aak_akey */
#ifdef PSW_AKEY_UINT64
typedef PACKED struct
{
  ExeRspMsgT  RspInfo;                    /* Response routing information */
  uint64      akey;
  uint32      checksum;
  uint32      esn;
} PswEnterAKeyMsgT;
#else
typedef PACKED struct
{
  ExeRspMsgT  RspInfo;                    /* Response routing information */
  uint32      akey_low;
  uint32      akey_high;
  uint32      checksum;
  uint32      esn;
} PswEnterAKeyMsgT;
#endif

typedef PACKED struct
{
  bool result;
} PswEnterAKeyRspMsgT;

typedef PACKED struct
{
   uint8 TMax;
   uint8 T2;
   uint8 T3;
   uint8 HTMax;
   uint8 TCh1;
   uint8 TCh2;
   uint8 HTMin;
} PswSetIHOThrsholdsMsgT;

typedef PACKED struct
{
   uint16 HTMR;
   uint8  Count;
} PswSetIHOTimerMsgT;

typedef PACKED struct
{
   bool  DoImmediateAccessHo;
   uint8 NoiseFloor;
   uint8 ThrMinDelayed;
   uint8 MinActDelayed;
   uint8 ThrSlopeDelayed;
   uint8 DelayedAcHoTimer;
} PswSetAccessHOParmsMsgT;

typedef PACKED struct
{
   uint8 ThrMinImmed;
   uint8 MinActImmed;
   uint8 ThrSlopeImmed;
   uint8 ThrMinDelayed;
   uint8 MinActDelayed;
   uint8 ThrSlopeDelayed;
   uint8 MinNghbrThr;
} PswSetNewIHOparametersMsgT;

typedef PACKED struct
{
   uint16 EcIo;
} PswSyncPilotStrengthMsgT;

/*AKK 09-11-01 QPCH mods*/
typedef PACKED struct
{
   bool PiStatus;
} PswQpchPiStatusMsgT;

typedef PACKED struct
{
   uint8   nomPwrExt;
   int8    nomPwr;
   uint8   numPreamble;
   uint8   bandClass;
   uint16  cdmaFreq;
   uint16  pilotPN;
   uint8   codeChanFCH;
} PswInjectHandoffMsgT;

typedef PACKED struct
{
   uint8   NumDigits;           /* number of digits to be dialled       */
   uint8   Digits[CP_MAX_CALLING_PARTY_NUMBER_SIZE +1];/* ascii digits, null terminated string*/
   uint16  InactTimeOut;        /* in sec, 0 = infinity, at least 20 */
} PswPsCallSetupMsgT;

typedef PACKED struct
{ 
   bool              Secondary;     /* FALSE, if primary.               */
   bool              Successful;    
} PswRlpOpenRspMsgT;

typedef PACKED struct
{ 
   bool              Secondary;     /* FALSE, if primary.               */
} PswRlpCloseRspMsgT;

typedef PACKED struct
{ 
   bool              Secondary;     /* FALSE, if primary.               */
} PswRlpIdleIndMsgT;
/*****************************************************************************
 * Defines
 ****************************************************************************/
typedef enum
{
  PSW_UI_ENCRYP_DISABLED,
  PSW_UI_ORYX_ENABLED
} PSW_UI_ENCRYPT_MODE_TYPE;

typedef enum
{
   PSW_BASIC_ENCRYPT_SUPP,
   PSW_BASIC_AND_ENHANCED_SUPP
} PSW_ENCRYPT_MODE_TYPE;

/* define set of frame sizes */
typedef enum
{
   PSW_DCCH_FS_20MS = 1,          /* 20 ms frame size only         */
   PSW_DCCH_FS_5MS = 2,           /* 5 ms frame size only          */
   PSW_DCCH_FS_5MS_AND_20MS = 3   /* both 5 and 20 ms frame size   */
} PSW_DCCH_FRAME_SIZE_TYPE;

typedef enum
{
   PSW_FCH_FS_20MS = 0,
   PSW_FCH_FS_5MS_AND_20MS = 1
} PSW_FCH_FRAME_SIZE_TYPE;

/* define set of RC settings */
typedef enum
{
   PSW_FOR_RC_1 = 0x100,
   PSW_FOR_RC_2 = 0x80,
   PSW_FOR_RC_3 = 0x40,
   PSW_FOR_RC_4 = 0x20,
   PSW_FOR_RC_5 = 0x10,
   PSW_FOR_RC_6 = 0x8,
   PSW_FOR_RC_7 = 0x4,
   PSW_FOR_RC_8 = 0x2,
   PSW_FOR_RC_9 = 0x1
} PSW_FOR_RC_TYPE;

typedef enum
{
   PSW_REV_RC_1 = 0x20,
   PSW_REV_RC_2 = 0x10,
   PSW_REV_RC_3 = 0x8,
   PSW_REV_RC_4 = 0x4,
   PSW_REV_RC_5 = 0x2,
   PSW_REV_RC_6 = 0x1
} PSW_REV_RC_TYPE;

/* define set of RC Preference settings */
typedef enum
{
   PSW_RC_PREF_1 = 1,
   PSW_RC_PREF_2 = 2,
   PSW_RC_PREF_3 = 3,
   PSW_RC_PREF_4 = 4,
   PSW_RC_PREF_5 = 5,
   PSW_RC_PREF_6 = 6,
   PSW_RC_PREF_7 = 7,
   PSW_RC_PREF_8 = 8,
   PSW_RC_PREF_9 = 9
} PSW_RC_PREF_TYPE;

/* define set of Geo-location support */
typedef enum
{
   PSW_GL_NO_SUPPORT = 0,
   PSW_GL_AFLT = 1,
   PSW_GL_AFLT_AND_GPS = 2,
   PSW_GL_GPS = 3
} PSW_GEO_LOC_TYPE;

typedef enum
{
   PSW_GATING_RATE_SET_00  =  0x00,     /* Gating rates 1                */
   PSW_GATING_RATE_SET_01  =  0x01,     /* Gating rates 1 and 1/2        */
   PSW_GATING_RATE_SET_10  =  0x02,     /* Gating rates 1, 1/2 and 1/4   */
   PSW_GATING_RATE_SET_11  =  0x03      /* reserved                      */
} PSW_GATING_RATE_SET_TYPE;


/* FOR SCH Multiplex options --------------------------------------- */
typedef enum
{
   PSW_FOR_SCH_0x03_MUX_OPTION  = 0x0001,   /* For SCH 0x03 Mux options,  bit,   */
   PSW_FOR_SCH_0x809_MUX_OPTION = 0x0002,   /* For SCH 0x809 Mux options, bit,   */
   PSW_FOR_SCH_0x811_MUX_OPTION = 0x0004,   /* For SCH 0x811 Mux options, bit,   */
   PSW_FOR_SCH_0x821_MUX_OPTION = 0x0008,   /* For SCH 0x821 Mux options, bit,   */
   PSW_FOR_SCH_0x905_MUX_OPTION = 0x0010,   /* For SCH 0x905 Mux options, bit,   */
   PSW_FOR_SCH_0x909_MUX_OPTION = 0x0020,   /* For SCH 0x909 Mux options, bit,   */
   PSW_FOR_SCH_0x911_MUX_OPTION = 0x0040,   /* For SCH 0x911 Mux options, bit,   */
   PSW_FOR_SCH_0x921_MUX_OPTION = 0x0080,   /* For SCH 0x921 Mux options, bit,   */

   PSW_FOR_SCH_0x04_MUX_OPTION  = 0x0100,   /* For SCH 0x04 Mux options,  bit,   */
   PSW_FOR_SCH_0x80a_MUX_OPTION = 0x0200,   /* For SCH 0x80a Mux options, bit,   */
   PSW_FOR_SCH_0x812_MUX_OPTION = 0x0400,   /* For SCH 0x812 Mux options, bit,   */
   PSW_FOR_SCH_0x822_MUX_OPTION = 0x0800,   /* For SCH 0x822 Mux options, bit,   */
   PSW_FOR_SCH_0x906_MUX_OPTION = 0x1000,   /* For SCH 0x906 Mux options, bit,   */
   PSW_FOR_SCH_0x90a_MUX_OPTION = 0x2000,   /* For SCH 0x90a Mux options, bit,   */
   PSW_FOR_SCH_0x912_MUX_OPTION = 0x4000,   /* For SCH 0x912 Mux options, bit,   */
   PSW_FOR_SCH_0x922_MUX_OPTION = 0x8000    /* For SCH 0x922 Mux options, bit,   */
} PSW_FOR_SCH_MUX_OPTION;


/* REV SCH Multiplex options --------------------------------------- */
typedef enum
{
   PSW_REV_SCH_0x03_MUX_OPTION  = 0x0001,   /* Rev SCH 0x03 Mux options,  bit,   */
   PSW_REV_SCH_0x809_MUX_OPTION = 0x0002,   /* Rev SCH 0x809 Mux options, bit,   */
   PSW_REV_SCH_0x811_MUX_OPTION = 0x0004,   /* Rev SCH 0x811 Mux options, bit,   */
   PSW_REV_SCH_0x821_MUX_OPTION = 0x0008,   /* Rev SCH 0x821 Mux options, bit,   */
   PSW_REV_SCH_0x905_MUX_OPTION = 0x0010,   /* Rev SCH 0x905 Mux options, bit,   */
   PSW_REV_SCH_0x909_MUX_OPTION = 0x0020,   /* Rev SCH 0x909 Mux options, bit,   */
   PSW_REV_SCH_0x911_MUX_OPTION = 0x0040,   /* Rev SCH 0x911 Mux options, bit,   */
   PSW_REV_SCH_0x921_MUX_OPTION = 0x0080,   /* Rev SCH 0x921 Mux options, bit,   */

   PSW_REV_SCH_0x04_MUX_OPTION  = 0x0100,   /* Rev SCH 0x04 Mux options,  bit,   */
   PSW_REV_SCH_0x80a_MUX_OPTION = 0x0200,   /* Rev SCH 0x80a Mux options, bit,   */
   PSW_REV_SCH_0x812_MUX_OPTION = 0x0400,   /* Rev SCH 0x812 Mux options, bit,   */
   PSW_REV_SCH_0x822_MUX_OPTION = 0x0800,   /* Rev SCH 0x822 Mux options, bit,   */
   PSW_REV_SCH_0x906_MUX_OPTION = 0x1000,   /* Rev SCH 0x906 Mux options, bit,   */
   PSW_REV_SCH_0x90a_MUX_OPTION = 0x2000,   /* Rev SCH 0x90a Mux options, bit,   */
   PSW_REV_SCH_0x912_MUX_OPTION = 0x4000,   /* Rev SCH 0x912 Mux options, bit,   */
   PSW_REV_SCH_0x922_MUX_OPTION = 0x8000    /* Rev SCH 0x922 Mux options, bit,   */
} PSW_REV_SCH_MUX_OPTION;


/* FCH & DCCH Multiplex options -------------------------------------- */
typedef enum
{
   PSW_MUX_OPTION_0x1   = 0x1,
   PSW_MUX_OPTION_0x2   = 0x2,
   PSW_MUX_OPTION_0x704 = 0x4
} PSW_MUX_OPTIONS_TYPES;


/* define set of bitmap indicator */
typedef enum
{
   PSW_SO_0_BITS = 0,
   PSW_SO_4_BITS = 1,
   PSW_SO_8_BITS = 2
} PSW_SERV_OPT_BITMAP_IND;

/* define set of Band Class settings */
typedef enum
{
   PSW_BC_0 = 0x80,
   PSW_BC_1 = 0x40,
   PSW_BC_2 = 0x20,
   PSW_BC_3 = 0x10,
   PSW_BC_4 = 0x8,
   PSW_BC_5 = 0x4,
   PSW_BC_6 = 0x2,
   PSW_BC_7 = 0x1,
   PSW_BC_INVALID = 0
} PSW_BAND_CLASS_BIT_TYPE;


/* define set of Band Subclass settings */
typedef enum
{
   PSW_BSUBC_0 = 0x80,
   PSW_BSUBC_1 = 0x40,
   PSW_BSUBC_2 = 0x20,
   PSW_BSUBC_3 = 0x10,
   PSW_BSUBC_4 = 0x8,
   PSW_BSUBC_5 = 0x4,
   PSW_BSUBC_6 = 0x2,
   PSW_BSUBC_7 = 0x1,
   PSW_BSUBC_INVALID = 0
} PSW_BAND_SUB_CLASS_TYPE;

typedef enum
{
   PSW_CH_IND_RESERVED = 0x00,
   PSW_CH_IND_FCH      = 0x01,
   PSW_CH_IND_DCCH     = 0x02,
   PSW_CH_IND_FCH_AND_DCCH = 0x03
} PSW_CH_IND_TYPE;

typedef PACKED struct
{
   PSW_FCH_FRAME_SIZE_TYPE  FchFrameSize;           /* FCH_FRAME_SIZE, FCH frame Size capability indicator   */
   uint8                    ForFchLen;              /* FOR_FCH_LEN, Fwd Fundemental Channel Config Info. Len */
   uint16                   ForFchRcMap;            /* FOR_FCH_RC_MAP Forward Fundemental Radio Config. Info.*/
   uint8                    RevFchLen;              /* REV_FCH_LEN, Rev Fundemental Channel Config Info. Len */
   PSW_REV_RC_TYPE          RevFchRcMap;            /* REV_FCH_RC_MAP Reverse Fundemental Radio Config. Info.*/

} PSW_FCH_TYPE_SPECIFIC_FIELDS_TYPE;


typedef PACKED struct
{
   PSW_DCCH_FRAME_SIZE_TYPE DcchFrameSize;   /* DCCH_FRAME_SIZE, DCCH frame Size capability indicator */
   uint8                    ForDcchLen;      /* FOR_DCCH_LEN, Fwd Dedicated Ctrl Ch. Config Info. Len */
   uint16                   ForDcchRcMap;    /* FOR_DCCH_RC_MAP Fwd Dedicated Ch. Radio Config. Info. */
   uint8                    RevDcchLen;      /* REV_DCCH_LEN, Rev Dedicated Ctrl Ch. Config Info. Len */
   PSW_REV_RC_TYPE          RevDcchRcMap;    /* REV_DCCH_RC_MAP Rev Dedicated Ch. Radio Config. Info. */
} PSW_DCCH_TYPE_SPECIFIC_FIELDS_TYPE;


typedef PACKED struct
{
   uint8             ForSchLen;              /* FOR_SCH_LEN, Fwd Supplemental length in 3 bit uints   */
   uint16            ForSchRcMap;            /* FOR_SCH_RC_MAP, Fwd Supplemental Channel Radio Config Information. */
   uint8             ForSchNum;              /* FOR_SCH_NUM, Number of Forward Supplemental Cannel    */
   bool              ForTurboSupportedSch1;  /* FOR_TURBO_SUPPORTED, 1 Ch Fwd Turbo Coding Supported  */
   bool              ForTurboSupportedSch2;  /* FOR_TURBO_SUPPORTED, 2 Ch Fwd Turbo Coding Supported  */
   uint8             ForMaxTurboBlockSize;   /* FOR_MAX_TURBO_BLOCK_SIZE, Fwd Max Turbo Block Size    */
   bool              ForConvSupportedSch1;   /* FOR_CONV_SUPPORTED, 1 Ch Fwd convolutional Coding Supported */
   bool              ForConvSupportedSch2;   /* FOR_CONV_SUPPORTED, 2 Ch Fwd convolutional Coding Supported */
   uint8             ForMaxConvBlockSize;    /* FOR_MAX_CONV_BLOCK_SIZE, Fwd Max Conv. Code Block Size*/
} PSW_FOR_SCH_TYPE_SPECIFIC_FIELDS_TYPE;


typedef PACKED struct
{
   uint8             RevSchLen;              /* REV_SCH_LEN, Rev Supplemental length in 3 bit uints   */
   uint8             RevSchRcMap;            /* REV_SCH_RC_MAP Rev Dedicated Ch. Radio Config. Info.  */
   uint8             RevSchNum;              /* REV_SCH_NUM, Number of Reverse Supplemental Cannel    */
   bool              RevTurboSupportedSch1;  /* REV_TURBO_SUPPORTED, 1 Ch Rev Turbo Coding Supported  */
   bool              RevTurboSupportedSch2;  /* REV_TURBO_SUPPORTED, 2 Ch Rev Turbo Coding Supported  */
   uint8             RevMaxTurboBlockSize;   /* REV_MAX_TURBO_BLOCK_SIZE, Rev Max Turbo Block Size    */
   bool              RevConvSupportedSch1;   /* REV_CONV_SUPPORTED, 1 Ch Rev convolutional Coding Supported */
   bool              RevConvSupportedSch2;   /* REV_CONV_SUPPORTED, 2 Ch Rev convolutional Coding Supported */
   uint8             RevMaxConvBlockSize;    /* REV_MAX_CONV_BLOCK_SIZE, Rev Max Conv. Code Block Size*/
} PSW_REV_SCH_TYPE_SPECIFIC_FIELDS_TYPE;


typedef PACKED struct
{
   uint8             MaxMsNakRoundsFwd;      /* MAX_MS_NAK_ROUNDS_FWD Maximum number of RLP_NAK rounds
                                                supported by the MS on the forward traffic channel    */
   uint8             MaxMsNakRoundsRev;      /* MAX_MS_NAK_ROUNDS_REV Maximum number of RLP_NAK rounds
                                                supported by the MS on the reverse traffic channel    */
} PSW_RLP_CAP_BLOB_LEN_TYPE;


/* Status request, response */

/* Note: this struct body must sychronize to struct MsCapDb */
typedef PACKED struct
{
   /* structure offset: 00 - 17 */
   bool              Sr1Supported;
   bool              Sr3Supported;
   bool              RcClass1Supported;
   bool              RcClass2Supported;
   bool              RcClass3Supported;
   PSW_UI_ENCRYPT_MODE_TYPE UIEncryptionSupported;
   bool              EnhancedRc;
   uint16            bandClass;
   bool              RevFchGatingReq;
   bool              StsSupported;
   bool              ThreeXCchSupported;
   bool              WllSupported;
   PSW_SERV_OPT_BITMAP_IND SoBitmapInd;
   uint8             Reserved;
   bool              TieredServices;
   /*------- Information record for Capability Informtion --------------------------------------------*/
   /* structure offset: 18 - 26 */
   bool              QpchSupported;          /* QPCH, Quick Paging Channel Supported                  */
   bool              SlottedTimerSupported;  /* SLOTTED_TIMER, Slotted Timer Supported                */
   bool              ChsSupported;           /* CHS_SUPPORTED, Control Hold Mode indicator            */
   PSW_GATING_RATE_SET_TYPE GatingRateSet;   /* GATING_RATE_SET, set of supported Reverse Pilot
                                                gating rates.                                         */
   bool              ExtCapIncluded;         /* EXT_CAP_INCLUDED, Extended Capabilities Included
                                                Indicator                                             */
   bool              MaboSupported;          /* MABO, Mobile Assisted Burst Operation Capability
                                                indicator                                             */
   bool              SdbSupported;           /* SDB, Short Data Burst supported Indicator             */
   uint8             RlpCapBlopLen;          /* Field NOT USED. DELETE during next MS Cap Cleanup     */
   PSW_RLP_CAP_BLOB_LEN_TYPE RlpCapBlop;             /* RLP_CAP_BLOP, RLP capability information block of bits*/

   /*------- Information record for Channel Configuration Capability Information ---------------------*/
   /* structure offset: 27 - 28 */
   bool           OtdSupported;              /* OTD_SUPPORTED, OTD Supported Indicator                */
   bool           FchSupported;              /* FCH_SUPPORTED, Fundemental Ch. Supported Indicator    */
   /* structure offset: 29 - 34*/
   PSW_FCH_TYPE_SPECIFIC_FIELDS_TYPE FchTypeSpecificFields;
                                             /* FCH Type-specific fields, FCH Capability Infornmation */
   /* structure offset: 35 - 40 */
   bool           DcchSupported;             /* DCCH_SUPPORTED, Dedicated Ctrl Ch supported Indicator */
   PSW_DCCH_TYPE_SPECIFIC_FIELDS_TYPE DcchTypeSpecificFields;
                                             /* DCCH Type-specific fields, DCCH Capability Information*/
   /* structure offset: 41 - 51 */
   bool              ForSchSupported;        /* FOR_SCH_SUPPORTED, Fwd Supplemental Ch. Supported     */
   PSW_FOR_SCH_TYPE_SPECIFIC_FIELDS_TYPE ForSchTypeSpecificFields;
                                             /* FOR_SCH, Type-specific fields, Forward Supplemental
                                                Channel Configuration Capability Information          */
   /* structure offset: 52 - 61 */
   bool              RevSchSupported;        /* REV_SCH_SUPPORTED, Reverse Supplemental Ch. Supported */
   PSW_REV_SCH_TYPE_SPECIFIC_FIELDS_TYPE RevSchTypeSpecificFields;
                                             /* REV_SCH Type-specific fields, Reverse Supplemental
                                                Channel Configuration Capability Information          */
   /* offset: 62 */
   UINT8             revSchDurationUnit;
   UINT16            revSchDuration;

   bool              NonOctetAlignedData;    /* NONOCTET_ALIGNED_DATA Non-octet Aligned Data Block
                                                supported indicator.                                  */
   bool              OctetAlignedData;       /* OCTET_ALIGNED_DATA, Octet Aligned Data Block supported
                                                                                   indicator          */
   /*------- Information record for Extended Multiplex Option ----------------------------------------*/
   uint8             ForFchMoSupport;        /* For FCH Mux option                                    */

   uint8             RevFchMoSupport;        /* Rev FCH Mux option                                    */

   uint8             ForDcchMoSupport;       /* MO_FOR_DCCH, Forward Dedicated Control Channel
                                                Multiplex Options                                     */
   uint8             RevDcchMoSupport;       /* MO_REV_DCCH, Reverse Dedicated Control Channel
                                                Multiplex Options                                     */

   PSW_FOR_SCH_MUX_OPTION   ForSchHighestRs1Mo;
                                             /* Highest Rate Set 1 Forward Supplemental Channel
                                                Multiplex option supported                            */
   PSW_FOR_SCH_MUX_OPTION   ForSchHighestRs2Mo;
                                             /* Highest Rate Set 2 Forward Supplemental Channel
                                                Multiplex option supported                            */

   PSW_REV_SCH_MUX_OPTION   RevSchHighestRs1Mo;
                                             /* Highest Rate Set 1 Reverse Supplemental Channel
                                                Multiplex option supported                            */
   PSW_REV_SCH_MUX_OPTION   RevSchHighestRs2Mo;
                                             /* Highest Rate Set 2 Reverse Supplemental Channel
                                                Multiplex option supported                            */

   /*------- Information record for Geo-location codes -----------------------------------------------*/
   /* structure offset: 87 */
   PSW_GEO_LOC_TYPE  GeoLocType;             /* GEO_LOC, Geo-location Codes                           */
   /*------- Information record for Band Subclass ----------------------------------------------------*/
   /* structure offset: 88 */
   PSW_BAND_SUB_CLASS_TYPE  BandSubclassInfo_5;/* BAND_SUBCLASS_INFO, Band Subclass Information         */
   PSW_BAND_SUB_CLASS_TYPE  BandSubclassInfo_2_10;/* BAND_SUBCLASS_INFO, Band Subclass Information         */
   
   /* structure offset: 89 */
   PSW_ENCRYPT_MODE_TYPE    encryptModeSupported; /* ENCRYPT_MODE */

   uint8             OpMode;    /* Mobile Station Operation Mode */
   /* IS801 Location Services capability Information */
   /* struct offset 91 */
   uint8 			locPilotPhCap;  /* Mapping in IS801-1 Table 3.2.4.2-2 */
   /* struct offset 92 */
   uint16 			locGPSAcqCap;   /* Mapping in IS801-1 Table 3.2.4.2-3 */
   /* struct offset 94 */
   uint16 			locLocCalcCap;  /* Mapping in IS801-1 Table 3.2.4.2-4 */
   /* struct offset 96 */
   bool             locTCPCap;      /* Location Services TCP Capability */
   /* struct offset 97 */
   bool             locMSOrigCap;   /* Location Services MS Originated Cap */

   /* structure offset: 98 */
   UINT16            DcchOnlyPingPeriod;

   bool             dataReadyToSend;  /* DRS */
   PSW_CH_IND_TYPE  channelIndicator; /* CH_IND */
   
   UINT8		    InfoRecCapabilityMap;	

   uint16           Checksum;

} PswMsCapDbWriteMsgT;


typedef enum
{
  PSW_HSPD_CONTROL_HOLD_STATE = 0,
  PSW_HSPD_ACTIVE_STATE = 1
} PswHspdActiveControlHoldT;

typedef PACKED struct
{
  PswHspdActiveControlHoldT HspdNewState;
} PswHspdActiveControlHoldMsgT;

typedef enum
{
  PSW_SCH_9_6_KBPS   = 0,
  PSW_SCH_19_2_KBPS  = 1,
  PSW_SCH_38_4_KBPS  = 2,
  PSW_SCH_76_8_KBPS  = 3,
  PSW_SCH_153_6_KBPS = 4,
  PSW_SCH_307_2_KBPS = 5,
  PSW_SCH_614_4_KBPS = 6

} PswSchPrefRateT;

typedef PACKED struct
{
  uint8             SrId;
  PswSchPrefRateT   PrefRate;
  uint16            Duration;

} PswRevSchRequestMsgT;

/* PSW MISC parameters reported by DBM */
typedef PACKED struct
{
  uint8 StartupActiveNam;
  bool  opSidEntryForLastSystem;

} PswMiscT;

/* Get Active NAM message */
typedef PACKED struct
{
  ExeRspMsgT  RspInfo;         /* Response routing information */

} PswGetActiveNamMsgT;

/* Get Active NAM response message */
typedef PACKED struct
{
  uint8       CurrentActiveNam;

} PswGetActiveNamResponseMsgT;

/* Select Active NAM message */
typedef PACKED struct
{
  ExeRspMsgT  RspInfo;         /* Response routing information */
  uint8       SelectActiveNam;

} PswSelectActiveNamMsgT;

typedef enum
{
  DUAL_NAM_NOT_SUPPORTED = 0,
  NAM1_SELECTED          = 1,
  NAM2_SELECTED          = 2,
  PS_ACTIVE_NAM1_LOCKED  = 3,
  PS_ACTIVE_NAM2_LOCKED  = 4

} ActiveNamResultT;

/* Select Active NAM response message */
typedef PACKED struct
{
  uint8       ActiveNamResult;

} PswSelectActiveNamResponseMsgT;

typedef PACKED struct
{
  uint8  PWR_CNTL_STEP;
  bool   USE_TIME;
  uint8  ACTION_TIME;
  bool   FPC_INCL;
  uint8  FPC_MODE;
  bool   FPC_PRI_CHAN;
  bool   FPC_OLPC_FCH_INCL;
  uint8  FPC_FCH_FER;
  uint8  FPC_FCH_MIN_SETPT;
  uint8  FPC_FCH_MAX_SETPT;
  bool   FPC_OLPC_DCCH_INCL;
  uint8  FPC_DCCH_FER;
  uint8  FPC_DCCH_MIN_SETPT;
  uint8  FPC_DCCH_MAX_SETPT;
  bool   FPC_SEC_CHAN;
  uint8  NUM_SUP;
  bool   SCH_ID;
  uint8  FPC_SCH_FER;
  uint8  FPC_SCH_MIN_SETPT;
  uint8  FPC_SCH_MAX_SETPT;
  bool   FPC_THRESH_INCL;
  uint8  FPC_SETPT_THRESH;
  bool   FPC_THRESH_SCH_INCL;
  uint8  FPC_SETPT_THRESH_SCH;
  bool   RPC_INCL;
  uint8  RPC_NUM_REC;
  uint8  RPC_ADJ_REC_TYPE_0;
  uint8  RPC_ADJ_REC_LEN_0;
  bool   PCM_FCH_INCL;
  uint8  FCH_CHAN_ADJ_GAIN;
  bool   PCM_DCCH_INCL;
  uint8  DCCH_CHAN_ADJ_GAIN;
  bool   SCH0_INCL;
  uint8  SCH0_CHAN_ADJ_GAIN;
  bool   SCH1_INCL;
  uint8  SCH1_CHAN_ADJ_GAIN;
  uint8  RPC_ADJ_REC_TYPE_1;
  uint8  RPC_ADJ_REC_LEN_1;
  bool   RL_ATT_ADJ_GAIN_TYPE_1;
  bool   RC3_RC5_20MS_INCL_1;
  uint8  RL_ATT_ADJ_GAIN_1500;
  uint8  RL_ATT_ADJ_GAIN_2700;
  uint8  RL_ATT_ADJ_GAIN_4800;
  uint8  RL_ATT_ADJ_GAIN_9600;
  bool   RC4_RC6_20MS_INCL_1;
  uint8  RL_ATT_ADJ_GAIN_1800;
  uint8  RL_ATT_ADJ_GAIN_3600;
  uint8  RL_ATT_ADJ_GAIN_7200;
  uint8  RL_ATT_ADJ_GAIN_14400;
  bool   MS5_INCL;
  uint8  NORM_ATT_GAIN_9600_5MS;
  uint8  RPC_ADJ_REC_TYPE_2;
  uint8  RPC_ADJ_REC_LEN_2;
  bool   CODE_TYPE;
  bool   RL_ATT_ADJ_GAIN_TYPE_2;
  bool   RC3_RC5_20MS_INCL_2;
  uint8  RL_ATT_ADJ_GAIN_19200;
  uint8  RL_ATT_ADJ_GAIN_38400;
  uint8  RL_ATT_ADJ_GAIN_76800;
  uint8  RL_ATT_ADJ_GAIN_153600;
  uint8  RL_ATT_ADJ_GAIN_307200;
  uint8  RL_ATT_ADJ_GAIN_614400;
  bool   RC4_RC6_20MS_INCL_2;
  uint8  RL_ATT_ADJ_GAIN_28800;
  uint8  RL_ATT_ADJ_GAIN_576600;
  uint8  RL_ATT_ADJ_GAIN_115200;
  uint8  RL_ATT_ADJ_GAIN_230400;
  uint8  RL_ATT_ADJ_GAIN_460800;
  uint8  RL_ATT_ADJ_GAIN_1036800;
} PswTestPowerControlMsgT;

typedef PACKED struct
{
  uint8 NumAlt;
  uint16 Phase[SYS_CP_MAX_CANDIDATE_LIST_PILOTS];
  uint16 Strength[SYS_CP_MAX_CANDIDATE_LIST_PILOTS];
} PswL1dAlternatePilotsMsgT;

/* PSW_LOCATION_SVC_CFG_MSG */
typedef PACKED struct {
   bool  Mode; /* location privacy enabled */
} PswLocationSvcCfgMsgT;

 typedef PACKED struct
 {
 	bool MultiSMSMode;
 }PswSMSModeMsgT;
 
 
 
#ifdef SYS_OPTION_AMPS
/********************************/
/* AMPS Related Data Structures */
/********************************/
typedef PACKED struct
{
  uint16 Rssi;
} PswAmpsSetRssiThMsgT;

/* Transmit Status Indications ... for RECC access attempts   */
typedef enum {
   PSW_AMPS_RECC_TX_SUCCESS = 0,    /* Success            */
   PSW_AMPS_RECC_TX_MAXBUSY,        /* MAXBUSYsl exceeded */
   PSW_AMPS_RECC_TX_MAXSZTR         /* MAXSZTRsl exceeded */
} PswAmpsTxStatusT;

typedef PACKED struct
{
   PswAmpsTxStatusT TxStatus;
} PswAmpsBbIndTxStatMsgT;

/* Channel Format Indications */
typedef enum {
   PSW_AMPS_CHAN_IDLE = 0,
   PSW_AMPS_CONTROL_CHAN,
   PSW_AMPS_VOICE_CHAN,
   PSW_AMPS_CHAN_FORMAT_NONE
} PswAmpsChFormatT;

typedef PACKED struct
{
   PswAmpsChFormatT Format;
} PswAmpsBbIndChanFormatMsgT;

typedef PACKED struct
{
   uint8 Scc;
} PswAmpsBbIndSatCcMsgT;

/* FOCC word is 28 bits */
typedef PACKED struct
{
   uint8  Bytes[4];      /* Last 4 bits are not used */
   uint32 FrameIndex;    /* For duty cycle management 32 bits so message cracking will work */
} PswAmpsFoccWordT;

typedef PACKED struct
{
   PswAmpsFoccWordT Focc;
} PswAmpsBbIndFoccDataMsgT;

/* FVC word is 28 bits */
typedef PACKED struct
{
   uint8 Bytes[4];      /* Last 4 bits are not used */
} PswAmpsFvcWordT;

typedef PACKED struct
{
   PswAmpsFvcWordT Fvc;
} PswAmpsBbIndFvcDataMsgT;

typedef PACKED struct
{
   bool CarrierStatus;
} PswAmpsRelayRadioTuneStatMsgT;

typedef enum {
   PSW_AMPS_FSU_NOT_LOCKED = 0,
   PSW_AMPS_FSU_LOCKED
} PswAmpsFsuLockT;

typedef PACKED struct
{
   PswAmpsFsuLockT LockStatus;
} PswAmpsRelayRadioFsuLockMsgT;

typedef PACKED struct
{
   uint16 RssiValue;
} PswAmpsRelayRadioRssiValMsgT;

typedef enum {
   PSW_AMPS_RADIO_CARRIER_OFF = 0,
   PSW_AMPS_RADIO_CARRIER_ON
} PswAmpsCarrierStatusT;

typedef PACKED struct
{
   PswAmpsCarrierStatusT CarrierStatus;
} PswAmpsRelayRadioCarrierStatMsgT;

typedef PACKED struct
{
   uint8 PowerLevel;
} PswAmpsRelayRadioPowerLvlMsgT;

#define PSW_AMPS_NUM_DIGITS 30
typedef PACKED struct
{
   bool              Otasp;                        /* OTASP originated call        */
   PswRequestModeT   RequestMode;                  /* requested call type          */
   bool              SpecialService;               /* special service option requested */
   PswServiceOptionT ServiceOption;                /* requested service option     */
   bool              PrivacyMode;                  /* voice privacy indicator      */
   bool              DigitMode;                    /* digit encoding indicator     */
   PswNumberTypeT    NumberType;                   /* type of number               */
   PswNumberPlanT    NumberPlan;                   /* numbering plan               */
   uint8             NumDigits;                    /* number of digits to be dialled       */
   uint8             Digits[PSW_AMPS_NUM_DIGITS];  /* ascii digits, null terminated string*/
} PswAmpsCpRqCallInitiateMsgT;

typedef PACKED struct
{
   bool              Otasp;                        /* OTASP originated call        */
   PswRequestModeT   RequestMode;                  /* requested call type          */
   bool              SpecialService;               /* special service option requested */
   PswServiceOptionT ServiceOption;                /* requested service option     */
   bool              PrivacyMode;                  /* voice privacy indicator      */
   bool              DigitMode;                    /* digit encoding indicator     */
   PswNumberTypeT    NumberType;                   /* type of number               */
   PswNumberPlanT    NumberPlan;                   /* numbering plan               */
   uint8             NumDigits;                    /* number of digits to be dialled       */
   uint8             Digits[PSW_AMPS_NUM_DIGITS];  /* ascii digits, null terminated string*/
} PswAmpsCpRqHookflashMsgT;

typedef enum {
   PSW_AMPS_AUDIO_VOICE_OFF = 0,
   PSW_AMPS_AUDIO_VOICE_ON
} PswAmpsVoiceStatT;

typedef PACKED struct
{
   PswAmpsVoiceStatT VoiceStat;
} PswAmpsAudioVoiceMsgT;

typedef PACKED struct
{
   uint16	IpcNumGoodSync;
   uint16	IpcNumBadSync;
} PswAmpsWordSyncRspMsgT;

#endif  /* #ifdef SYS_OPTION_AMPS */

typedef PACKED struct
{
  uint8   UpdateIMSI_M;       /* if update IMSI_M */
  uint8   UpdateIMSI_T;       /* if update IMSI_T */
  uint8   IMSI_S[10];         /* IMSI_S 10 raw digits */
  uint8   NUM_IMSI_S_DIGITS;  /* IMSI_S number of digits */
  uint8   UpdateIMSI_11_12;   /* if update IMSI_T */
  uint8   IMSI_11_12[2];      /* IMSI_11_12 2 raw digits */
  uint8   UpdateMCC;          /* if update MCC */
  uint8   MCC[3];             /* MCC 3 raw digits */
  uint8   UpdateMDN;          /* if update MCC */ 
  uint8   Mdn[10];            /* MDN max 10 digits */
  uint8   MDN_NUM_DIGITS;     /* MDN number of digits */
} PswWriteRawIMSIMsgT;

typedef PACKED struct
{
  ExeRspMsgT  RspInfo;
} PswReadRawIMSIMsgT;

typedef PACKED struct
{
  uint8   ValidIMSImap;      /* bit0: IMSI_M, bit1:IMSI_T */
  uint8   M_MCC[3];          /* MCC raw digits */
  uint8   M_IMSI_11_12[2];   /* M_IMSI_11_12 raw digits */
  uint8   M_IMSI_S[10];      /* M_IMSI_S raw digits */
  uint8   M_IMSI_Class;		 
  uint8   M_AddrNum;
  uint8   T_MCC[3];          /* MCC raw digits */
  uint8   T_IMSI_11_12[2];   /* M_IMSI_11_12 raw digits */
  uint8   T_IMSI_S[10];      /* M_IMSI_S raw digits */
  uint8   T_IMSI_Class;		 
  uint8   T_AddrNum;
  uint8   Mdn[NAM_MAX_MDN_DIGITS];  /* MDN max 16 digits */
  uint8   MDN_NUM_DIGITS;           /* MDN number digits */
  uint8   ACCOLCp;           /* access overload class */
} PswReadRawIMSIRspT;

typedef PACKED struct
{
  bool    vetoSlotted;
} PswL1dSlottedVetoMsgT;
 



#endif /* #ifndef _PSWAPI_H_ */

#ifndef  _L1DAPI_H_
#define  _L1DAPI_H_


/*----------------------------------------------------------------------------
* Include Files
----------------------------------------------------------------------------*/

#include "sysdefs.h"
#include "sysapi.h"
#include "l1dampsapi.h"
#include "sysparm.h"
/*----------------------------------------------------------------------------
* EXE Interfaces - Definition of Signals and Mailboxes
*----------------------------------------------------------------------------*/
#define L1D_MDM_MAILBOX         EXE_MAILBOX_1_ID   

/* Note: When any signal defn changes, l1d_trace.txt has to be updated */
#define L1D_MDM_STARTUP_SIGNAL  EXE_SIGNAL_1  

/* define signals used by L1D state machine to initiate actions at future times */
#define L1D_TIME_CHG_SIGNAL                 EXE_SIGNAL_2
#define L1D_FRAME_SET_SIGNAL                EXE_SIGNAL_3
#define L1D_ACCESS_PREAMBLE_SIGNAL          EXE_SIGNAL_4
#define L1D_ENABLE_REV_TX_SIGNAL            EXE_SIGNAL_5
#define L1D_SVC_CFG_SIG_TX_ON_SIGNAL        EXE_SIGNAL_6
#define L1D_RSCH_LMD_STOP_SIGNAL            EXE_SIGNAL_7
#define L1D_RF_TUNE_COMPLETE_SIGNAL         EXE_SIGNAL_8
#define L1D_SERVICE_CONFIG_SIGNAL           EXE_SIGNAL_9
#define L1D_SERVICE_CONFIG_NN_SIGNAL        EXE_SIGNAL_10
#define L1D_HARD_HANDOFF_SIGNAL             EXE_SIGNAL_11
#define L1D_SOFT_HANDOFF_SIGNAL             EXE_SIGNAL_12
#define L1D_RSCH_SIGNAL                     EXE_SIGNAL_13
#define L1D_RSCH_DISABLE_SIGNAL             EXE_SIGNAL_14
#define L1D_HO_PREAMBLE_OFF_SIGNAL          EXE_SIGNAL_15
#define L1D_POWER_CONTROL_PARAMS_SIGNAL     EXE_SIGNAL_16
#define L1D_FSCH_SIGNAL                     EXE_SIGNAL_17
#define L1D_FSCH_DISABLE_SIGNAL             EXE_SIGNAL_18
#define L1D_LONG_CODE_CHANGE_SIGNAL         EXE_SIGNAL_19
#define L1D_CDMA_TO_AMPS_HO_SIGNAL          EXE_SIGNAL_20
#define L1D_FSCH_BURST_END_SIGNAL           EXE_SIGNAL_21
#define L1D_SUBCHAN_GAIN_SIGNAL             EXE_SIGNAL_22
#define L1D_LMD_RLP_STOP_REQ_SIGNAL         EXE_SIGNAL_23

/*----------------------------------------------------------------------------
* Message IDs for signals and commands sent to L1D
*----------------------------------------------------------------------------*/
/*
* IMPORTANT NOTE: Please add msg id's at the end of the enum (not in the middle)
*                 but before L1D_NUM_CMD_MSG_IDS.
*/
typedef enum 
{
/* 0*/ L1D_ACCESS_ABORT_MSG,
        L1D_ACCESS_SIG_MSG, 
        L1D_BCCH_START_MSG,
        L1D_BCCH_STOP_MSG, 
        L1D_CAND_FREQ_MSG,
        L1D_CAND_FREQ_ABORT_MSG, 
        L1D_CAND_FREQ_START_MSG, 
        L1D_CSCH_STOP_MSG, 
        L1D_DC_BIAS_MSG,         
        L1D_EACH_SIG_MSG, 
/* A*/ L1D_FCCCH_START_MSG, 
        L1D_FDSCH_ACQ_IND_MSG,
        L1D_FSCH_MSG,
        L1D_IDLE_CHANGE_PILOT_SET_MSG, 
        L1D_IDLE_SETS_LIST_UPDATE_MSG, 
        L1D_LONG_CODE_CHANGE_MSG, 
        L1D_PAGING_CHAN_START_MSG, 
        L1D_PAGING_CHAN_STOP_MSG, 
        L1D_PILOT_ACQ_START_MSG, 
        L1D_PILOT_PWR_MSG,       
/*14*/ L1D_PREAMBLE_OFF_MSG,
        L1D_PREAMBLE_ON_MSG,
        L1D_QUICK_PAGE_BITS_MSG,
        L1D_RESET_MSG, 
        L1D_RESOURCE_ALLOC_MSG,
        L1D_RESOURCE_DEALLOC_MSG,
        L1D_RESOURCE_REQ_MSG, 
        L1D_REVERSE_PILOT_CTL_MSG,
        L1D_REVERSE_POWER_SET_MSG,
        L1D_RX_POWER_MSG,
/*1E*/ L1D_SEARCH_PARMS_UPDATE_MSG,
        L1D_SEARCH_RSLT_MSG,
        L1D_SERVICE_CONFIG_MSG,
        L1D_SERVICE_CONFIG_NN_MSG,
        L1D_SETS_LIST_UPDATE_MSG,
        L1D_SLEEP_RDY_MSG,
        L1D_SLOTTED_MODE_DISABLE_MSG,
        L1D_SLOTTED_MODE_PARAMS_MSG,
        L1D_SLEEP_MSG,
        L1D_WAKEUP_MSG,
/*28*/  L1D_DSPM_SLOTTED_MSG,
        L1D_SYNC_ACQ_START_MSG,
        L1D_TIMING_CHANGE_MSG,
        L1D_TRAFFIC_CHAN_HARD_HANDOFF_MSG,
        L1D_TRAFFIC_CHAN_PWR_STEP_MSG,   /* obsolete */
        L1D_TRAFFIC_CHAN_SOFT_HANDOFF_MSG,
        L1D_TRAFFIC_CHAN_START_MSG,
        L1D_TRAFFIC_CHAN_STOP_MSG,
        L1D_TRAFFIC_INIT_COMPLETE_MSG,
        L1D_TRANSMITTER_CONTROL_MSG,
/*32*/  L1D_FOR_POWER_CONTROL_PARAMS_MSG,
        L1D_OUTERLOOP_REQ_MSG,
        L1D_FPC_REP_MSG,
        L1D_SIM_MSG,
        L1D_TST_DIAG_MONITOR_MSG,
        L1D_TST_GET_PHONE_STATE_MSG,
        L1D_IDLE_DIFF_FREQ_SEARCH_MSG,
        L1D_ACCESS_PROBE_COMPLETE_MSG,
        L1D_GET_PS_INFO_RSP_MSG,
        L1D_TST_TX_CDMA_MSG,
/*3C*/  L1D_RSCH_MSG,
        L1D_AMPS_HANDOFF_MSG,
        L1D_FPC_CAL_FACTOR_MSG,
        L1D_GET_MMI_INFO_RSP_MSG,
        L1D_GET_DSPV_INFO_RSP_MSG,
        L1D_AFLT_SEARCH_RSLT_MSG,
        L1D_AFLT_DSP_SEARCH_TIMEOUT_MSG,
        L1D_POS_AFLT_SEARCH_MSG,
        L1D_POS_AFLT_ABORT_MSG,
        L1D_DSM_MDM_RST_ACK_MSG,
/*46*/  L1D_RX_POWER_REQ_MSG,
        L1D_SYNC_CHAN_MSG_RECD_MSG,
        L1D_INIT_MSG,
        L1D_TST_DEBUG_FILTER_MSG,
        L1D_QPCH_CCI_DECODE_REQ_MSG,
        L1D_IDLE_PRIORITY_SET_UPDATE_MSG,
        L1D_OOSA_MSG,
        L1D_OOSA_WAKEUP_MSG,
        L1D_PUF_PROBE_ABORT_MSG,
        L1D_PUF_PROBE_SEND_MSG,
/*50*/  L1D_SET_T_ADD_MSG,
        L1D_SEND_AMPS_RSSI_MSG,
        L1D_GPS_INIT_GPS_MSG,
        L1D_GPS_NWK_RPT_SYSTEM_TIME_MSG,
        L1D_GPS_CONT_CDMA_REQ_MSG,
        L1D_GPS_N1_GOOD_FRAMES_FOUND_MSG,
        L1D_GPS_PSMM_SENT_MSG,
        L1D_GPS_SWITCHING_HANDOFF_COMPLETE_MSG,
        L1D_GPS_GOOD_FRAMES_TMR_EXP_MSG,
/*59*/  L1D_GPS_XMIT_TMR_EXP_MSG,
        L1D_GPS_HANDOFF_IND_TMR_EXP_MSG,
        L1D_GPS_END_GPS_REQ_MSG,
        L1D_DSPM_TIMING_RSP_MSG,
        L1D_TST_IDLE_DIFF_FREQ_SRCH_MODE_MSG,
        L1D_AFLT_USUAL_SEARCH_TIMEOUT_MSG,
        L1D_TST_IGNORE_RESET_MSG,
/*60*/ L1D_FSCH_CONFIG_UPDATE_MSG,
        L1D_RSCH_CONFIG_UPDATE_MSG,
        L1D_SRCH_RESULT_COMPLETE_MSG,
        L1D_VCXO_COMP_MSG,
        L1D_ACCESS_IN_PROGRESS_MSG,
        L1D_GET_DEBUG_SCRN_INFO_MSG,
        L1D_HWD_RESET_ACK_MSG,
        L1D_RSCH_POWER_SET_MSG,
        L1D_ALT_PILOT_ACQUIRE_MSG,
#if ((SYS_ASIC == SA_ROM) && (SYS_VERSION <= SV_REV_C3))
        L1D_TST_AFC_CONFIG_MSG,
        L1D_TST_RX_AGC_CONFIG_MSG,
#endif
        L1D_NUM_CMD_MSG_IDS
} L1dMsgIdT;
/*
* IMPORTANT NOTE: Please add msg id's at the end of the enum (not in the middle)
*                 but before L1D_NUM_CMD_MSG_IDS.
*/
typedef enum 
{
  PCH  = 0,
  QPCH1= 1,
  QPCH2= 2,
  CCI  = 3
} L1dSleepModeT;

/*----------------------------------------------------------------------------
* Basic Types
*----------------------------------------------------------------------------*/
#define L1D_LONG_CODE_MASK_SIZE       6    /* array size for long code mask */
#define L1D_LONG_CODE_STATE_SIZE      6    /* array size for long code state */
#define L1D_MAX_RATE_ENTRY            4    /* array size for fch power array */
#define L1D_LONG_CODE_MASK_PAR_SIZE   2    /* array size for long code mask parameter array */
#define L1D_AUX_WALSH_ARRAY_SIZE      1    /* array size for aux pilot walsh codes */
                                           /* NOTE: -- The true size for this array is 65 -- It is set to 1 to */
                                           /* reduce the size of the L1dTrafficChanStartMsgT struct. When the Aux Pilot */
                                           /* Walsh code arrays are enabled, they will be moved to a separate message   */
#define L1D_CHANGE_PILOT_SET_SEQ_NUM -1    /* default sequence number for L1D_CHANGE_PILOT_SET_MSG */ 

#define L1D_MAX_SCH_SHO               8
#define L1D_MAX_FORWARD_SCH           2
#define L1D_MAX_REVERSE_SCH           2

#define L1D_PDM_BAL_SIZE              8
#define L1D_ADC_BAL_SIZE              8

/* L1D_FSCH_MSG */

typedef PACKED struct
{
  uint16        PilotPn;
  uint16        ForSchCcIndex;
  uint8         QofMaskIdSch;
} L1dForSchShoRecT;

typedef PACKED struct
{
  uint8         Fer;
  uint8         MinSetpt;
  uint8         MaxSetpt;
} L1dFpcOlPcParamsT;

typedef PACKED struct
{
  uint8             SchId;
  L1dFpcOlPcParamsT Sch;
} L1dFpcOlPcSCHParamsT;

typedef PACKED struct
{
  uint8             SchId;
  uint8             ForSchRate;
  uint8             ForSchDuration;
  SysActionTimeT    ForSchActionTime;
  uint8             NumSupSho;
  L1dForSchShoRecT  ForSchSho[ L1D_MAX_SCH_SHO ];
} L1dForSchRecT;

typedef PACKED struct
{
  bool                  FpcPriChansIncl;
  uint8                 FpcPriChans;
  uint8                 PilotGatingUseRate;
  bool                  ForSchFerRep;
  uint8                 NumForSch;
  L1dForSchRecT         ForSchRec[ L1D_MAX_FORWARD_SCH ];
  bool                  FpcIncl;
  uint8                 FpcModeSch;
  uint8                 FpcSecChan;
  uint8                 FpcSchInitSetptOp;
  bool                  FpcThreshSchIncl;
  uint8                 FpcSetptThreshSch;
  uint8                 NumSup;
  bool                  FpcSchInitSetptIncl;
  int16                 FpcSchInitSetpt[ L1D_MAX_FORWARD_SCH ];
  L1dFpcOlPcSCHParamsT  Sch[ L1D_MAX_FORWARD_SCH ];
} L1dFschMsgT;

/*
L1D_FSCH_CONFIG_UPDATE_MSG
*/
typedef PACKED struct
{
  bool              DroppingFchSchPilot;
  uint8             NumForSch;
  uint8             NumSupSho[ L1D_MAX_FORWARD_SCH ];
  L1dForSchShoRecT  ForSchSho[ L1D_MAX_FORWARD_SCH ][ L1D_MAX_SCH_SHO ];
} L1dForSchConfigUpdateMsgT;

/*
L1D_RSCH_CONFIG_UPDATE_MSG
*/
typedef PACKED struct
{
  uint8             RevSchId;
  uint8             RevSchRate;
  uint8             RevWalshId;
} L1dRschConfigT;

typedef PACKED struct
{
  uint8             NumRevSch;
  L1dRschConfigT    RevSchConfigRec[L1D_MAX_REVERSE_SCH];
} L1dRschConfigUpdateMsgT;

/*
*/

typedef PACKED struct
{
  SysActionTimeT        ActTime;
  uint8                 Channels;
  uint8                 FpcPriChan;
} L1dResourceAllocMsgT;


typedef PACKED struct
{
  SysActionTimeT        ActTime;
  uint8                 Channels;
  uint8                 PilotGatingRate;
} L1dResourceDeallocMsgT;


/* define pilot list elements */ 
/* these structures may be implemented later when Rev A is supported */
typedef PACKED struct 
{
  uint8 TdPowerLevel;     /* Transmit Diversity Power Level, see Table 3.7.6.1-1, IS2000.5-A */
  uint8 TdMode;           /* Transmit Diversity Mode, see Table 3.7.6.1-2, IS2000.5-A */ 
} L1dX1CommPilotTdT; 
 
typedef PACKED struct 
{
  uint8  Qof;                               /* Quasi-orthogonal function index for the Auxiliary Transmit Diversity Pilot
                                               Possible values: 0..3,  see Table 3.1.3.1.12-2, IS2000.2-A */
  uint8  WalshLength;                       /* Length of the Walsh Code, WalshLength = (WALSH_LENGTH + 6) where WALSH_LENGTH 
                                               is determined by mapping in Table 3.7.6.1-3, IS2000.5-A */
  uint8  AuxWalsh[L1D_AUX_WALSH_ARRAY_SIZE]; /* Walsh Code for the Auxiliary Pilot */ 
  uint8  AuxTdPowerLevel;                   /* See Table 3.7.6.1-4, IS2000.5-A */
  uint8  TdMode;                            /* Transmit Diversity Mode, see Table 3.7.6.1-2, IS2000.5-A */
} L1dX1AuxPilotTcT; 

typedef PACKED struct 
{
  uint8  Qof;                                     /* Quasi-orthogonal function index, possible values: 0..3, 
                                                     see Table 3.1.3.1.12-2 */
  uint8  WalshLength;                             /* Length of the Walsh Code, WalshLength = (WALSH_LENGTH + 6) where 
                                                     WALSH_LENGTH is determined by mapping in Table 3.7.6.1-3, IS2000.5-A */
  uint8 AuxPilotWalsh[L1D_AUX_WALSH_ARRAY_SIZE];  /* Walsh COde for the Auxiliary Pilot */
} L1dX1AuxPilotT; 

typedef PACKED union 
{
  L1dX1CommPilotTdT PilotType0;
  L1dX1AuxPilotT    PilotType1;
  L1dX1AuxPilotTcT  PilotType2;
} L1dPilotRecT; 

 
typedef PACKED struct
{
    uint16          PilotPn;                    
    bool            AddPilotRecIncl;     /* 0 for common pilot, 1 for others */
    uint8           PilotRecType;        /* see Table 3.7.2.3.2.21-6, IS2000.5-A, valid only if AddPilotRecIncl = 1 */
    L1dPilotRecT    PilotRec;            /* union, valid only if AddPilotRecIncl = 1 */
    bool            PowerCombInd;        /* Power control symbol combining indicator */
    uint16          CodeChanFch;         /* Code channel index (Walsh) for FCH, valid if ChInd == 1 or 3 */
    uint8           QofMaskIdFch;        /* Qof for FCH, see Table 3.1.3.1.12-2, IS2000.2A, valid if ChInd == 1 or 3 */
    uint16          CodeChanDcch;        /* Code channel index for DCCH, valid if ChInd == 2 or 3 */
    uint8           QofMaskIdDcch;       /* Qof for FCH, see Table 3.1.3.1.12-2, IS2000.2A, valid if ChInd == 2 or 3 */
    uint8	        SrchOffset;		     /* Search offset (0=not included)  */
} L1dTcActiveSetT;
 
/*----------------------------------------------------------------------------
*  Message Definitions
*       There is a section for each defined message type.  If
*       there is no MsgT shown, there is no data associated with
*       the message.
*----------------------------------------------------------------------------*/

/*
** L1D_ACCESS_ABORT_MSG
*/
typedef PACKED struct
{
    SysSystemTimeT      SendTime;                           /* System Time (20ms) the signal tranmission is to start */
    uint8               PreambleSize;                       /* Preamble size in 20ms frames, L1D controls preamble */
} L1dAccessAbortMsgT;

/*
** L1D_ACCESS_SIG_MSG
*/
typedef PACKED struct
{
    SysSystemTimeT      SendTime;                           /* System Time (20ms) the signal tranmission is to start */
    int16               PowerCorrect;                       /* Transmit power level, less mip */
    bool                UseCurrMip;                         /* Flag to indicate use of current mean input power */
    uint8               LcMask[L1D_LONG_CODE_MASK_SIZE];    /* Long code mask  */
    uint16              RandomDelay;                        /* PN randomization delay in chips before access attempt */
    uint16              AccessFrames;                       /* total number of access channel frames */
    uint8               PreambleSize;                       /* Preamble size in 20ms frames, L1D controls preamble */
} L1dAccessSigMsgT;

/*
** L1D_BCCH_START_MSG
*/
typedef enum
{
    L1D_BCCH_4800_RATE,
    L1D_BCCH_9600_RATE,
    L1D_BCCH_19200_RATE
} L1dBcchDataRateT;

typedef enum
{
    L1D_BCCH_QUARTER_CRAT,
    L1D_BCCH_HALF_CRAT
} L1dBcchCodeRateT;


typedef PACKED struct
{
    SysCdmaBandT        CdmaBand;      /* band */
    uint16              FreqChan;      /* frequency channel */
    uint16              PilotPn;       /* Serving base station pilot PN squence */
    uint8               WalshCode;     /* BCCH code channel */
    L1dBcchDataRateT    BRate;         /* BCCH data rate */
    bool                TdSupported;   /* Transmit Diversity supported */
    uint8               TdMode;        /* Transmit Divesity Mode: OTD or STS */
    uint8               TdPwrLevel;                         /* Transmit Diversity Power Level */
    L1dBcchCodeRateT    CRate;                              /* Code Rate:    */
    uint8               LcMask[L1D_LONG_CODE_MASK_SIZE];    /* Long code Mask */
} L1dBcchStartMsgT;

/*
** L1D_BCCH_STOP_MSG
*/

/*
** L1D_CAND_FREQ_MSG (and supporting structure/enum definitions)
*/
typedef enum
{
   L1D_CP_SM_NO_SEARCH_PRIORITIES_OR_SEARCH_WINDOWS,
   L1D_CP_SM_SEARCH_PRIORITIES,
   L1D_CP_SM_SEARCH_WINDOWS,
   L1D_CP_SM_SEARCH_PRIORITIES_AND_SEARCH_WINDOWS
} L1dNghbrSearchModeT;

typedef PACKED struct
{
   uint16 NghbrPN;
   uint8  CfSrchWinN;      /* Candidate Frequency search window */
   uint8  SrchOffsetNghbr; /* Zero if not included */
} L1dNghbrPilotT;

typedef PACKED struct
{
   SysCdmaBandT        CdmaBand;        /* EIA/TIA band identifier */
   uint16              FreqChan;        /* Frequency channel number */
   L1dNghbrSearchModeT CfNghbrSrchMode; /* Neighbor Search mode */
   uint8               NumPilots;       /* Number of Pilot PNs that are used */
   L1dNghbrPilotT      Pilot[SYS_CP_MAX_CANDIDATE_FREQ_LIST];
} L1dCandFreqMsgT;

/*
** L1D_CAND_FREQ_START_MSG
*/
typedef PACKED struct
{
   SysActionTimeT ActTime;         /* Action Time */
   bool           AlignTiming;     /* Align Timing Indicator */
   uint8          SearchOffset;    /* Search Offset in 1.25msec power control groups from action time */
   uint8          TotalOffTimeFwd; /* Time off Fwd channel in 1.25msec power control groups */
   uint8          TotalOffTimeRev; /* Time off Rev channel in 1.25msec power control groups */
} L1dCandFreqSearchStartMsgT;

/*
** L1D_CSCH_STOP_MSG
*/

/*
** L1D_DC_BIAS_MSG
*/

/*
** L1D_EACH_SIG_MSG
*/
typedef enum
{
    FRAME_DURATION_5MS,
    FRAME_DURATION_10MS,
    FRAME_DURATION_20MS
}L1dFrameDurationT;


typedef PACKED struct 
{
    uint32 MaskPar[L1D_LONG_CODE_MASK_PAR_SIZE];
} L1dLongCodeMaskParT;

typedef PACKED struct
{
   
    uint8               EachSlot;               /* Slot duration in 1.25 ms units */
    uint8               SysTimeOffset;          /* N as defined in IS2000.2-A 2.1.3.4 */
    L1dLongCodeMaskParT LongCodeMaskPar;        /* Contains parameters to calculate Long Code Mask for EACH */
    bool                PreambleEnabled;        /* Allowed to send preamble on RPiCH */
    int16               RPichPwrPreamble;       /* RPiCH MOP when transmiting the preamble */
    int16               RPichPwr;               /* RPiCH Power after preamble in db */
    bool                UseCurrMip;             /* Flag to indicate use of current mean */
                                                /*  input power */
    uint8               PreambleNumFrac;        /* (Fractional preamble duration + 1) in 1.25ms */
    uint8               PreambleFracDuration;   /* preamble_frac_duration + 1) in 1.25ms */
    uint8               PreambleOffDuration;    /* In 1.25 ms */
    uint8               PreambleAddDuration;    /* Additional preamble duration in 1.25ms*/
    int16               EachPwr;                /* Transmit power used on the EACH in db */
    L1dFrameDurationT   FrameDuration;          /* frame duration in units of msecs(5,10,20) */
    uint8               FrameNumBits;           /* number of bits per frame */
} L1dEachSigMsgT;

/*
** L1D_FCCCH_START_MSG
*/
typedef enum
{
    L1D_FCCCH_9600_20,      /* 9600 at 20ms */
    L1D_FCCCH_19200_20,     /* 19200 at 20ms */
    L1D_FCCCH_19200_10,     /* 19200 at 10ms */
    L1D_FCCCH_38400_20,     /* 38400 at 20ms */
    L1D_FCCCH_38400_10,     /* 38400 at 10ms */
    L1D_FCCCH_38400_5       /* 38400 at 5ms */
}L1dFccchDataRateT;

typedef enum
{
    L1D_FCCCH_QUARTER_CRATE,
    L1D_FCCCH_HALF_CRATE
}L1dFccchCRateT;

typedef PACKED struct
{
    uint16              PilotPn;      /* Serving base station pilot PN squence */
    uint8               WalshCode;    /* FCCCH channel number */
    L1dFccchCRateT      CRate;                              /* FCCCH coding rate */
    L1dFccchDataRateT   DataRate;                           /* FCCCH data rate: See Table 3.7.2.3.2.31-1*/
    uint8               LcMask[L1D_LONG_CODE_MASK_SIZE];    /* Long code Mask */
} L1dFccchStartMsgT;

/*
** L1D_FDSCH_ACQ_IND_MSG
*/



/*
** L1D_IDLE_CHANGE_PILOT_SET_MSG
*/



typedef PACKED struct
{
  int16          PilotSeq;            /* Seq number used in PSW_SCAN_MEASUREMENT_MSG */
  bool           FreqInc;             /* Frequency Included */
  SysCdmaBandT   CdmaBand;            /* band (1st relase always PCS)*/
  uint16         FreqChan;            /* frequency channel */
  uint8          NumPilotPn;          /* Number of Pilot PN's */
  uint16         PilotPn [SYS_CP_MAX_IDLE_PRIORITY_PILOTS];
} L1dIdleChangePilotSetMsgT;



/*
** L1D_IDLE_SETS_LIST_UPDATE_MSG
*/

typedef enum 
{
  L1D_NINE_DB_BELOW,
  L1D_SIX_DB_BELOW,
  L1D_THREE_DB_BELOW,
  L1D_SAME_AS_FWD_PILOT,
  L1D_NOT_INCLUDED
} L1dTdPowerLevelT;

typedef PACKED struct
{
    uint16              NghbrPn;            /* Neighbor station PN offset */
    uint8               SrchWinNghbr;       /* Neighbor search window size, coded value per Table 2.6.6.2.1-1, IS2000-5 */
    SysCdmaBandT        Band;               /* Neigbor band class */
    bool                FreqIncl;           /* 0 = Freq Not Incl, 1 = Freq Incl */
    uint16              Freq;               /* Cdma channel number */
    uint8               SrchOffsetNghbr;    /* coded value per Table 2.6.6.2.1-2, IS2000.5 */
    L1dTdPowerLevelT    TdPowerLevel;       /* power level relative to fwd pilot */
} L1dIdleNghborConfigT;

typedef PACKED struct
{
    int16   PilotSeq;    /* Sequence number to match up with the measurement report */ 
    uint16  PilotInc;    /* 64 chip units, Pilot PN sequence offset index increment which specifies the interval 
                            between pilots */
    uint8   NumPilots;   /* Number of pilots in the idle neighbor configuration list */
    L1dIdleNghborConfigT  IdleNeighborConfig [SYS_CP_MAX_NEIGHBOR_LIST_PILOTS];    
} L1dIdleSetsListUpdateMsgT;


/*
** L1D_LONG_CODE_CHANGE_MSG
*/
typedef PACKED struct
{
    uint8           LcMask[L1D_LONG_CODE_MASK_SIZE];    /* long code mask to change to          */
    SysSystemTimeT  ActTime;                            /* system time the change is to take effect  */
} L1dLongCodeChangeMsgT;

/*
** L1D_PAGING_CHAN_START_MSG
*/
typedef enum
{
    L1D_PAGING_9600_RATE,
    L1D_PAGING_4800_RATE 
} L1dPagingDataRateT;

typedef PACKED struct
{
    SysCdmaBandT        CdmaBand;                           /* band (1st relase always PCS) */
    uint16              FreqChan;                           /* frequency channel */
    uint16              PilotPn;                            /* Serving base station pilot PN squence */
    uint8               WalshCode;                          /* Page Channel number (1 to 7) */
    L1dPagingDataRateT  DataRate;                           /* Paging channel data rate 4800 bps or 9600 bps */
    uint8               LcMask[L1D_LONG_CODE_MASK_SIZE];    /* Long code Mask */
} L1dPagingChanStartMsgT;

/*
** L1D_PAGING_CHAN_STOP_MSG
*/

/*
** L1D_PILOT_ACQ_START_MSG
*/
typedef PACKED struct
{
    SysCdmaBandT   CdmaBand;    /* band */
    uint16         FreqChan;    /* frequency channel */
} L1dPilotAcqStartMsgT;

/*
** L1D_PILOT_PWR_MSG
*/

/*
** L1D_PREAMBLE_OFF
*/

/*
** L1D_PREAMBLE_ON
*/

/*
** L1D_PSW_SHARED_BUFFER_READY_MSG
*/
typedef PACKED struct
{
    L1dMsgIdT    MsgId;      /* Msg ID of msg placed into large message buffer */
    void         *DataPtr;   /* pointer to buffer containing message */
} L1dPswMsgBufferReadyMsgT;


/*
** L1D_QUICK_PAGE_BITS_MSG
*/

/*
** L1D_RESET_MSG
*/

/*
** L1D_RESOURCE_REQ_MSG
*/
typedef PACKED struct
{
    bool      ContinuousPilot;  /* 0 = gated pilot,  1= continous pilot */
    uint8     PilotGatingRate;  /* Valid for gated pilot */
} L1DResourceReqMsgT;

/*
** L1D_REVERSE_PILOT_CTL_MSG
*/
typedef PACKED struct
{
    bool    PilotEnable;        
    int8    RpchGating;         
} L1DReversePilotCtlMsgT;

/*
** L1D_REVERSE_POWER_SET_MSG
*/
/************************************************************
FchPwr20Ms[0] = FCH power level at eigth rate and 20ms
FchPwr20Ms[1] = FCH power level at quarter rate and 20ms
FchPwr20Ms[2] = FCH power level at half rate and 20ms
FchPwr20Ms[3] = FCH power level at full rate and 20ms

PilotRefLevel20Ms[0] = pilot ref level at eigth rate and 20ms
PilotRefLevel20Ms[1] = pilot ref level at quarter rate and 20ms
PilotRefLevel20Ms[2] = pilot ref level at half rate and 20ms
PilotRefLevel20Ms[3] = pilot ref level at full rate and 20ms
**************************************************************/         
 
typedef PACKED struct
{
    int8    FchPwr20Ms[L1D_MAX_RATE_ENTRY];
    int8    DcchPwr; 
    int8    PilotRefLevel20Ms[L1D_MAX_RATE_ENTRY]; 
}L1dPowerParsT;

typedef PACKED struct 
{
     SysSystemTimeT ActionTime;     /* System Time (20ms) takes place */
     uint8          PwrCntlStep;    /* Closed loop power control step size */
     bool           rpcIncl;        /* Flag to indicate the foll power info is incl */
     int8           ChPowerLevel;   /* Open Loop Tx power correction RC1&2 */
     int16          RPiCHPwr;       /* Open Loop Tx power correction RC3&4 */
     L1dPowerParsT  PwrPars;        /* RC3-6: parameters for code channel power calculation */
}L1dReversePowerSetMsgT;

/*
** L1D_RX_POWER_MSG
*/

/*
** L1D_SEARCH_PARMS_UPDATE_MSG
*/
typedef PACKED struct 
{
    uint16 ActWinSize;
    uint16 NeighWinSize;
    uint16 RemainWinSize;
} L1dSearchParmsUpdateMsgT; 

/*
** L1D_SEARCH_RSLT_MSG
*/

/*
** L1D_SERVICE_CONFIG_MSG
*/
typedef PACKED struct
{
    uint8              SchId;
    uint8              SchRc;
    bool               Coding;
} L1dSchChanConfigT;

typedef PACKED struct
{
    bool               FchCcIncl;
    SysFchFrameSizeT   FchFrameSize;
    uint8              ForFchRc;
    uint8              RevFchRc;
    bool               DcchCcIncl;
    SysDcchFrameSizeT  DcchFrameSize;
    uint8              ForDcchRc;
    uint8              RevDcchRc;
    bool               ForSchCcIncl;
    uint8              NumForSch;
    L1dSchChanConfigT  ForSchConfig[ L1D_MAX_FORWARD_SCH ];
    bool               RevSchCcIncl;
    uint8              NumRevSch;
    L1dSchChanConfigT  RevSchConfig[ L1D_MAX_REVERSE_SCH ];
} L1dServiceConfigT;


typedef PACKED struct
{
    L1dServiceConfigT   SrvcConfig; /* traffic channel service configuration */
    SysActionTimeT      ActTime;    /* Time for Service Configuration to take effect */
} L1dServiceConfigMsgT;

/*
** L1D_SERVICE_CONFIG_NN_MSG
*/
typedef PACKED struct
{
    bool            FpcIncl;
    uint8           FpcPriChan;         /* valid only if FpcIncl == 1 */
    uint8           FpcMode;            /* valid only if FpcIncl == 1 */
    bool            FpcOlpcFchIncl;
    uint8           FpcFchFer;          /* valid only if FpcOlpcFchIncl == 1 */
    uint8           FpcFchMinSetPt;        /* valid only if FpcOlpcFchIncl == 1 */
    uint8           FpcFchMaxSetPt;     /* valid only if FpcOlpcFchIncl == 1 */
    bool            FpcOlpcDcchIncl;         
    uint8           FpcDcchFer;         /* valid only if FpcOlpcDcchIncl == 1 */
    uint8           FpcDcchMinSetPt;    /* valid only if FpcOlpcDcchIncl == 1 */
    uint8           FpcDcchMaxSetPt;    /* valid only if FpcOlpcDcchIncl == 1 */
    bool            GatingRateIncl;      
    uint8           PilotGatingRate;    /* valid only if GatingRateIncl == 1 */
    SysActionTimeT  ActTime;
} L1DServiceConfigNnMsgT;

/*
** L1D_SETS_LIST_UPDATE_MSG
*/
typedef PACKED struct
{
  uint16    PilotPn;
} L1dActivePilotT;

typedef PACKED struct
{
    uint8           NumPilots;
    L1dActivePilotT Pilot[SYS_CP_MAX_ACTIVE_LIST_PILOTS];
} L1dActiveListT; 

typedef PACKED struct
{
    uint16  NumPilots;
    uint16  Pilot[SYS_CP_MAX_CANDIDATE_LIST_PILOTS];
} L1dCandListT; 

typedef PACKED struct
{
    uint16  PilotPn;
    uint8   Priority;
    uint8   SrchOffset; 
    uint8   SrchWinNghbr;
} L1dTrafficNghbrConfig;

typedef PACKED struct
{
    uint8   SearchMode;
    uint8   NumPilots;
    L1dTrafficNghbrConfig TrafficNghbrPilot[SYS_CP_MAX_NEIGHBOR_LIST_PILOTS];   
} L1dNghbrListT; 

typedef PACKED struct
{
  int16          PilotSeq;   /* Sequence number to match up with the measurement report */ 
  uint16         PilotInc;   /* Pilot PN sequence offset index increment, specifies interval between pilots in 64 chip units */
  L1dActiveListT Active;     /* active set list */
  L1dCandListT   Candidate;  /* candidate set list */
  L1dNghbrListT  Neighbor;   /* neighbor set list */
} L1dSetsListUpdateMsgT; 

/*
** L1D_PSW_SHARED_BUFFER_READY_MSG
*/
typedef PACKED struct
{
    L1dMsgIdT    MsgId;      /* Msg ID of msg placed into large message buffer */
    void         *DataPtr;   /* pointer to buffer containing message */
} L1dPswSharedBufferReadyMsgT;


/*
** L1D_SLEEP_MSG
*/
typedef PACKED struct
{
   uint32   SystemTime;      /* Next wake system time.  */
   int16         PiPosition;      /* Sleep mode = Qpch1, Qpch2, or CCI indicator position*/
   L1dSleepModeT SleepMode;
} L1dSleepMsgT;
/*
** L1D_WAKEUP_MSG aka Immediate Wake 
*/
/* Null Message body */
/*
** L1D_SLOTTED_PAGING_DISABLE_MSG
*/
/* Null Message body */
/*
** L1D Search Complete Message
*/
/* Null Message Body */

/*
** L1D_SLOTTED_MODE_PARAMS_MSG aka Enable message
*/
typedef PACKED struct
{
    uint16   PageSlot;           /* assigned slot to monitor         */
    uint8    SlotCycleIndex;     /* selected slot cycle index        */
    bool     QpchEnabled;       /* Quick Paging available           */
    uint8    QpageCh;            /* Quick Paging channel number      */
    bool     QpchRate;           /* QPch rate False = 4800bps : True = 9600bps */
} L1dSlottedModeParamsMsgT;

/*
** L1D_QPCH_CCI_DECODE_REQ_MSG aka Prep DSPM forIdle handoff
*/
/* Null Message body */
/*
** L1D_SYNC_ACQ_START_MSG
*/

/*
** L1D_TIMING_CHANGE_MSG
*/
typedef PACKED struct
{
    uint8   State[L1D_LONG_CODE_STATE_SIZE];
} L1dLongCodeStateT;

typedef PACKED struct
{
    uint32              ActFrameNum;    /* system time search is to take effect */
    uint16              PilotPn;        /* serving base station pilot PN squence */
    L1dLongCodeStateT   LCState;        /* system long code timing value */
    uint8               LpSec;          /* #of leap sec since start of System Time */
    int8                LtmOff;         /* Offset of local time from system time */
    bool                DayLt;          /* DAYLT, Daylight savings indicator */
    uint8               SysTime[5];     /* system time |mclee were to put define
                                         * for #of array elements ???*/
} L1dTimingChangeMsgT;

/*
** L1D_TRAFFIC_CHAN_HARD_HANDOFF_MSG
*/
typedef PACKED struct
{
    SysActionTimeT   ActTime;         	/* system time traffic start is to take effect */
    bool             FreqIncl;         /* flag indicating if new channel */
    SysCdmaBandT     BandClass;        /* band  */
    uint16           CdmaFreq;         /* frequency channel */
    bool             LcMaskIncl;			/* flag indicating new long code mask included*/
    uint8            LcMask[L1D_LONG_CODE_MASK_SIZE]; /* long code mask */
    bool             FrameOffsetIncl;	/* flag indicating if new frame offset included*/
    uint8            FrameOffset;      /* Fwd & Rev Traffic Channel frames are delayed  */
                                       /* FrameOffset x 1.25ms relative to system timing */
    bool             PwrCntlStepIncl;  /* flag indicating if PwrCntlStep included */
    uint8            PwrCntlStep;      /* PC step size, 0=1db, 1=0.5db, 2=0.25db */
    uint8            NumActSets;       /* number of active set pilot PNs */
    L1dTcActiveSetT  ActSetList[SYS_CP_MAX_NUM_TC_ACTIVE_PILOTS];   /* active set list */
    uint8        		NumPreambleFrames;/* Number of preamble frames (0xFF = continuous) */
    bool 				SrchWinIncl;		/* flag indication if search window is included */
    uint8				SrchWinA;			/* search window size for active and candidate set */
    uint8				SrchWinN;			/* search window size for neighbor set */
    uint8				SrchWinR;			/* search window size for remaining set */
   /* The following parameters are valid only for Fwd RC > 2 */
    uint8            ChInd;            /* Channel Indicator: 1=FCH, 2=DCCH, 3=BOTH */
    bool             FpcSubChanGainIncl;
    uint8            FpcSubChanGain;   /* forward power control subchannel relative gain */
    bool             UsePcTime;        /* use the power control time */
    SysActionTimeT	PCTime;         	/* system time power control is to take effect */
    bool             RevFchGatingMode;	/* Reverse eighth gating mode indicator */
                                       /* see 2.1.2.3.2, IS2000.2-A */
    uint8            RevPwrCntlDelay;  /* Reverse power control delay */                           
    bool             ScrIncluded;      /* If the service configuration messages will be sent */
} L1dTrafficChanHardHandoffMsgT;


/* SNRTODO To be deleted */
/*
** L1D_TRAFFIC_CHAN_PWR_STEP_MSG
*/
typedef PACKED struct
{
    uint8   PwrCntlStep;
} L1dTrafficChanPwrStepMsgT;


/*
** L1D_TRAFFIC_CHAN_SOFT_HANDOFF_MSG
*/
typedef PACKED struct
{
   SysActionTimeT    ActTime;         /* system time search is to take effect */
   uint8             NumActSets;      /* number of active set piolt PNs */
   uint8             NumForSup;
   L1dTcActiveSetT   ActSetList[SYS_CP_MAX_NUM_TC_ACTIVE_PILOTS]; /* active set list */
   bool              SrchWinIncl;      /* flag indication if search window is included */
   uint8             SrchWinA;         /* search window size for active and candidate set */
   uint8             SrchWinN;         /* search window size for neighbor set */
   uint8             SrchWinR;         /* search window size for remaining set */
   bool              FpcSubChanGainIncl;
   uint8             FpcSubChanGain;   /* forward power control subchannel relative gain */
   bool              UsePcTime;        /* use the power control time */
   SysActionTimeT    PCTime;           /* system time power control is to take effect */
   bool              RevFchGatingMode; /* Reverse eighth gating mode indicator */
                                       /* see 2.1.2.3.2, IS2000.2-A */
   uint8             RevPwrCntlDelay;  /* Reverse power control delay */                           
} L1dTrafficChanSoftHandoffMsgT;

/*
** L1D_TRAFFIC_CHAN_START_MSG
*/
typedef PACKED struct 
{
    uint8   CurrSetPt;          /* Set to FPC_FCH_INIT_SETPT or FPC_DCCH_INIT_SETPT  */
    uint8   Fer;                /* See Table 3.7.3.3.2.25-2, IS2000.5A */
    uint8   MinSetPt;           /* Minimum outer loop Eb/Nt setpoint */
    uint8   MaxSetPt;           /* Maximum outer loop Eb/Nt setpoint */
} L1dFpcParamsT; 

typedef PACKED struct
{
    bool                FreqIncl;                                   /* flag indicating if new channel */
    SysCdmaBandT        BandClass;                                  /* band  */
    uint16              CdmaFreq;                                   /* frequency channel */
    uint8               LcMask[L1D_LONG_CODE_MASK_SIZE];            
    uint8               FrameOffset;                                /* Fwd & Rev Traffic Channel frames are delayed  */
                                                                    /* FrameOffset x 1.25ms relative to system timing */
    uint8               ForRc;                                      /* RC for FCH and DCCH, see Table 3.7.2.3.2.21-3 */
    uint8               RevRc;                                      /* RC for FCH and DCCH, see Table 3.7.2.3.2.21-3 */
    uint8               PwrCntlStep;                                /* PC step size, 0=1db, 1=0.5db, 2=0.25db */
    bool                Fch5MsFrames;                               /* indicates support for 5ms frames */
    bool                Dcch5MsFrames;                              /* indicates support for 5ms frames */
    uint8               NumActSets;                                 /* number of active set pilot PNs */
    L1dTcActiveSetT     ActSetList[SYS_CP_MAX_NUM_TC_ACTIVE_PILOTS];   /* active set list */
    uint8               ChInd;                                      /* Channel Indicator: 1=FCH, 2=DCCH, 3=BOTH */
                                                                    /* applies to all active set members */
   /* The following parameters are valid only for Fwd RC > 2 */
    uint8               FpcMode;                                    /* Fwd pwr control mode  */
    uint8               FpcModeNoSch;
    uint8               FpcModeSch;
    bool                FpcPriChanIncl;
    uint8               FpcPriChan;                                 /* pwr control sub-channel indicator, 0=FCH, 1=DCCH */
    uint8               FpcSubChanGain;
    L1dFpcParamsT       FpcFch;                                     /* fch field valid only if ch_ind =1 or 3 */
    L1dFpcParamsT       FpcDcch;                                    /* dcch field valid only if ch_ind=2 or 3*/
   /* The following parameters are valid only for Rev RC > 2 */
    bool                RevFchGatingMode;                           /* Reverse eighth gating mode indicator
                                                                       see 2.1.2.3.2, IS2000.2-A */
    uint8               RevPwrCntlDelay;                             
    uint8               PilotGatingUseRate;                         /* 0=Not Gated, 1=Gated */
    uint8               PilotGatingRate;                            /* Only valid if Reverse Pilot is gated, see
                                                                       Table 3.7.3.3.2.34-2, IS2000.5 for rate information */
} L1dTrafficChanStartMsgT;

/*
** L1D_TRAFFIC_CHAN_STOP_MSG
*/

/*
** L1D_TRAFFIC_INIT_COMPLETE_MSG
*/

/*
** L1D_TRANSMITTER_CONTROL_MSG
*/
typedef enum
{
    L1D_TX_OFF = 0,      /* turns off transmitter   */
    L1D_TX_ON_IMMEDIATE, /* turns transmitter on immediately */
    L1D_TX_ON_CF_RULES   /* reenables transmitter according to     */
                         /*   IS2000.5-A 2.6.6.2.8.3.3 after visit */
                         /*   to candidate frequency               */
} L1dTxControlT;


typedef PACKED struct 
{
    L1dTxControlT TxControl;
} L1dTransmitterControlMsgT; 


/*
** L1D_FOR_POWER_CONTROL_PARAMS_MSG
*/
typedef PACKED struct
{
    uint8           Fer;
    uint8           MinSetPt;
    uint8           MaxSetPt;
} FpcOlPcParamsT;


typedef PACKED struct
{
    uint8           SchId;
    FpcOlPcParamsT  Sch;
} FpcOlPcSchParamsT;


typedef PACKED struct
{
    SysActionTimeT    ActTime;
    bool              FpcIncl;
    bool              FpcPriChan;
    uint8             FpcMode;
    bool              FpcOlpcFchIncl;
    FpcOlPcParamsT    Fch;
    bool              FpcOlpcDcchIncl;
    FpcOlPcParamsT    Dcch;
    bool              FpcSecChan;
    bool              FpcThreshIncl;
    uint8             FpcSetPtThresh;
    bool              FpcThreshSchIncl;
    uint8             FpcSetPtThreshSch;
    uint8             NumSup;
    FpcOlPcSchParamsT Sch[2];
}L1dForPowerControlParamMsgT;


/*
** L1D FORWARD POWER CONTROL SPY STRUCTURES
*/
typedef PACKED struct
{
    uint8           ForRc;
    uint8           FpcMode;
    uint8           FpcPriChan;
    uint8           ChInd;
    L1dFpcParamsT   Fch;
    L1dFpcParamsT   Dcch;
    uint8           FpcSubChanGain;
} L1dFpcTrafChanStartMsgSpyT;

typedef PACKED struct
{
    SysActionTimeT  ActTime;
    bool            FchCcIncl;
    uint8           ForFchRc;
    uint8           RevFchRc;
    bool            DcchCcIncl;
    uint8           ForDcchRc;
    uint8           RevDcchRc;
} L1dFpcServiceConfigMsgSpyT;

typedef PACKED struct
{
    SysActionTimeT  ActTime;
    bool            FpcIncl;
    uint8           FpcPriChan;
    uint8           FpcMode;
    bool            OlpcFchIncl;
    uint8           FchFer;
    uint8           FchMinSetPt;
    uint8           FchMaxSetPt;
    bool            OlpcDcchIncl;
    uint8           DcchFer;
    uint8           DcchMinSetPt;
    uint8           DcchMaxSetPt;
} L1dFpcConfigNnMsgSpyT;

typedef PACKED struct
{
  uint8                 FpcPriChans;
  uint8                 PilotGatingUseRate;
  bool                  FpcIncl;
  uint8                 FpcModeSch;
  uint8                 FpcSecChan;
  uint8                 FpcSchInitSetptOp;
  bool                  FpcThreshSchIncl;
  uint8                 FpcSetptThreshSch;
  uint8                 NumSup;
  uint8                 FpcSchInitSetpt[ L1D_MAX_FORWARD_SCH ];
  L1dFpcOlPcSCHParamsT  Sch[ L1D_MAX_FORWARD_SCH ];
} L1dFschMsgSpyT; /* same as L1dFschMsgT but without the NumForSch and ForSchRec */

/*
** L1D_TST_DIAG_MONITOR_MSG
*/
/* Diagnostic Paramters Message */
typedef PACKED struct
{
   ExeRspMsgT  RspInfo;
} L1dTstDiagMonitorMsgT;

/* Pilot Sets info used in the next typedef */
typedef PACKED struct
{
   uint16 PNoffset;
   uint16 PilotEcByIo;
} L1dTstDiagMonitorRspSetInfoT;

/* Diagnostic Monitor Response */
typedef PACKED struct
{
   int16  TxPower;
   int16  RxAgcAntPower;
   uint8  ActiveSetNumber;
   uint8  CandidateSetNumber;
   uint8  NeighborSetNumber;
   L1dTstDiagMonitorRspSetInfoT  ActiveSetInfo[SYS_CP_MAX_ACTIVE_LIST_PILOTS];
   L1dTstDiagMonitorRspSetInfoT  CandidateSetInfo[SYS_MAX_CANDIDATE_LIST_PILOTS];
   L1dTstDiagMonitorRspSetInfoT  NeighborSetInfo[SYS_CP_MAX_NEIGHBOR_LIST_PILOTS];
   uint16 CodeChanFch[SYS_CP_MAX_ACTIVE_LIST_PILOTS];    /* Code channel index (Walsh) for FCH, valid if ChInd == 1 or 3 */
   uint16 CodeChanDcch[SYS_CP_MAX_ACTIVE_LIST_PILOTS];   /* Code channel index for DCCH, valid if ChInd == 2 or 3 */
   uint16 ActSetSrchWin;
   uint16 NghbrSetSrchWin;
   uint16 RemSetSrchWin;
} L1dTstDiagMonitorRspT;

/*
** L1D_TST_GET_PHONE_STATE_MSG
*/
/* Get Phone State Message */
typedef PACKED struct
{
   ExeRspMsgT  RspInfo;
} L1dTstMdGetPhoneStateMsgT;

/* Get Phone State Response */
typedef PACKED struct
{
   uint8       CurrentMode;
   uint8       PhoneType;
   uint16      CurrentChannel;
   uint16      PARangeVal;
   uint16      CurrentAudioPath;
   uint16      TxState;
   uint16      SynthLock;
   uint16      LNARange;
   uint16      FmTxMuteState;
   uint16      FmRxMuteState;
   uint16      CompressorState;
   uint16      ExpandorState;
   uint16      SatColorCodeState;
   uint16      CurrentFmState;
   uint16      CurrentVocoderRate;
   uint16      CurrentTxPunctureRate;
   uint16      CurrentCDMAState;
   uint32      Gpio_0;
   uint32      Gpio_1;
   uint32      Gpio_i_o_0;
   uint32      Gpio_i_o_1;
   uint16      CurrentKeypress;
   uint8       Pdm_Bal[L1D_PDM_BAL_SIZE];
   uint8       Adc_Bal[L1D_ADC_BAL_SIZE];
} L1dTstMdGetPhoneStateRspT;

/* A. Kulkarni. 02/12/02. */
typedef PACKED struct
{
	uint16      FmTxMuteState;
    uint16      FmRxMuteState;
}L1dGetMmiInfoRspMsgT;

typedef PACKED struct
{
	uint16		CompressorState;
	uint16		ExpandorState;
}L1dGetDspvInfoRspMsgT;


/* Rate Test enum for Tx Rate Test Message support */
typedef enum
{
   L1D_TEST_TX_OFF           = 0,
   L1D_TEST_TX_ACCESS        = 1,
   L1D_TEST_TX_TRAFF_FULL    = 2,
   L1D_TEST_TX_TRAFF_HALF    = 3,
   L1D_TEST_TX_TRAFF_QUARTER = 4,
   L1D_TEST_TX_TRAFF_EIGHTH  = 5
} L1dTstTxCdmaRateT;

/* Tx Rate Test Message */
typedef PACKED struct
{
  L1dTstTxCdmaRateT  Rate;
} L1dTstTxCdmaEtsMsgT;


/*
** L1D_IDLE_DIFF_FREQ_SEARCH_MSG
*/

/*
** L1D_RSCH_MSG
*/
typedef PACKED struct
{
  uint8             RevSchId;
  uint8             RevSchDuration;
  SysActionTimeT    RevSchActionTime;
  uint8             RevSchRate;
  uint8             RevWalshId;
  int8              PilotReferenceLevel;
  int8              RschPower;
} L1dRschRecT;

typedef PACKED struct
{
  uint8             RevSchDtxDuration;
  uint8             NumRevSch;
  L1dRschRecT       RevSchRec[L1D_MAX_REVERSE_SCH];
} L1dRschMsgT;

/* L1D_AMPS_HANDOFF_MSG */
typedef struct 
{
	SysActionTimeT	ActionTime;	/* action time for switching from CDMA to AMPS */
} L1dAmpsHandoffMsgT;

/* Amala K. 03/01/02 - AFLT */

/*
** L1D_POS_AFLT_SEARCH_MSG
*/

/**----------------------------------------------------------------**
  L1dPosAfltSearchMsgT
  This message is sent by PSW when PSW receives a Position 
  Determination Data Message (Provide BS Almanac) message from the BS. 
  L1D will return a PSW_POS_AFLT_MEASUREMENTS_MSG as soon as possible.
 **----------------------------------------------------------------**/

typedef PACKED struct
{
  /* individual PILOT_PN from BS ALMANAC */
  uint16 PilotPN;
  /* individual TIME_CORRECTION from BS ALMANAC */
  int16  TimeCorrect;
} AFLTPilotT;

typedef PACKED struct
{
  uint16 SeqNum;	
  /* REF_PN from BS ALMANAC */
  uint16 RefPN;
  /* TIME_CRRCTION_REF from BS ALMANAC */
  int16  TimeCorrectRef;
  /* NUM_PILOTS_P from BS ALMANAC */
  uint8  NumPilots;
  /* data valid */
  bool   Valid;
  /* array of PILOT_PNs from BS ALMANAC */
  AFLTPilotT PosAfltConfig[SYS_CP_MAX_AFLT_LIST_PILOTS];
} L1dPosAfltSearchMsgT;


/*
** L1D_POS_AFLT_ABORT_MSG
*/

/**----------------------------------------------------------------**
  L1dPosAfltAbortT
  This message is sent by PSW, after PSW sent a 
  L1D_POS_AFLT_SEARCH_MSG, to request L1D to abort AFLT pilot
  measurements for the given sequence number.

  L1D should respond with a PSW_POS_AFLT_ACK_ABORT_MSG to acknowledge
  that the abort occurred.
 **----------------------------------------------------------------**/

typedef PACKED struct
{
  uint16 	SeqNum;
} L1dPosAfltAbortMsgT;

/* Amala K. - AFLT END */

/*
** L1D_TST_DEBUG_FILTER_MSG
*/
typedef PACKED struct
{
  uint8  RxPowerMsgFilterFlag;
  uint8  SearchRsltMsgFilterFlag;
} L1dTstDebugFilterMsgT;

/*********** GPS Messages***********/

/*
** L1D_GPS_INIT_GPS_MSG
*/
typedef PACKED struct
{
   bool IsTraffic;
} L1dGpsInitGpsMsgT;
 

/*
** L1D_GPS_CONT_CDMA_REQ_MSG
*/

/*
** L1D_GPS_N1_GOOD_FRAMES_FOUND_MSG,
*/

/*
** L1D_GPS_PSMM_SENT_MSG
*/

/*
** L1D_GPS_SWITCHING_HANDOFF_COMPLETE_MSG
*/

/*
** L1D_GPS_GOOD_FRAMES_TMR_EXP_MSG
*/

/*
** L1D_GPS_XMIT_TMR_EXP_MSG
*/

/*
** L1D_GPS_HANDOFF_IND_TMR_EXP_MSG
*/

/*
** L1D_GPS_END_GPS_REQ_MSG
*/
typedef PACKED struct
{
  bool IsTraffic; 
} L1dGpsEndGpsReqMsgT;


/*
** L1D_DSPM_TIMING_RSP_MSG
*/
/*********** End GPS Messages***********/

/*
** L1D_TST_IDLE_DIFF_FREQ_SRCH_MODE_MSG
*/
typedef PACKED struct
{
   bool              Disable;  /* TRUE=disable, FALSE=enable */
}L1dIdleDiffFreqSrchModeMsgT;

typedef PACKED struct
{
   bool              Access;   /* TRUE  = Going into Access / Start
                                * FALSE = Finish Access / Stop
                                */
}L1dAccessInProgressMsgT;

/*
** L1D_VCXO_COMP_MSG
*/
typedef PACKED struct
{
   bool VcxoCompEnable;  /* TRUE=DSPm Vcxo Compensation Mode enabled */
   uint16 AvePilotPwrReport;
}L1dVcxoCompMsgT;
        
/*
** L1D_TST_IGNORE_RESET_MSG
*/
typedef PACKED struct
{
   bool              Disable;  /* TRUE=ignore reset */
}L1dTstIgnoreResetMsgT;

typedef PACKED struct
{
   uint8 pilotRefLevel;
   int8  rschPower;
} L1dRschPowerSetMsgT;
/*========================================================================================
==========================================================================================
===================== The following sections need to be resolved. ========================
==========================================================================================
========================================================================================*/

/*
** These are the old Slotted Page Messages.  We have changed the names in CBP4
** to L1dSlottedPagingEnableMsgT and L1dSlottedPagingSleepMsgT.
*/
#define L1dSPageEnableMsgT  L1dSlottedPagingEnableMsgT
#define L1dSPageSleepMsgT   L1dSlottedPagingSleepMsgT

/*
**  Old L1D_IDLE_PRIORITY_SET_MSG
*/

typedef PACKED struct
{
   bool           FreqInc;             /* Frequency Included */
   SysCdmaBandT   CdmaBand;            /* band (1st relase always PCS) */
   uint16         FreqChan;            /* frequency channel */
   uint8          NumPilotPn;          /* Number of Pilot PN's */
   uint16         PilotPn [SYS_CP_MAX_IDLE_PRIORITY_PILOTS];
} L1dIdlePrioritySetMsgT;


/*
**  Old L1D_PUF_PROBE_SEND_MSG.  This feature isn't supported in CBP4.
*/

typedef PACKED struct
{
   uint8          CdmaBand;           /* band (1st relase always PCS) */
   uint16         FreqChan;           /* frequency channel */
   uint8          SetupSize;          /* number of power control groups to send */
                                      /* channel preamble for prior to pulse */
   uint8          PulseSize;          /* number of power control groups to transmit */
                                      /* pulse for */
   SysActionTimeT ActTime;            /* Action Time */
   uint8          PowerOffset;        /* PUF probe power level above mean input */
                                      /* power level */
}L1dPufProbeSendMsgT;

/*
**  Old L1D_OOSA_MSG
*/
typedef PACKED struct
{
   uint32   SleepDuration;  /* Sleep duration in 1/10 seconds */
} L1dOosaMsgT;

/*
**  Old L1D_SET_T_ADD_MSG
*/
typedef PACKED struct
{
   uint8 TAdd;   
} L1dSetTAddMsgT;

typedef PACKED struct
{
	uint8			phonetype;
	uint8			AMPSsatcolorcode;
	uint8			curAMPSstate;
	uint8			curCDMAstate;
	uint8			curVocoderRate;
}L1dGetPSInfoRspMsgT;

/*
**  L1D_FPC_CAL_FACTOR_MSG
*/
typedef enum
{
   WRITE_ACTION = 0,
   READ_ACTION
} L1dFpcCalActionT;

typedef PACKED struct
{
   ExeRspMsgT        RespInfo;
   L1dFpcCalActionT  Action;  /* 0=write, 1=read */
   uint8             RC;      /* Radio Configuration , 0 - 4 */
   int16             CalcFactors[8];
} L1dFpcCalFactorMsgT;

typedef PACKED struct
{
   int16             CalcFactors[8];
} L1dFpcCalFactorRespMsgT;

/*
** L1D_RX_POWER_REQ_MSG
*/
typedef PACKED struct
{
   ExeRspMsgT        RespInfo;
}L1dRxPowerReqMsgT;

typedef PACKED struct
{
   int16             RxPower;
}L1dRxPowerRespMsgT;

/*
**L1D_GET_DEBUG_SCRN_INFO_MSG,
*/
typedef PACKED struct 
{
   ExeRspMsgT  RspInfo;                    /* Response routing information */
}L1dGetDebugScrnInfoRequestT;

typedef PACKED struct
{
   int16 txpower;
   int16 rxpower;
}L1dGetDebugScrnInfoResponseT;

typedef PACKED struct
{
   uint16 AltPhase;
   uint16 OverReachPilotPN;
} L1dAltPilotAcquireMsgT;

/*
** L1D_SYNC_CHAN_MSG_RECD_MSG
*/

/*
** L1D_INIT_MSG
*/

/*
** L1D_SEND_AMPS_RSSI_MSG
*/
#if (defined SYS_OPTION_AMPS)
typedef L1dAmpsRssiRspT L1dSendAmpsRssiMsgT;
#endif


/*----------------------------------------------------------------------------
* L1d global APIs
*----------------------------------------------------------------------------*/

bool L1dIsDiffFreqSearchActive (void);
void L1dGpsSyncPulseStartCb(uint32);
void L1dGpsSyncPulseEndCb(uint32);
void L1dGpsGoodFramesTmrExpCb(uint32);
void L1dGpsN2XmitFrmExpCb(uint32);
void L1dGpsN3HandoffIndTmrExpCb(uint32);



#endif

#ifndef _LMDAPI_H_
#define _LMDAPI_H_



/*----------------------------------------------------------------------------
 Include Files
----------------------------------------------------------------------------*/

#include "dbmapi.h"
#include "exeapi.h"
#include "ipcapi.h"
#include "sysdefs.h"
#include "sysapi.h"
#include "exeapi.h"

/*----------------------------------------------------------------------------
* EXE Interfaces - Definition of Signals and Mailboxes
*----------------------------------------------------------------------------*/
#define LMD_FORWARD_CHAN_SIG            EXE_SIGNAL_1
#define LMD_REVERSE_CHAN_SIG            EXE_SIGNAL_2
#define LMD_STARTUP_SIGNAL              EXE_SIGNAL_3
#define LMD_20MS_STROBE_SIG             EXE_SIGNAL_4
#define LMD_FWD_CHAN_SIGNAL_SUP         EXE_SIGNAL_5
#define LMD_START_SERVICE_CONFIG_SIG    EXE_SIGNAL_6
#define LMD_START_NN_SERVICE_CONFIG_SIG EXE_SIGNAL_7
#define LMD_SERVICE_OPTION_CONTROL_SIG  EXE_SIGNAL_8
#define LMD_REVERSE_SIGNALING_TXED_SIG  EXE_SIGNAL_9
#define LMD_REVERSE_SCH0_ON_SIG         EXE_SIGNAL_10 /* not in use*/
#define LMD_REVERSE_SCH0_STOP_SIG       EXE_SIGNAL_11 

#if (SYS_MAX_RSCH == 2)
#define LMD_REVERSE_SCH1_ON_SIG         EXE_SIGNAL_12
#define LMD_REVERSE_SCH1_STOP_SIG       EXE_SIGNAL_13
#endif /* end of SYS_MAX_RSCH*/ 

#define LMD_FORWARD_SCH0_STOP_SIG       EXE_SIGNAL_14 /* not in use*/
#define LMD_FORWARD_SCH0_ON_SIG         EXE_SIGNAL_15 /* not in use*/

#if (SYS_MAX_FSCH == 2)
#define LMD_FORWARD_SCH1_STOP_SIG       EXE_SIGNAL_16
#define LMD_FORWARD_SCH1_ON_SIG         EXE_SIGNAL_17
#endif /* end of SYS_MAX_FSCH */

#define LMD_ACCESS_PROBE_COMPLETE_SIG   EXE_SIGNAL_18
#define LMD_RESOURCE_UPDATE_SIG         EXE_SIGNAL_19


#define LMD_SIG_MAILBOX          EXE_MAILBOX_1_ID
#define LMD_SIG_MAILBOX_EVENT    EXE_MESSAGE_MBOX_1 
#define LMD_CMD_MAILBOX          EXE_MAILBOX_1_ID
#define LMD_CMD_MAILBOX_EVENT    EXE_MESSAGE_MBOX_1 

#define LMD_PT_MAILBOX           EXE_MAILBOX_2_ID
#define LMD_DS_MAILBOX_1         EXE_MAILBOX_3_ID
#define LMD_DS_MAILBOX_2         EXE_MAILBOX_4_ID 
#define LMD_SCH_DS_MAILBOX       EXE_MAILBOX_5_ID
#define LMD_FCH_DS_MAILBOX       LMD_DS_MAILBOX_1
#define LMD_DCCH_DS_MAILBOX      LMD_DS_MAILBOX_2

#define LMD_MAX_SIZE_FRAME   162

/* array size for long code mask */
#define LMD_LONG_CODE_MASK_SIZE     6

/* Voice Memo prefetch buffer size = (max size frame + the rate word) X 8.
 *
 * LMD defines a static buffer SpchData[] where it prefetches the speech
 * data during voice memo or answering machine playback.  There are
 * conditions where DBM is late in doing the block reads and LMD would
 * be starving of data.  By making the buffer size eight times bigger than
 * the maximun speech packet size we guarantee that LMD will always have
 * at least one packet in prefetch buffer.
 */
#define  LMD_VMEMO_PREFETCH_BUFF_SIZE  ((LMD_MAX_SIZE_FRAME+2)*8)
#define  LMD_VMEMO_PREFETCH_THRESHOLD  (LMD_MAX_SIZE_FRAME+2)

/*---------------------------------------------------------------
*  Message IDs for signals and commands sent to LMD
*---------------------------------------------------------------*/
/* Maximum number of SCH */

#ifdef SYS_OPTION_SCH_RAM_INUSE
#define  MAX_PDU_NUM         8  /* now 8 for one SCH */
#else
#define  MAX_PDU_NUM         1  /* now 8 for one SCH */
#endif

/*
** Messages of LmdCmdMsgIdT type
** should be sent to the LMD_CMD_MAILBOX.
*/
typedef enum
{
    /* PSW's commands for call processing*/
    LMD_ACCESS_PROBE_ABORT_MSG = 0,
    LMD_TRAFFIC_CHAN_START_MSG,
    LMD_TRAFFIC_CHAN_STOP_MSG,
    LMD_RESET_MSG,

    /* Service Configuration */
    LMD_SERVICE_CONFIGURATION_MSG = 0x10,
    LMD_SERVICE_CONFIG_NN_MSG,
    LMD_SERVICE_OPTION_CONTROL_MSG,
    LMD_CONVERSATION_SUBSTATE_CONN_MSG,
    LMD_CONVERSATION_SUBSTATE_DISC_MSG,
    LMD_LOOPBACK_SERVICE_OPTION_MSG,
    LMD_AUDIO_SSO_CONNECT_RSP_MSG,
    LMD_AUDIO_SSO_DISCONN_RSP_MSG,

    /* Markov/TDSO support */
    LMD_CLEAR_MARKOV_STATS_MSG = 0x20,     
    LMD_MARKOV_SERVICE_OPTION_MSG,       

    /* Power control message */
    LMD_PWR_CTL_PARMS_MSG = 0x30,
 
    /* Voice Activity */
    /* Debug */

    /* For Data services. */
    LMD_FCH_REV_DATA_MSG = 0x50,
    LMD_DCCH_REV_DATA_MSG,
    LMD_SCH_REV_DATA_MSG,
    LMD_SCCH_REV_DATA_MSG,
    /* Data services:  Fwd SCH and SCCH Ack Response Msg */
    LMD_SCH_FWD_DATA_RSP_MSG,
    LMD_SCCH_FWD_DATA_RSP_MSG,

   /* The TDSO TX data msg */ 
    LMD_FCH_REV_TDSO_DATA_MSG = 0x60,
    LMD_DCCH_REV_TDSO_DATA_MSG, 
    LMD_SCH_REV_TDSO_DATA_MSG,  
    LMD_SCCH_REV_TDSO_DATA_MSG, 
                   
    /* SCH setup messages */
    LMD_FSCH_MSG = 0x70,
    LMD_RSCH_MSG,
    LMD_FSCH_BURST_END_MSG,
    LMD_RSCH_STOP_MSG,
	LMD_FSCH_PDU_ORDER_CTRL_MSG,

    /* Reverse channel messages */
    LMD_REVERSE_ACCESS_SIG_MSG = 0x80,
    LMD_EACH_SIG_MSG,
    LMD_REVERSE_TRAFFIC_SIG_MSG,
    LMD_TRANSMITTER_STATUS_MSG,
    LMD_COUNTER_SUSPEND_MSG,

    /* For Diagnostic Monitor support */
    /* Voice quality muting */


    /* Resource Control: FCH and DCCH only */
    LMD_RESOURCE_UPDATE_MSG = 0xC0,
    
    /* Sim Message */
    LMD_SIM_MSG = 0xF0,

    /* Statistics counter retrieve and set message */
    LMD_GET_STAT_CNT_MSG,
    LMD_SET_STAT_CNT_MSG,
    LMD_RESET_STAT_CNT_MSG,

	/* Debug Screen Info message */
    LMD_GET_DEBUG_SCRN_INFO_MSG,

    /* Speech Routing Disable Message */
    LMD_SPEECH_DISABLE_MSG,
    
    /* Instruct LMD to stop requesting data from RLP but continue
     * submitting R-Sch Tx data to DSPm.
     */
    LMD_RLP_STOP_REQ_MSG,
    /* Voice Activity Testing msg..*/
    LMD_VOICE_ACT_TARGET_RATE_MSG,
    LMD_VOICE_ACTIVITY_TEST_OFF_MSG,

    LMD_CLEAR_FWD_SCH_DTX_STATS_MSG,
    
    LMD_NUM_CMD_MSG_IDS
} LmdCmdMsgIdT;

/*
** Messages of LmdPTMsgIdT type
** should be sent to the LMD_PT_MAILBOX.
*/
typedef enum
{
   LMD_TX_SPCH_MSG = 0x300,
   LMD_TX_LOOPBACK_MSG,
   LMD_TX_MARKOV_MSG,
   LMD_TX_TDSO_MSG
} LmdPTMsgIdT;

/*---------------------------------------------------------------
*  Basic Types
*---------------------------------------------------------------*/

/*
** The Logic To Physical Mapping (LPM) table is used in
** the service configuration messages.
*/
#define MAX_LPM_ENTRIES 15

typedef PACKED struct
{
    uint8    SrId;
    uint8    LogicalResource;
    uint8    PhysicalResource;
    bool     ForwardFlag;
    bool     ReverseFlag;
    uint8    Priority;
} LmdLpmT;


/*---------------------------------------------------------------
*  Message Definitions
*       There is a section for each defined message type.  If
*       there is no MsgT shown, there is no data associated with
*       the message.
*---------------------------------------------------------------*/

/* 
** LMD_REVERSE_ACCESS_SIG_MSG
*/
typedef PACKED struct
{       
    uint8   MsgLength;  /* Length of the message in bytes */
    uint8   MsgData[1]; /* Starting address of the message            */
} LmdReverseAccessSigMsgT;

/* 
** LMD_EACH_SIG_MSG
*/
typedef PACKED struct
{ 
    uint16  FrameNumBits;   /* number of bits per frame */
    uint16  MsgLength;      /* Message length in bytes */
    uint8   MsgData[1];     /* Starting address of the message */
} LmdReverseEACHSigMsgT;

/* 
** LMD_REVERSE_TRAFFIC_SIG_MSG
*/
typedef PACKED struct
{
    uint8   PrevInUse;  /* minimum of mob_p_rev and p_rev */
    uint16  MsgLength;  /* Length of message in bytes */
    uint8   MsgData[1];
} LmdReverseTrafficSigMsgT;

/*
** LMD_ACCESS_PROBE_ABORT_MSG
*/

/* 
** LMD_TRAFFIC_CHAN_START_MSG
*/
typedef PACKED struct
{
    uint8     LcMask[LMD_LONG_CODE_MASK_SIZE];    /* Long code Mask */
    uint8     ChInd;          /* Channel Indicator: 1=FCH, 2=DCCH, 3=BOTH */
    uint16    ForMuxOption;
    uint16    RevMuxOption;
    uint8     NumLpmEntries;
    LmdLpmT   LpmEntries[MAX_LPM_ENTRIES];
} LmdTrafficChanStartMsgT;

/* 
** LMD_SERVICE_CONFIGURATION_MSG
*/
#define     LMD_MAX_CON_REC     5
#define     LMD_MAX_SCH_NUM     2

typedef enum
{
    LMD_NO_TRAFFIC,
    LMD_PRIMARY_TRAFFIC,
    LMD_SECONDARY_TRAFFIC
} LmdTrafficTypeT;

typedef PACKED struct
{
    uint8           ConRef;         /* service option connection reference  */
    uint16          ServiceOption;  /* associated service option    */
    LmdTrafficTypeT ForTraffic;     /* forward traffic type */
    LmdTrafficTypeT RevTraffic;     /* reverse traffic type */
    uint8           SrId;
} LmdConnectionRecordT;

typedef PACKED struct
{
   uint8         SchId;
   bool          Coding; /*  True-- Turbo is On */
   uint16        SchMux; /* MuxOption of */
   uint8         SchRc;
}LmdSchChanConfigT;

typedef PACKED struct
{
    uint16              ForMuxOption;
    uint16              RevMuxOption;
    uint8               ForNumBits;
    uint8               RevNumBits;
    uint8               NumConRec;
    LmdConnectionRecordT    ConRec[LMD_MAX_CON_REC];        
    bool                FchCcIncl;            
    SysFchFrameSizeT    FchFrameSize;       /* valid only if FchCcIncl = 1 */
    uint8               FchForRc;
    uint8               FchRevRc;
    bool                DcchCcIncl;
    SysDcchFrameSizeT   DcchFrameSize;      /* valid only if dcch_cc_incl = 1 */
    uint8               DcchForRc;
    uint8               DcchRevRc;
    bool                ForSchCcIncl;
    uint8               NumForSch;          /* Number of Fwd SCH*/
    LmdSchChanConfigT   ForSchConfig[LMD_MAX_SCH_NUM];
    bool                RevSchCcIncl;
    uint8               NumRevSch;          /* number of Rev SCH*/
    LmdSchChanConfigT   RevSchConfig[LMD_MAX_SCH_NUM];
} LmdServiceConfigurationT;

typedef PACKED struct
{
    LmdServiceConfigurationT SrvcConfig;    /* traffic channel service configuration */
    SysActionTimeT           ActTime;       /* Time this message takes effect */
    bool                     SrvcConfigNonNegInc;
} LmdServiceConfigurationMsgT;

/* 
** LMD_SERVICE_CONFIG_NN_MSG
*/
typedef PACKED struct
{
    uint8           NumLpmEntries;
    LmdLpmT         LpmEntries[MAX_LPM_ENTRIES];
    SysActionTimeT  ActTime;         /* Time this message takes effect */
} LmdServiceConfigurationNnMsgT;

/* 
** LMD_SERVICE_OPTION_CONTROL_MSG
*/
typedef enum
{
   LMD_NULL_SERVICE_OPTION    = 0,
   LMD_SERVICE_OPTION_1       = 1,
   LMD_SERVICE_OPTION_2       = 2,
   LMD_SERVICE_OPTION_3       = 3,
   LMD_SERVICE_OPTION_4       = 4,
   LMD_SERVICE_OPTION_5       = 5,
   LMD_SERVICE_OPTION_6       = 6,
   LMD_SERVICE_OPTION_7       = 7,
   LMD_SERVICE_OPTION_9       = 9,
   LMD_SERVICE_OPTION_12      = 12,
   LMD_SERVICE_OPTION_13      = 13,
   LMD_SERVICE_OPTION_14      = 14,
   LMD_SERVICE_OPTION_15      = 15,
   LMD_SERVICE_OPTION_17      = 17,
   LMD_SERVICE_OPTION_18      = 18,
   LMD_SERVICE_OPTION_19      = 19,
   LMD_SERVICE_OPTION_22      = 22,
   LMD_SERVICE_OPTION_23      = 23,
   LMD_SERVICE_OPTION_24      = 24,
   LMD_SERVICE_OPTION_25      = 25,
   LMD_SERVICE_OPTION_32      = 32,
   LMD_SERVICE_OPTION_33      = 33,
   LMD_SERVICE_OPTION_35      = 35,
   LMD_SERVICE_OPTION_36      = 36,
   LMD_SERVICE_OPTION_54      = 54,
   LMD_SERVICE_OPTION_55      = 55,
   LMD_SERVICE_OPTION_68      = 68,
   LMD_SERVICE_OPTION_4100    = 4100,
   LMD_SERVICE_OPTION_4101    = 4101,
   LMD_SERVICE_OPTION_4103    = 4103,
   LMD_SERVICE_OPTION_32768   = 32768,
   LMD_SERVICE_OPTION_32798   = 32798,
   LMD_SERVICE_OPTION_32799   = 32799,
   LMD_SERVICE_OPTION_32858	  = 32858,
   LMD_SERVICE_OPTION_32859	  = 32859,
   LMD_DEFAULT_SERVICE_OPTION = 3
} LmdServiceOptionT;

typedef PACKED struct
{
   SysActionTimeT    ActTime;       /* system time (uint=20 ms) to  */
                                    /* send message                 */
   LmdServiceOptionT ServiceOption; /* service option number        */
   uint8             ConRef;        /* connection reference being controled */
   uint8             DataSize;      /* control data size in bytes   */
   uint8             Data[1];       /* control data                 */
} LmdServiceOptionControlMsgT;

/* 
** LMD_CONVERSATION_SUBSTATE_CONN_MSG
*/

/* 
** LMD_CONVERSATION_SUBSTATE_DISC_MSG
*/

/* 
** LMD_PWR_CTL_PARMS_MSG
*/
typedef PACKED struct
{ 
   uint8    ChanMonitor;           /* 0 = FCH, 1 = DCCH */
   bool     PwrThreshEnable;
   uint8    PwrRepThresh;
   bool     PwrPeriodEnable;
   uint8    PwrRepFrames;
   uint8    PwrRepDelay;
   bool     ResetFPC;
} LmdPwrCtlParmsMsgT;


typedef PACKED struct 
{
   uint16 ParmId;
   uint32 Value;
} LmdRevStatT;

typedef PACKED struct
{
   uint16      NumRecs;
   LmdRevStatT Rec[1];
} LmdRevStatSetMsgT;

typedef enum
{
   LMD_FULL_RATE     = 0x80,
   LMD_HALF_RATE     = 0x40,
   LMD_QUARTER_RATE  = 0x20,
   LMD_EIGHTH_RATE   = 0x10,
   LMD_NULL_RATE     = 0x00
} LmdRatesT;


/*
** LMD_FCH_REV_DATA_MSG
*/
/* wyf:  old */
typedef PACKED struct
{
   uint8            PrimarySrId;   /* primary sr id */
   SysRlpFrameRateT PrimaryRate;   /* frame rate for primary        */ 
   SysRlpFrameBufT  PrimaryData;   /* data buffer for primary       */
                                   /* ignored if PriRate = blank.   */
   uint8            SecondarySrId; /* secondary sr id */
   SysRlpFrameRateT SecondaryRate; /* frame rate for secondary      */
   SysRlpFrameBufT  SecondaryData; /* data buffer for secondary     */
                                   /* ignored if SecRate = blank.   */
} LmdFrameRspMsgT;

/* the SCH Rev data msg */
typedef PACKED struct
{
   uint8            PriSrId;      /* SR_ID for Primary Traffic     */
   SysRlpFrameRateT PriRate;      /* frame rate for primary        */ 
   SysRlpFrameBufT  PriData;  /* data buffer for primary       */
   uint8            SecSrId;      /* SR_ID for Secondary Traffic   */
   SysRlpFrameRateT SecRate;      /* frame rate for secondary      */
   SysRlpFrameBufT  SecData;  /* data buffer for secondary     */
} LmdDtchRevDsMsgT;

#define  LmdDtchRevDataMsgT      LmdDtchRevDsMsgT

/*
** LMD_SCH_REV_DATA_MSG
*/
typedef PACKED struct
{
   uint8              SrId;
   SysRlpFrameRateT   DataRate;
   uint8              PduType; 
   uint8              FrameType; /*The first 2 bits of RLP frame at SCH goes to the lower
                                 2 bits of frameType */
   uint8*             Data;
} LmdPDUOctetT;

typedef PACKED struct
{
   uint8         Page;                  /* Page in the buffer to be read      */
   uint8         NumberOfSCHPDU; /* Number of PDU data packets    */
   LmdPDUOctetT  PDUData[MAX_PDU_NUM];    /* data type defined in PDUOctet */

} LmdSchRevDataMsgT;

typedef PACKED struct
{
   uint8         Page;       /* Indicate buffer ready to be reuse             */
} LmdSchFwdDataRspMsgT;

/*
** LMD_SCCH_REV_DATA_MSG
*/
typedef PACKED struct
{
   bool       IsSecondary;    /* False=Primary, True=Secondary */
   uint8      NumberOfSCCH;   /* Number of SCCH included       */
   uint8*     Data[7];        /* data pointed by Data          */
} LmdScchRevDataMsgT;

/*
** LMD_SEC_HIGH_MSG,
*/
typedef PACKED struct
{
   bool SecPriorityHigh;
} LmdSecHighMsgT;

typedef enum
{
   LMD_TRANSMITTER_STATUS_ON, /* Transmitter is off */
   LMD_TRANSMITTER_STATUS_OFF /* Transmitter is on */
} LmdTransmitterStatusT;

/*
** LMD_TRANSMITTER_STATUS_MSG,
*/
typedef PACKED struct
{
   LmdTransmitterStatusT Status;
} LmdTransmitterStatusMsgT;

/*
** LMD_SET_STAT_CNT_MSG,
*/

typedef PACKED struct {
   uint8          Offset;              /* Offset from first element     */
   uint32         Value;               /* Value received from BS        */
} SetValueT;

/*
** LMD_SET_STAT_CNT_MSG,
*/
typedef PACKED struct {
   StatCounterT   StatCounter;         /* Statistics counter            */
   uint8          NumSetValue;         /* Number of set value included  */
   SetValueT      SetValue[MAX_STAT_ELEMENT];
} LmdSetStatCntMsgT;

/*
** LMD_GET_STAT_CNT_MSG,
*/
typedef PACKED struct {
  StatCounterT    StatCounter;         /* Statistics counter */
} LmdGetStatCntMsgT;

/*
**LMD_GET_DEBUG_SCRN_INFO_MSG,
*/
typedef PACKED struct {
  ExeRspMsgT  RspInfo;                    /* Response routing information */
} LmdGetDebugScrnInfoRequestT;

typedef PACKED struct {
  uint16 badframes;
  uint16 totframes;
} LmdGetDebugScrnInfoResponseT;

/*
** LMD_SPEECH_DISABLE_MSG,
*/
typedef PACKED struct {
   bool Disable;
} LmdSpeechDisableMsgT;


typedef enum
{
   MUX1_REV_FCH_1    = 1,      
   MUX1_REV_FCH_2,     
   MUX1_REV_FCH_3,      
   MUX1_REV_FCH_4,
   MUX1_REV_FCH_5,             
   MUX1_REV_FCH_6,     
   MUX1_REV_FCH_7,      
   MUX1_REV_FCH_8,
   MUX1_REV_FCH_9,     /* Reserved */
   MUX1_REV_FCH_10,    /* Reserved */
   MUX1_REV_FCH_11,            
   MUX1_REV_FCH_12,    
   MUX1_REV_FCH_13,     
   MUX1_REV_FCH_14,
   MUX1_FOR_FCH_1    = 15,     
   MUX1_FOR_FCH_2,     
   MUX1_FOR_FCH_3,      
   MUX1_FOR_FCH_4,
   MUX1_FOR_FCH_5,             
   MUX1_FOR_FCH_6,     
   MUX1_FOR_FCH_7,      
   MUX1_FOR_FCH_8,
   MUX1_FOR_FCH_9,             
   MUX1_FOR_FCH_10,    
   MUX1_FOR_FCH_11,     
   MUX1_FOR_FCH_12,
   MUX1_FOR_FCH_13,            
   MUX1_FOR_FCH_14,
   PAG_1             = 29,    /* number of paging ch messages received         */
   PAG_2,                     /* number of paging ch messages with bad crc     */
   PAG_3,                     /* number of paging ch msgs addressed to mobile  */
   PAG_4,                     /* number of paging ch half frames received      */
   PAG_5,                     /* # pag half frames rec. part of a valid message*/
   PAG_6,                     /* number of times loss of paging ch declared    */
   PAG_7,                     /* number of idle handoffs                       */   
   ACC_1             = 36,    /* number of layer 3 requests messages generated */
   ACC_2,                     /* number of layer 3 response messages generated */
   ACC_3,                     /* number of times access probe was gen. 2 times */
   ACC_4,                     /* number of times access probe was gen. 3 times */
   ACC_5,                     /* number of times access probe was gen. 4 times */
   ACC_6,                     /* number of times access probe was gen. 5 times */
   ACC_7,                     /* number of times access probe was gen. 6 times */
   ACC_8,                     /* number of unsuccessful access attempts        */
   LAYER2_RTC1       = 44,     
   LAYER2_RTC2,        
   LAYER2_RTC3,         
   LAYER2_RTC4,
   LAYER2_RTC5,
   OTHER_SYS_TIME    = 49,
   MUX2_REV_FCH_1    = 50,     
   MUX2_REV_FCH_2,     
   MUX2_REV_FCH_3,      
   MUX2_REV_FCH_4,
   MUX2_REV_FCH_5,             
   MUX2_REV_FCH_6,     
   MUX2_REV_FCH_7,      
   MUX2_REV_FCH_8,
   MUX2_REV_FCH_9,             
   MUX2_REV_FCH_10,    
   MUX2_REV_FCH_11,     
   MUX2_REV_FCH_12,
   MUX2_REV_FCH_13,            
   MUX2_REV_FCH_14,    
   MUX2_REV_FCH_15,     
   MUX2_REV_FCH_16,
   MUX2_REV_FCH_17,            
   MUX2_REV_FCH_18,    
   MUX2_REV_FCH_19,     
   MUX2_REV_FCH_20,
   MUX2_REV_FCH_21,            
   MUX2_REV_FCH_22,    
   MUX2_REV_FCH_23,     
   MUX2_REV_FCH_24,
   MUX2_REV_FCH_25,            
   MUX2_REV_FCH_26,
   MUX2_FOR_FCH_1    = 76,     
   MUX2_FOR_FCH_2,     
   MUX2_FOR_FCH_3,      
   MUX2_FOR_FCH_4,
   MUX2_FOR_FCH_5,             
   MUX2_FOR_FCH_6,     
   MUX2_FOR_FCH_7,      
   MUX2_FOR_FCH_8,
   MUX2_FOR_FCH_9,             
   MUX2_FOR_FCH_10,    
   MUX2_FOR_FCH_11,     
   MUX2_FOR_FCH_12,
   MUX2_FOR_FCH_13,            
   MUX2_FOR_FCH_14,    
   MUX2_FOR_FCH_15,     
   MUX2_FOR_FCH_16,
   MUX2_FOR_FCH_17,            
   MUX2_FOR_FCH_18,    
   MUX2_FOR_FCH_19,     
   MUX2_FOR_FCH_20,
   MUX2_FOR_FCH_21,            
   MUX2_FOR_FCH_22,    
   MUX2_FOR_FCH_23,     
   MUX2_FOR_FCH_24,
   MUX2_FOR_FCH_25,            
   MUX2_FOR_FCH_26,
   /* reverse SCCH  not supported in CBP4 */
   SCCH1_REV_S       = 102,    
   SCCH1_REV_P,        
   SCCH2_REV_S,         
   SCCH2_REV_P,  
   SCCH3_REV_S,                
   SCCH3_REV_P,        
   SCCH4_REV_S,         
   SCCH4_REV_P,  
   SCCH5_REV_S,                
   SCCH5_REV_P,        
   SCCH6_REV_S,         
   SCCH6_REV_P,  
   SCCH7_REV_S,                
   SCCH7_REV_P,             
   /* forward SCCH  not supported in CBP4 */
   SCCH1_FOR_S       = 116,    
   SCCH1_FOR_P,        
   SCCH2_FOR_S,         
   SCCH2_FOR_P,
   SCCH3_FOR_S,                
   SCCH3_FOR_P,        
   SCCH4_FOR_S,         
   SCCH4_FOR_P,
   SCCH5_FOR_S,                
   SCCH5_FOR_P,        
   SCCH6_FOR_S,         
   SCCH6_FOR_P,
   SCCH7_FOR_S,                
   SCCH7_FOR_P,
   /* MM RTC not supported in CBP4 */
   MM_RTC1           = 130,    
   MM_RTC2,            
   MM_RTC3,             
   MM_RTC4,
   MM_RTC5,
   
   MUX1_REV_FCH_15   = 135,    /* Reserved */      
   MUX1_REV_FCH_5_ms,          /* not supported in CBP4 */
   MUX1_FOR_FCH_15,            /* Reserved */      
   MUX1_FOR_FCH_5_ms,          /* not supported in CBP4 */
   MUX2_REV_FCH_27,            /* Reserved */      
   MUX2_REV_FCH_5_ms,          /* not supported in CBP4 */
   MUX2_FOR_FCH_27,            /* Reserved */      
   MUX2_FOR_FCH_5_ms,          /* not supported in CBP4 */
   
   MUX1_REV_DCCH_1   = 143,    
   MUX1_REV_DCCH_2,    
   MUX1_REV_DCCH_3,     
   MUX1_REV_DCCH_4,
   MUX1_REV_DCCH_5,            
   MUX1_REV_DCCH_6,    
   MUX1_REV_DCCH_7,     
   MUX1_REV_DCCH_8,
   MUX1_REV_DCCH_9,            
   MUX1_REV_DCCH_10,   
   MUX1_REV_DCCH_11,    
   MUX1_REV_DCCH_12,
   MUX1_REV_DCCH_13,           
   MUX1_REV_DCCH_14,   
   MUX1_REV_DCCH_15,   
   MUX1_REV_DCCH_5_ms,  /* not supported in CBP4 */
  
   MUX1_FOR_DCCH_1   = 159,    
   MUX1_FOR_DCCH_2,    
   MUX1_FOR_DCCH_3,     
   MUX1_FOR_DCCH_4,
   MUX1_FOR_DCCH_5,            
   MUX1_FOR_DCCH_6,    
   MUX1_FOR_DCCH_7,     
   MUX1_FOR_DCCH_8,
   MUX1_FOR_DCCH_9,            
   MUX1_FOR_DCCH_10,   
   MUX1_FOR_DCCH_11,    
   MUX1_FOR_DCCH_12,
   MUX1_FOR_DCCH_13,           
   MUX1_FOR_DCCH_14,   
   MUX1_FOR_DCCH_15,   
   MUX1_FOR_DCCH_5_ms,  /* not supported in CBP4 */

   MUX2_REV_DCCH_1   = 175,    
   MUX2_REV_DCCH_2,    
   MUX2_REV_DCCH_3,     
   MUX2_REV_DCCH_4,
   MUX2_REV_DCCH_5,            
   MUX2_REV_DCCH_6,    
   MUX2_REV_DCCH_7,     
   MUX2_REV_DCCH_8,
   MUX2_REV_DCCH_9,            
   MUX2_REV_DCCH_10,   
   MUX2_REV_DCCH_11,    
   MUX2_REV_DCCH_12,
   MUX2_REV_DCCH_13,           
   MUX2_REV_DCCH_14,   
   MUX2_REV_DCCH_15,    
   MUX2_REV_DCCH_16,
   MUX2_REV_DCCH_17,           
   MUX2_REV_DCCH_18,   
   MUX2_REV_DCCH_19,    
   MUX2_REV_DCCH_20, 
   MUX2_REV_DCCH_21,           
   MUX2_REV_DCCH_22,   
   MUX2_REV_DCCH_23,    
   MUX2_REV_DCCH_24,    
   MUX2_REV_DCCH_25,           
   MUX2_REV_DCCH_26,   
   MUX2_REV_DCCH_27,
   MUX2_REV_DCCH_5_ms,  /* not supported in CBP4 */
  
   MUX2_FOR_DCCH_1   = 203,    
   MUX2_FOR_DCCH_2,    
   MUX2_FOR_DCCH_3,     
   MUX2_FOR_DCCH_4,
   MUX2_FOR_DCCH_5,            
   MUX2_FOR_DCCH_6,    
   MUX2_FOR_DCCH_7,     
   MUX2_FOR_DCCH_8,
   MUX2_FOR_DCCH_9,            
   MUX2_FOR_DCCH_10,   
   MUX2_FOR_DCCH_11,    
   MUX2_FOR_DCCH_12,
   MUX2_FOR_DCCH_13,           
   MUX2_FOR_DCCH_14,   
   MUX2_FOR_DCCH_15,    
   MUX2_FOR_DCCH_16,
   MUX2_FOR_DCCH_17,           
   MUX2_FOR_DCCH_18,   
   MUX2_FOR_DCCH_19,    
   MUX2_FOR_DCCH_20, 
   MUX2_FOR_DCCH_21,           
   MUX2_FOR_DCCH_22,   
   MUX2_FOR_DCCH_23,    
   MUX2_FOR_DCCH_24,    
   MUX2_FOR_DCCH_25,           
   MUX2_FOR_DCCH_26,   
   MUX2_FOR_DCCH_27,
   MUX2_FOR_DCCH_5_ms,  /* not supported in CBP4 */

   SCH0_REV_1X       = 231,    
   SCH0_REV_2X,        
   SCH0_REV_4X,         
   SCH0_REV_8X,
   SCH0_REV_16X,             
   SCH0_REV_LTU,       
   SCH0_REV_LTUOK,  /* not supported in CBP4 */

   /*  reverse SCH1 not supported in CBP4 */
   SCH1_REV_1X       = 238,    
   SCH1_REV_2X,        
   SCH1_REV_4X,         
   SCH1_REV_8X,
   SCH1_REV_16X,
   SCH1_REV_LTU,
   SCH1_REV_LTUOK,   /* not supported in CBP4 */

   SCH0_FOR_1X       = 245,    
   SCH0_FOR_2X,        
   SCH0_FOR_4X,         
   SCH0_FOR_8X,
   SCH0_FOR_16X,
   SCH0_FOR_LTU,
   SCH0_FOR_LTUOK,   /* not supported in CBP4 */

   /*  forward SCH1 not supported in CBP4 */
   SCH1_FOR_1X       = 252,    
   SCH1_FOR_2X,        
   SCH1_FOR_4X,         
   SCH1_FOR_8X,
   SCH1_FOR_16X,
   SCH1_FOR_LTU,
   SCH1_FOR_LTUOK,    /* not supported in CBP4 */
   END_OF_LIST
 } RetrievableAndSettableParametersT;

typedef PACKED struct
{
  bool  CounterSuspend;
}LmdCounterSuspendMsgT;

typedef PACKED struct
{
  SysActionTimeT     ActTime;
  uint8              ch_ind; 
  bool               SrvConfigNonNegInc;
}LmdResourceUpdateT;

#ifdef LMD_UNIT_TEST

typedef  enum 
{
   LMD_TEST_ACCESS_PROBE,
   LMD_TEST_SO2,
   LMD_TEST_SCH,
   LMD_TEST_SCH_FWD_DATA,
   LMD_TEST_SCH_REV_RSP_MSG,
   LMD_TEST_REV_DATA_REQ_MSG
}LmdTestMsgT;

typedef PACKED struct 
{
   uint32       ChnlRdy;      /* bit field indicating which channels are ready */
   uint32       ChnlDataQlty; /* bit field indicating channels are good (CRC or rate decn) */
   uint32       FundRate;     /* fund chnl rate if assigned */
   uint32       ChnlType;     /* channel type, FwdChnlTypeT, 4 bits per channel */
                              /* [0:3] channel 0 type */
                              /* [4:7] channel 1 type */
                              /* [8:11] channel 2 type */
                              /* [12:15] channel 3 type */
   uint32       ChnlSize[4];  /* channel size, 1 word per channel */
                              /* ChnlSize[0]: channel 0 size, in bits */
                              /* ChnlSize[1]: channel 1 size, in bits */
                              /* ChnlSize[2]: channel 2 size, in bits */
                              /* ChnlSize[3]: channel 3 size, in bits */
} LmdTstIpcCpFwdChnlDtaMsgT;

typedef PACKED struct
{
   LmdTstIpcCpFwdChnlDtaMsgT SigBuf;
   uint32  DB0[IPC_DTA_MBOX_FWD_CH0_SIZE];
   uint32  DB1[IPC_DTA_MBOX_FWD_CH1_SIZE];
   uint32  DB2[IPC_DTA_MBOX_FWD_CH2_SIZE];
   uint32  DB3[IPC_DTA_MBOX_FWD_CH3_SIZE];
}LmdTestFwdDBMboxT;

typedef PACKED struct
{
   RevChnlDtaMsgT SigBuf;
   uint32  Pad[3];
   uint32  DB0[IPC_DTA_MBOX_REV_CH0_SIZE];
   uint32  DB1[IPC_DTA_MBOX_REV_CH1_SIZE];
   uint32  DB2[IPC_DTA_MBOX_REV_CH2_SIZE];
}LmdTestRevDBMboxT;
#endif
/*-----------------------------------------
*   The Forward SCH  Msg.
*------------------------------------------*/

typedef PACKED struct
{
  uint8               ForSchId;
  uint8               ForSchDuration;
  uint8               ForSchRate;

}LmdFschRecT;

typedef PACKED struct
{
   bool             PilotGatingUseRate;
   bool             ForSchFerRep;
   uint8            NumForSch; /* number of SCH assigned.*/  
   LmdFschRecT      FschRec[2];

}LmdFschMsgT;

typedef PACKED struct
{
  uint8 sch_id;
  bool  FinalBurst;
  
}LmdFschBurstEndMsgT;
/*-----------------------------------------
*   The Reverse SCH  Msg.
*------------------------------------------*/
typedef PACKED struct
{
  uint8              RevSchId;
  uint8              RevSchDuration;
  uint8              RevSchRate;

}LmdRschRecT;

typedef PACKED struct
{
   uint8            RevSchDtxDuration;
   uint8            NumRevSch; /* number of SCH assigned.*/
   LmdRschRecT      RschRec[LMD_MAX_SCH_NUM];
   
}LmdRschMsgT;


/* The Fwd RLP Data Reading Order revert Msg */
typedef PACKED struct
{

   bool   ReadSwap;

}LmdFschPduOrderCtrlMsgT;

/* the Voice Activity Test Msg..*/
typedef enum
{
   LMD_VOICE_ACT_20_PERCENT = 0,
   LMD_VOICE_ACT_40_PERCENT = 1,
   LMD_VOICE_ACT_60_PERCENT = 2,
   LMD_VOICE_ACT_80_PERCENT = 3,
   LMD_VOICE_ACT_100_PERCENT= 4


}LmdVoiceActivityRateT;

/*========================================================================================
==========================================================================================
===================== The following sections need to be resolved. ========================
==========================================================================================
========================================================================================*/


typedef enum
{
  LMD_PRIORITY_NO_MSG,
  LMD_PRIORITY_HIGH,  /* send in < 200 msec */
  LMD_PRIORITY_MEDIUM,  /* send in < 300 msec */
  LMD_PRIORITY_LOW  /* send in < 500 msec */
} LmdMsgPriorityT;

/*---------------------------------------------------------------
*  Miscellaneous Typedefs
*       These typedefs are not used in any of the messages defined
*       so far.  They are here in case other units need these definitions
*       to compile.  If they do not eventually move to message definitions,
*       they should be removed from this file.
*---------------------------------------------------------------*/

typedef enum
{
    LMD_FRAME_RATE_FULL,
    LMD_FRAME_RATE_HALF,
    LMD_FRAME_RATE_QUARTER,
    LMD_FRAME_RATE_EIGHTH,
    LMD_FRAME_ERASURE,
    LMD_FRAME_RATE_FULL_LIKELY,
   LMD_FRAME_RATE       = 0x7FFFFFFF
} LmdFrameRateFwdT;

typedef PACKED struct
{
   uint8 Frames;
} LmdPowerReportFramesT;

typedef enum
{
  LMD_RTC_PREAMBLE = 0,
  LMD_RTC_SIGNALING_ONLY,
  LMD_RTC_TRAFFIC
} LmdRTCTrafficT;
/* Tx signal Control data type */
typedef enum
{
  LMD_REV_TX_DISABLE =0,
  LMD_REV_TX_ENABLE=1

}LmdTxSignalControlT;
/* Rate Test enum for Tx Rate Test Message support - identical to enum in <l1dapi.h> */
typedef enum
{
   LMD_TEST_TX_OFF           = 0,
   LMD_TEST_TX_ACCESS        = 1,
   LMD_TEST_TX_TRAFF_FULL    = 2,
   LMD_TEST_TX_TRAFF_HALF    = 3,
   LMD_TEST_TX_TRAFF_QUARTER = 4,
   LMD_TEST_TX_TRAFF_EIGHTH  = 5
} LmdTstTxCdmaRateT;


typedef	PACKED struct
{
   uint32  ActTime;

}LmdRschStopMsgT; 
/*---------------------------------------------------------------
**  Global function prototypes for use outside LMD
**---------------------------------------------------------------*/

/*-- Reverse channel rate test case handler -----*/
extern void LmdRateTestControl( LmdTstTxCdmaRateT RateCtrl );
/*--- Reverse Tx Signal Control handler----------*/
extern void LmdRevTxSignalControl( LmdTxSignalControlT  TxSigControl);

/* Erasure counter handlers */
extern uint32 LmdSvcErasureCntGet(void);
void LmdSvcErasureCntReset(void);

/*-- Vocoder status --*/
extern bool LmdSvcVocStatusGet (void);




#endif


#ifndef MONAPI_H
#define MONAPI_H

#include "ipcapi.h"
#include "sysapi.h"
#include "sysdefs.h"
#include "dbmapi.h"

/*----------------------------------------------------------------------------
 Defines Macros used in this file
----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 Defines Constants used in this file
----------------------------------------------------------------------------*/

/* Define MON command mailbox ids */
#define MON_MAILBOX              EXE_MAILBOX_1_ID  /* Command mailbox */
#define MON_MAILBOX_EVENT        EXE_MESSAGE_MBOX_1
#define MON_MAILBOX_DNLD         EXE_MAILBOX_2_ID  /* DSP download mailbox */
#define MON_MAILBOX_DNLD_EVENT   EXE_MESSAGE_MBOX_2

#define MON_DIRECT_BUF_SIGNAL    EXE_SIGNAL_1      /* Signal from IPC direct buffer mailbox has data */

/* Define MON CP peek/poke max data size in uint16 units */
#define MON_MAX_PEEK_SIZE        123
#define MON_MAX_POKE_SIZE        123

/* Define max spy data size in bytes */
#define MON_MAX_SPY_SIZE         400

/* Define max printf string size in chars */
#define MON_MAX_PRINTF_STR_SIZE  128

/* Define max file info string size in chars */
#define MON_MAX_FILEINFO_STR_SIZE  128

/* Define max trace and printf number of arguments */
#define MON_MAX_TRACE_ARG_SIZE   10
#define MON_MAX_PRINTF_ARG_SIZE  10

/* Define voice test hardware control constants */
#define MON_DAI_RX_ENABLE        0x1
#define MON_DAI_TX_ENABLE        0x2
#define MON_DAI_TX_RX_ENABLE     0x3

#define MANU_STRING              {"Nokia"}   /* no more than 20 char */
#define MODEL_STRING             {"CBP4PLUS"}      /* no more than 20 char */
/* Define deep sleep voting flags for the different units that need them.
   These defines are used as bit masks in calls to MonDeepSleepSuspend 
   and MonDeepSleepResume */

typedef enum {

	MON_DEEP_SLEEP_HWD_1_FLAG = 0x00000001,
	MON_DEEP_SLEEP_HWD_2_FLAG = 0x00000002,
	MON_DEEP_SLEEP_HWD_3_FLAG = 0x00000004,
	MON_DEEP_SLEEP_HWD_4_FLAG = 0x00000008,
	/* there are 4 more HWD flags below. go look for them */

	MON_DEEP_SLEEP_IOP_1_FLAG = 0x00000010,
	MON_DEEP_SLEEP_IOP_2_FLAG = 0x00000020,
	MON_DEEP_SLEEP_IOP_3_FLAG = 0x00000040,
	MON_DEEP_SLEEP_IOP_4_FLAG = 0x00000080,

	MON_DEEP_SLEEP_L1D_1_FLAG = 0x00000100,
	MON_DEEP_SLEEP_L1D_2_FLAG = 0x00000200,
	MON_DEEP_SLEEP_L1D_3_FLAG = 0x00000400,
	MON_DEEP_SLEEP_L1D_4_FLAG = 0x00000800,

	MON_DEEP_SLEEP_LMD_1_FLAG = 0x00001000,
	MON_DEEP_SLEEP_LMD_2_FLAG = 0x00002000,
	MON_DEEP_SLEEP_LMD_3_FLAG = 0x00004000,
	MON_DEEP_SLEEP_LMD_4_FLAG = 0x00008000,
							   
	MON_DEEP_SLEEP_BAL_1_FLAG = 0x00010000,
	MON_DEEP_SLEEP_BAL_2_FLAG = 0x00020000,
	MON_DEEP_SLEEP_BAL_3_FLAG = 0x00040000,
	MON_DEEP_SLEEP_BAL_4_FLAG = 0x00080000,
							   
	MON_DEEP_SLEEP_MON_1_FLAG = 0x00100000,
	MON_DEEP_SLEEP_MON_2_FLAG = 0x00200000,
	MON_DEEP_SLEEP_MON_3_FLAG = 0x00400000,
	MON_DEEP_SLEEP_MON_4_FLAG = 0x00800000,

	MON_DEEP_SLEEP_HWD_5_FLAG = 0x01000000,
	MON_DEEP_SLEEP_HWD_6_FLAG = 0x02000000,
	MON_DEEP_SLEEP_HWD_7_FLAG = 0x04000000,
	MON_DEEP_SLEEP_HWD_8_FLAG = 0x08000000,

	MON_DEEP_SLEEP_HWD_9_FLAG = 0x10000000,
	MON_DEEP_SLEEP_HWD_10_FLAG = 0x20000000,

/* reserved for testing */
	MON_DEEP_SLEEP_TST_1_FLAG = 0x40000000	/* don't use > 0x7FFF FFFF */

} MonDeepSleepVetoT;

/*------------------------------------------------------------------------
* Define typedefs used in MON API
*------------------------------------------------------------------------*/

/* Define fault halt/continue flag */
typedef enum
{
   MON_CONTINUE  = 0x00,
   MON_HALT      = 0x01
} MonFaultTypeT;

/* Define all unit fault code numbers */
typedef enum
{
   MON_CP_FAULT_UNIT   = 0x00,
   MON_DBM_FAULT_UNIT  = 0x01,
   MON_EXE_FAULT_UNIT  = 0x02,
   MON_IPC_FAULT_UNIT  = 0x03,
   MON_IOP_FAULT_UNIT  = 0x04,
   MON_L1D_FAULT_UNIT  = 0x05,
   MON_LMD_FAULT_UNIT  = 0x06,
   MON_MON_FAULT_UNIT  = 0x07,
   MON_PSW_FAULT_UNIT  = 0x08,
   MON_HWD_FAULT_UNIT  = 0x09,
   MON_TST_FAULT_UNIT  = 0x0A,
   MON_BAL_FAULT_UNIT  = 0x0B,
   MON_SYS_FAULT_UNIT  = 0x0C,
   MON_RLP_FAULT_UNIT  = 0x0D,
   MON_HLW_FAULT_UNIT  = 0x0E,
   MON_AIW_FAULT_UNIT  = 0x0F,
   MON_PDE_FAULT_UNIT  = 0x10,
   MON_L1A_FAULT_UNIT  = 0x11,
   MON_UIM_FAULT_UNIT  = 0x12,
   MON_UI_FAULT_UNIT   = 0x13,
   MON_FSM_FAULT_UNIT  = 0x14,
  MON_BREW_FAULT_UNIT = 0x18,
   MON_NUM_FAULT_UNIT,  
   MON_CP_BOOT_FAULT_UNIT = 0x20, /* these 2 are only used by boot code and therefore  */
   MON_BOOT_FAULT_UNIT    = 0x21  /* should not be accounted for in MON_NUM_FAULT_UNIT */
} MonFaultUnitT;

/* Define Big Buffer modes */
typedef enum
{
   MON_BB_NORMAL_MODE     = 0x00,
   MON_BB_RX_WRITE_MODE,  
   MON_BB_TX_WRITE_MODE,
   MON_BB_RX_READ_MODE,
   MON_BB_RX_SIGDEL_WRITE_MODE
} MonBbModeTypeT;

/* Define MON Task msg Ids */
typedef enum 
{
   MON_PEEK_MSG             = 0x00,
   MON_POKE_MSG             = 0x01,
   MON_SPY_MSG              = 0x02,
   MON_TRACE_MSG            = 0x03,
   MON_MEM_TEST_MSG         = 0x04,
   MON_VERSION_MSG          = 0x05,
   MON_CBP_VERSION_MSG      = 0x06,
   MON_DOWN_COMP_MSG        = 0x07,
   MON_DSP_HEART_BEAT_MSG   = 0x08,
   MON_BB_CONFIG_MSG        = 0x09,
   MON_BB_SYNC_START_MSG    = 0x0A,
   MON_BB_TIM_CHANGE_MSG    = 0x0B,
   MON_BB_WRITE_MSG         = 0x0C,
   MON_BB_READ_MSG          = 0x0D,
   MON_BB_TX_CONFIG_MSG     = 0x0E,
   MON_FLASH_ID_MSG         = 0x0F,
   MON_RESET_PROCESSOR_MSG  = 0x10,
   MON_VOCODER_TEST_MSG     = 0x11,
   MON_SPEECH_WRITE_MSG     = 0x12,
   MON_SPEECH_READ_MSG      = 0x13,
   /* MON_DSP_CONFIG_MSG       = 0x14, MON_DSPM_CONFIG_MSG and MON_DSPM_CONFIG_MSG are now used instead*/
   MON_DSP_INFO_MSG         = 0x15,
   MON_SLEEP_CONTROL_MSG    = 0x16,
   MON_FAULT_CTRL_MSG       = 0x17,
   MON_BLK_WRITE_DB_MSG     = 0x18,
   MON_BLK_READ_DB_MSG      = 0x19,
   MON_DBUF_LOOPBACK_MSG    = 0x1A,
   MON_DSP_DOWNLOAD_CNF_MSG = 0x1B,
   MON_REV_CHANNEL_DATA_MSG = 0x1C,
   MON_RESERVED1_MSG        = 0x1D,
   MON_SET_CLEAR_GPIO_MSG   = 0x1E,
   MON_READ_GPIO_MSG        = 0x1F,
   MON_JUMP_TO_BOOT_MSG     = 0x20,
   MON_SLEEP_ENTER_MSG      = 0x21,
   MON_MONITOR_GPIO_MSG     = 0x22,
   MON_DSPM_PEEK_MSG        = 0x23,
   MON_DSPV_PEEK_MSG        = 0x24,
   MON_DSPM_PEEK_RSP_MSG    = 0x25,
   MON_DSPV_PEEK_RSP_MSG    = 0x26,
   MON_DSPM_CONFIG_MSG      = 0x27,
   MON_DSPV_CONFIG_MSG      = 0x28,
   MON_CP_CONFIG_INFO_MSG   = 0x29,
   MON_BOOT_VERSION_MSG     = 0x2A,
   MON_BITWISE_OPERATION_MSG= 0x2B,
   MON_DSPM_CODE_PEEK_MSG   = 0x2C,
   MON_DSPV_CODE_PEEK_MSG   = 0x2D,
   MON_DSPM_CODE_PEEK_RSP_MSG = 0x2E,
   MON_DSPV_CODE_PEEK_RSP_MSG = 0x2F,
   MON_DSP_JTAG_POWER_MSG     = 0x30,
   MON_TEST_DSP_PATCH_MSG     = 0x31,
   MON_GET_TEST_PATCH_INFO_MSG= 0x32,
   MON_DSPV_ALIVE_MSG         = 0x33,
   MON_DSPV_VERSION_RSP_MSG   = 0x34
} MonMsgIdT;

/* Define sleep control msg structure */
typedef PACKED struct
{
   bool	LightSleepEnableFlag;	/* Light sleep enable flag   */
   bool	DeepSleepEnableFlag;	/* deep sleep enable flag    */
} MonSleepControlMsgT;

	/* MON_SLEEP_ENTER_MSG */
typedef PACKED struct {
	uint32	WakeupTime;	/* time to spend in deep sleep, 32KHz-tick units */
} MonSleepEnterMsgT;

/* Define Fault msg structure */
typedef PACKED struct
{
   MonFaultUnitT  UnitId;
   uint32         SysTime;
   uint32         FaultCode1;
   uint32         FaultCode2;
   MonFaultTypeT  FaultType;
} MonFaultMsgT;

/* Define Fault control msg structure */
typedef PACKED struct 
{
   bool  Control;
} MonFaultCtrlMsgT;

/* Define Fault control msg types */
typedef enum
{
   MON_FAULT_DISABLED = 0x00,
   MON_FAULT_ENABLED
} MonFaultCtrlTypeT;

/* Define printf msg structure */
typedef PACKED struct 
{
   uint32         SysTime;
   uint8          String[MON_MAX_PRINTF_STR_SIZE];
   uint32         Args[1];
} MonPrintfMsgT;

/* Define printf msg structure */
typedef PACKED struct 
{
   uint32         SysTime;
   uint8          String[MON_MAX_FILEINFO_STR_SIZE];
} MonFileInfoMsgT;

/* Define Big Buffer write msg types */
typedef enum
{
   MON_BB_WR_INIT_TYPE     = 0x00,
   MON_BB_WR_DATA_TYPE
} MonBbWriteTypeT;

/* Define Mon ack types */
typedef enum
{
   MON_ACK_TYPE     = 0x00,
   MON_NACK_TYPE
} MonAckTypeT;

/* Define Flash Program msg types */
typedef enum
{
   MON_FLASH_PROG_INIT_TYPE   = 0x00,
   MON_FLASH_PROG_DATA_TYPE
} MonFlashProgTypeT;

/* Define flash sections */
typedef enum
{
   MON_CP_BOOT_FLASH_SECTION       = 0x00,
   MON_CP_CODE_FLASH_SECTION,
   MON_DSPM_CODE_FLASH_SECTION, 
   MON_DSPV_CODE_FLASH_SECTION, 
   MON_CP2_CODE_FLASH_SECTION,
   MON_CP3_CODE_FLASH_SECTION,
   MON_FSM_DATA_FLASH_SECTION,
   MON_ALL_FLASH_SECTION,
   MON_FLASH_SECTION_LIST_SIZE
} MonFlashSectionT;

/* Define Speech write msg types */
typedef enum
{
   MON_SPEECH_WR_INIT_TYPE     = 0x00,
   MON_SPEECH_WR_DATA_TYPE
} MonSpeechWriteTypeT;

/* Define download complete mgs */
typedef PACKED struct 
{
   uint16      Checksum;
} MonDownCompMsgT; 

/* Define peek/poke address types */
typedef enum
{
   MON_ADDRTYPE_MEMORY   = 0,
   MON_ADDRTYPE_REGISTER,
   MON_ADDRTYPE_32BITDATA
} MonAddrTypeT;

/* Define MON peek msg */
typedef PACKED struct 
{
   ExeRspMsgT     RspInfo;    
   uint32         StartAddr;
   uint8          NumUint16;
   MonAddrTypeT   AddrType;
} MonPeekMsgT;

/* Define MON peek response msg */
typedef PACKED struct 
{
   uint32      StartAddr;
   uint8       NumUint16;
   uint16      Data[1];
} MonPeekRspMsgT;

/* Define MON poke msg command */
typedef PACKED struct 
{
   ExeRspMsgT     RspInfo;    
   uint32         StartAddr;
   uint8          NumUint16;
   MonAddrTypeT   AddrType;
   uint16         Data[1];
} MonPokeMsgT;

/* Define MON poke response msg */
typedef PACKED struct 
{
   uint32      StartAddr;
   uint8       NumUint16;
} MonPokeRspMsgT;

/* Define MON_BITWISE_OPERATION_MSG command */
typedef enum
{
   MON_BITOP_TOGGLE = 0,
   MON_BITOP_SET,
   MON_BITOP_CLEAR
} MonBitwiseOperatorT;

typedef PACKED struct
{
   ExeRspMsgT           RspInfo;
   uint16               *Address;
   MonBitwiseOperatorT  Operator;
   uint16               Mask;
} MonBitOpMsgT;

/* Define MON_BITWISE_OPERATION_MSG response msg */
typedef PACKED struct
{
   uint16         OldValue;
   uint16         NewValue;
} MonBitOpRspMsgT;

/* Define MON spy msg command */
typedef PACKED struct 
{
   ExeRspMsgT  RspInfo;    
   uint16      SpyId;
   uint8       SpyActive;
} MonSpyMsgT;

/* Define MON spy msg response */
typedef PACKED struct 
{
   uint16      SpyId; 
   uint32      SysTime; 
   uint8       Data[1];
} MonSpyRspMsgT;

/* Define MON trace msg command */
typedef PACKED struct 
{
   ExeRspMsgT  RspInfo;    
   uint16      TraceId;
   uint8       TraceData[1];
} MonTraceMsgT;

/* Define MON trace response msg */
typedef PACKED struct 
{
   uint16      TraceId; 
   uint32      SysTime; 
   uint32      Args[1]; 
} MonTraceRspMsgT;

/* Define MON memory test msg command */
typedef PACKED struct 
{
   ExeRspMsgT  RspInfo;    
   uint32      StartAddr;              
   uint32      NumBytes;              
} MonMemTestMsgT;

/* Define MON memory test response msg */
typedef PACKED struct 
{
   bool        Result; 
   uint32      ErrAddr; 
} MonMemTestRspMsgT;

/* Define MON version type */
typedef PACKED struct 
{
   uint8       UnitNum;
   uint8       VerInfo[3];
   uint8       TimeInfo[5]; 
} MonVersionT;

/* Define MON version msg */
typedef PACKED struct 
{
   ExeRspMsgT   RspInfo;    
} MonVersionMsgT;

/* Define MON version response msg */
typedef PACKED struct 
{
   MonVersionT  Info[2];
} MonVersionRspMsgT;

/* Define MON CBP version msg */
typedef PACKED struct 
{
   ExeRspMsgT   RspInfo;    
} MonCBPVersionMsgT;

/* DSP patch RAM structure */
typedef PACKED struct 
{
   uint16   PatchAddr;
   uint16   MatchAddr;
} PatchInfoT;

typedef PACKED struct 
{
   uint16      NumPatches;
   uint8       BuildDate[13];
   uint8       BuildTime[8];
   uint8       PatchRevision[11];
   uint8       IdNumber;
   uint8       IdString[16];
   uint16      CodeSize;
} BuildInfoT;

/* Define MON CBP version response msgs
   (the RAM version does not have any patch information) */
typedef PACKED struct 
{
   uint8       AsicType[14];
   uint16      ChipIdHi;
   uint16      ChipIdLow;
   BuildInfoT  BuildInfo[2]; /* DSPM and DSPV */
} MonCBPVersionRspMsgT;

/* Define MON DSP heart beat msg */
typedef PACKED struct 
{
   uint16   ProcessorId;
} MonDspHeartBeatMsgT;

/* Define MON Big Buffer Configuration msg */
typedef PACKED struct 
{
   MonBbModeTypeT   Mode;
   uint32           StartAddr;
   uint32           StopAddr;
} MonBbConfigMsgT;

/* Define MON Big Buffer Synchronous start msg */
typedef PACKED struct
{
   uint8    Mode;
   uint32   StartAddr;       
   uint32   StopAddr;        
   uint16   LongCodeState[3];
   uint16   LongCodeRxMask[3]; 
   uint16   LongCodeTxMask[3]; 
   uint16   RxChanSel;  
   uint16   LcTxRxDly;
} MonBbSyncStartMsgT;

/* Define MON Big Buffer timing change msg */
typedef PACKED struct
{
   uint16 FrameOffset;
} MonBbTimChangeMsgT;

/* Define MON Big Buffer Write msg */
typedef PACKED struct 
{
   ExeRspMsgT        RspInfo;    
   uint32            SeqNum;
   MonBbWriteTypeT   MsgType;
   uint16            Checksum;
   uint8             NumBytes;
   uint8             Data[1];
} MonBbWriteMsgT;

/* Define MON Big Buffer Write response msg */
typedef PACKED struct 
{
   uint32            SeqNum;
   MonAckTypeT       AckType;
} MonBbWriteRspMsgT;

/* Define MON Big Buffer Read msg */
typedef PACKED struct 
{
   ExeRspMsgT        RspInfo;    
   uint32            SeqNum;
   uint32            ReadAddr;
   uint8             NumBbWords;
} MonBbReadMsgT;

/* Define MON Big Buffer Read response msg */
typedef PACKED struct 
{
   uint32            SeqNum;
   uint16            Checksum;
   uint8             NumBytes;
   uint8             Data[1];
} MonBbReadRspMsgT;

/* Define MON tx hw test msg */
typedef PACKED struct
{
   uint16 FrameOffset;
   uint16 TxOffset;
   uint16 TxPh;
   uint16 TxRxOffset;
} MonBbTxConfigMsgT;

/* Define Flash ID msg */
typedef PACKED struct 
{
   ExeRspMsgT     RspInfo;    
} MonFlashIdMsgT;

/* Define Flash ID response msg */
typedef PACKED struct 
{
   uint16         ManfId;
   uint16         DevId;
} MonFlashIdRspMsgT;

/* 
   This data structure is needed even though there is
   no message ID defined. It is used by the boot loader
   code 
*/
/* Define Flash erase msg */
typedef PACKED struct 
{
   ExeRspMsgT        RspInfo;    
   MonFlashSectionT  FlashSection;
} MonFlashEraseMsgT;

/* 
   This data structure is needed even though there is
   no message ID defined. It is used by the boot loader
   code 
*/
/* Define Flash erase response msg */
typedef PACKED struct 
{
   MonFlashSectionT  FlashSection;
} MonFlashEraseRspMsgT;

/* 
   This data structure is needed even though there is
   no message ID defined. It is used by the boot loader
   code 
*/
/* Define Flash program msg */
typedef PACKED struct 
{
   ExeRspMsgT         RspInfo;    
   uint32             SeqNum;
   MonFlashProgTypeT  MsgType;
   MonFlashSectionT   FlashSection;
   uint16             Checksum;
   uint16             NumBytes;
   uint8              Data[1];
} MonFlashProgMsgT;

/* 
   This data structure is needed even though there is
   no message ID defined. It is used by the boot loader
   code 
*/
/* Define Flash program response msg */
typedef PACKED struct 
{
   uint32             SeqNum;
   MonAckTypeT        AckType;
} MonFlashProgRspMsgT;

/* Define processor reset msg */
typedef PACKED struct 
{
   IpcProcIdT    ProcessorId;
} MonResetProcessorMsgT;

/* Enumerated type for specifying Vocoder Test Mode */   
 typedef enum
{
  MON_ENCODER_DECODER_INVALID    = 0x0,  
  MON_ENCODER_ONLY               = 0x1,  
  MON_DECODER_ONLY               = 0x2,  
  MON_ENCODER_DECODER            = 0x4  
} MonVTstModeT;

/* Structure for sending Vocoder Test Mode */   
typedef PACKED struct
{
  ExeRspMsgT    RspInfo;    
  uint32        DownloadStartAddr;        
  uint32        UploadStartAddr;        
  uint32        DownloadLength;         
  uint32        UploadLength;         
  MonVTstModeT  TestMode;  
  bool          EnableHw;
  uint32        PacketSize;
  uint16        AudioSpchSrvcOptMaxRate;   /* maximum voice encode rate */
} MonVTstMsgT;

typedef PACKED struct
{
  bool          TestComplete;
  uint32        DownloadStartAddr;        
  uint32        DownloadWordsUsed;        
  uint32        UploadStartAddr;        
  uint32        UploadWordsCreated;        
  uint32        MaxCompPackets;
  uint32        NumCompPackets;
  uint32        MaxPcmWords;
  uint32        NumPcmWords;
} MonVTstRspT;

/* Define MON Speech data Write msg */
typedef PACKED struct 
{
   ExeRspMsgT           RspInfo;    
   uint32               SeqNum;
   MonSpeechWriteTypeT  MsgType;
   uint32               WriteAddr;
   uint16               Checksum;
   uint8                NumBytes;
   uint8                Data[1];
} MonSpeechWriteMsgT;

/* Define MON Speech data Write response msg */
typedef PACKED struct 
{
   uint32            SeqNum;
   MonAckTypeT       AckType;
} MonSpeechWriteRspMsgT;

/* Define MON Speech Data Read msg */
typedef PACKED struct 
{
   ExeRspMsgT        RspInfo;    
   uint32            SeqNum;
   uint32            ReadAddr;
   uint8             NumBytes;
} MonSpeechReadMsgT;

/* Define MON Speech data Read response msg */
typedef PACKED struct 
{
   uint32            SeqNum;
   uint16            Checksum;
   uint8             NumBytes;
   uint8             Data[1];
} MonSpeechReadRspMsgT;

/* Define MON DSPM/V Config message */
typedef PACKED struct 
{
   ExeRspMsgT        RspInfo;    
} MonDspConfigMsgT;

/* Define MON DSPM/V Config response message */
typedef PACKED struct 
{
   uint16            ConfigDataL;              
   uint16            ConfigDataH;    
} MonDspConfigRspMsgT;

/* Define MON DSP information msg received from DSPM and DSPV */
typedef PACKED struct 
{
   uint16            ProcessorId;
   uint16            ConfigDataL;
   uint16            ConfigDataH;
} MonDspInfoMsgT;

/* Define MON block data base write message */
typedef PACKED struct
{
   ExeRspMsgT         RspInfo;    
   uint32             SeqNum;
   DbmProgTypeT       MsgType;
   DbmBlkDataBaseIdT  DataBaseId;
   uint32             Offset;
   uint16             Checksum;
   uint16             NumBytes;
   uint8              Data[1];
} MonBlkDbWriteMsgT;

/* Define MON block data base write response message */
typedef PACKED struct
{
   uint32             SeqNum;
   DbmBlkDataBaseIdT  DataBaseId;
   DbmAckTypeT        AckType;
} MonBlkDbWriteRspMsgT;

/* Define MON block data base read message */
typedef PACKED struct
{
   ExeRspMsgT         RspInfo;    
   uint32             SeqNum;
   DbmBlkDataBaseIdT  DataBaseId;
   uint32             Offset;
   uint16             NumBytes;
} MonBlkDbReadMsgT;

/* Define MON block data base read response message */
typedef PACKED struct
{
   uint32             SeqNum;
   DbmBlkDataBaseIdT  DataBaseId;
   uint32             DataBaseSize;
   uint32             Offset;
   uint16             Checksum;
   uint16             NumBytes;
   uint8              Data[1];
} MonBlkDbReadRspMsgT;

/* Define MON direct buffer mailbox loopback message */
typedef PACKED struct
{
   ExeRspMsgT  RspInfo;    
   uint32      NumLoops;
   uint16      NumWords;
   uint16      Data[1];
} MonDBufLoopbackMsgT;

/* Define MON direct buffer mailbox loopback response msg */
typedef PACKED struct 
{
   bool        Result; 
} MonDBufLoopbackRspMsgT;

/* Define MON DSP download confirmation message */
typedef PACKED struct
{
  bool         Result;   /* False return result indicates error in download */
} MonDspDownloadCnfMsgT;

/* Define MON REV channel data message */
typedef PACKED struct
{
  uint16  ChnlDataReady;   /* Bitfield indicating which channels contain data */
  uint16  FundRate;        /* Fundamental rate, if assigned */
  uint16  NumWords;
  uint16  Data[1];
} MonRevChnlDataMsgT;

/* Define set/clear GPIO actions used by MonSetClearGPIOMsgT */
typedef enum
{
   MON_CLEAR_GPIO = 0x00,
   MON_SET_GPIO
} MonGPIOActionT;

/* Define MON Set Clear GPIO msg */
typedef PACKED struct 
{
   uint8          GPIONum;
   MonGPIOActionT Action;
} MonSetClearGPIOMsgT;

/* Define MON Read GPIO msg */
typedef PACKED struct 
{
   ExeRspMsgT  RspInfo;    
   uint8       GPIONum;
} MonReadGPIOMsgT;

/* Define MON Read GPIO Response msg */
typedef PACKED struct 
{
   bool  Data;
} MonReadGPIORspMsgT;

/* Define MON Monitor GPIO msg */
typedef PACKED struct 
{
   ExeRspMsgT  RspInfo;    
   uint8       GPIONum;
} MonMonitorGPIOMsgT;

/* Define MON Read GPIO Response msg */
typedef PACKED struct 
{
    uint8  Direction; /* GPIO_OUTPUT=0, GPIO_INPUT=1, Invalid = 0xff */
    uint8  Data;
} MonMonitorGPIORspMsgT;

/* Define MON DSP peek msg */
typedef PACKED struct 
{
   uint16       StartAddr;              
   uint16       NumWords;    
} MonDspPeekMsgT;

/* Define MON DSP peek response message */
typedef PACKED struct 
{
   uint16       StartAddr;
   uint16       NumWords;
   uint16       RspTaskId; /* CP task id to which the peek response should be routed */
   uint16       RspMboxId; /* CP mailbox id to which the peek response should be routed */
   uint16       Data[1];
} MonDspPeekRspMsgT;

/* Define MON Boot version msg */
typedef PACKED struct 
{
   ExeRspMsgT   RspInfo;    
} MonBootVersionMsgT;

/* Define MON Boot version response msg */
typedef PACKED struct 
{
   MonVersionT  Info[2];
} MonBootVersionRspMsgT;

/* Data structure for HS identity info msg */
typedef PACKED struct 
{
   ExeRspMsgT   RspInfo;    
} MonCpConfigInfoMsgT;

/* Data Sturcture for HS identity Rsp Msg */
typedef PACKED struct 
{ 
  uint8     AsicType;
  uint8     CPVersion;
  uint8     DSMPatchId;
  uint8     DSVPatchId;
  uint8     RFOption;
  uint8     RefClockFreq;
  bool      UIMEnabled;
  bool      FSMEnabled;
  bool      DataSvcEnabled;
  bool      SchEnabled;
  bool      AmpsEnabled;
  bool      GPSEnabled;
  bool      AuxAdcPollingEnabled;
  bool      DigitalRxAgcEnabled;
  uint8     ManufactureId[20];
  uint8     ModelId[20];

} MonCpConfigInfoRspMsgT;

/* Data Structure for DSP JTAG Power Msg */
typedef PACKED struct
{
   uint8  ProcessorId;
   uint8  Power;
} MonDspJtagPowerMsgT;

/* Message Structures and definitions for patch test messages */
typedef enum
{
   MON_TEST_PATCH_FAILED,
   MON_TEST_PATCH_PASSED,
   MON_TEST_PATCH_END
} MonTestResultT;

typedef enum
{
   MONTEST_NO_REASON = 0,
   DSP_FAILED_TO_BOOT,
   VERSION_RSP_NOT_RECEIVED,
   WRONG_SEQUENCE_NUMBER
} MonTestReasonT;

typedef PACKED struct
{
   ExeRspMsgT     RspInfo;
   bool           UseDefaultAddresses;
   uint16         PatchAddr;
   uint16         MatchStartAddr;
   uint16         MatchEndAddr;
   uint16         NumLoops;
} MonTestDspPatchMsgT;

typedef PACKED struct
{
   uint16         PatchAddr;
   uint16         MatchAddr;
   uint16         ReturnCode;
   MonTestResultT Result;
   MonTestReasonT Reason;
} MonTestDspPatchRspMsgT;

typedef PACKED struct
{
   ExeRspMsgT  RspInfo;    
} MonGetTestPatchInfoMsgT;

typedef PACKED struct
{
   uint16      *CodeP;
   uint16      CodeSize;
   uint16      CodeAddr;
   uint16      PatchAddr;
   uint16      MatchStartAddr;
   uint16      MatchEndAddr;
}MonGetTestPatchInfoRspMsgT;

typedef PACKED struct
{
   uint16      Address;
   uint16      NumWords;
   uint16      Code[1];
}MonDspCodePokeMsgT;

typedef PACKED struct
{
   uint16      Sequence;
} MonTestDspStartMsgT;

typedef PACKED struct
{
   uint16      Sequence;
} MonTestDspVerRspMsgT;

/*------------------------------------------------------------------------
*  Define Global Data
*------------------------------------------------------------------------*/

/*------------------------------------------------------------------------
*  Define Global Function Prototypes
*------------------------------------------------------------------------*/
extern bool MonRestartDspv (void);

/*------------------------------------------------------------------------
 *  monsleep.c interface
 *------------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif
void    MonDeepSleepTimeSlice( uint32 Time );
void    MonDeepSleepCancel( void );
uint32	MonDeepSleepSuspend( MonDeepSleepVetoT BitMask );
uint32	MonDeepSleepResume( MonDeepSleepVetoT BitMask );
void    MonDeepSleepRequest( uint32 WakeTime );
bool    MonDeepSleepIsVetoed( void );
#ifdef __cplusplus
}
#endif



/*****************************************************************************
 
  FUNCTION NAME: MonFault
 
  DESCRIPTION:

    This routine sends software fault declared by a software unit to
    the ETS. If this is a CONTINUE type fault then the fault is sent
    to the ETS and this routine returns to the caller. If this is a 
    HALT type fault this routine does NOT return to the caller and the
    processor is put into an endless loop.
 
  PARAMETERS:
 
    UnitNum     - The number of the unit declaring a fault
    FaultCode1  - The fault or error code
    FaultCode2  - Second fault or error code
    FaultType   - Type of fault HALT or CONTINUE
               
  RETURNED VALUES:
 
    None.
 
*****************************************************************************/
#ifdef SYS_DEBUG_FAULT_FILE_INFO

#define MonFault(Unit, Code1, Code2, Type)  __MonFault(Unit, __FILE__, __LINE__, Code1, Code2, Type)

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

extern void __MonFault(MonFaultUnitT UnitNum, const char *filename, unsigned line, 
                       uint32 FaultCode1, uint32 FaultCode2, MonFaultTypeT FaultType);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#else

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

extern void MonFault(MonFaultUnitT UnitNum, uint32 FaultCode1, uint32 FaultCode2,
                     MonFaultTypeT FaultType);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SYS_DEBUG_FAULT_FILE_INFO */


/*****************************************************************************
 
  FUNCTION NAME: MonSpy

  DESCRIPTION:

    This routine spies a set of data defined in this call. This data will
    be sent to the ETS as spy data if the spy id has been activated.

  PARAMETERS:
 
    SpyId      -  Spy Id value
    DataP      -  Pointer to data to spy
    NumBytes   -  Number of bytes to spy

  RETURNED VALUES:

    None.
 
*****************************************************************************/

extern void MonSpy(uint16 SpyId, uint8 *DataP, uint32 NumBytes);


/*****************************************************************************
 
  FUNCTION NAME: MonSpyGather

  DESCRIPTION:

    This routine spies a set of data defined in this call. This data will
    be sent to the ETS as spy data if the spy id has been previously 
    activated.  This call differs from the MonSpy call only in that it collects
    the data for the spy as specified in the list provided.

  PARAMETERS:
 
    SpyId      -  Spy Id value
    NumSeg     -  Number of Segments in the ListP
    ListP      -  List of Data segments to gather into the spy.

  RETURNED VALUES:

    None.
 
*****************************************************************************/

typedef struct
{
    uint8 * DataP;
    uint32 NumBytes;
} MonSpyGatherItemT;

extern void MonSpyGather(uint16 SpyId, uint8 NumSeg, MonSpyGatherItemT * ListP );


/*****************************************************************************
 
  FUNCTION NAME: MonSpyInquire

  DESCRIPTION:

    This routine returns the state of a spy. Spies can be active or
    inactive.

  PARAMETERS:
 
    SpyId     -  Spy Id value

  RETURNED VALUES:

    Boolean   -  TRUE if spy active
                 FALSE if spy inactive
 
*****************************************************************************/

extern bool MonSpyInquire(uint16 SpyId);


/*****************************************************************************
 
  FUNCTION NAME: MonTrace

  DESCRIPTION:

    This routine sends trace data back to the EST if the appropriate
    trace has been previously activated. The current limitations on 
    this call are, there can only be up to 10 arguments in the
    variable length argument list.

  PARAMETERS:
 
    TraceId      - The number of the unit declaring a fault
    NumArgs      - Number of arguments to follow
    ...          - Argument list

  RETURNED VALUES:

    None.

*****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

extern void MonTrace(uint16 TraceId, uint32 NumArgs, ...);

#ifdef __cplusplus
}
#endif


/*****************************************************************************
 
  FUNCTION NAME: MonPrintf

  DESCRIPTION:

    This routine performs a printf to the ETS screen. The current limitations
    on this call are, the text string can not be > 23 characters long, there 
    can be up to 10 parameters, all format control characters are supported 
    except %s and %c.
    
    Notes:
      1) By defining SYS_PRINTF the normal operation of this function is
         replaced by printf from the standard c library. This is useful when
         debugging without a serial port. Depending on the actual 
         implementation of printf the display will appear on the debugger
         console window.
    

  PARAMETERS:

    Normal printf parameters.

  RETURNED VALUES:

    None.
 
*****************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif
#ifndef SYS_PRINTF
   extern void MonPrintf(char *fmt, ...);
#else
   #define MonPrintf  MonSimPrintf
   #include <stdio.h>
   extern void MonSimPrintf(char *fmt, ...);
#endif
#ifdef __cplusplus
}
#endif

/*****************************************************************************
 
  FUNCTION NAME: MonSprintf

  DESCRIPTION:

    This is a simplified version of sprintf().

    The limitations are:

       1) The output buffer size is limited to SYS_MAX_SPRINTF_STR_SIZE (80)
          including the NULL terminator.

       2) The format options only include: %d, %nd, %0nd, %x, %nx, %0nx,
          %X, %nX and %0nX, where n is the number of digits to output,

       3) All other formats are exluded and taken as normal text (there
          are no error checking on the format options).

  PARAMETERS:
 
    Normal sprintf parameters.

  RETURNED VALUES:

    None.
 
*****************************************************************************/
#define SYS_MAX_SPRINTF_STR_SIZE 80
extern void MonSprintf (char *Buf, const char *Fmt, ...);


/*------------------------------------------------------------------------
 *  montimestamp.c interface
 *------------------------------------------------------------------------*/
	/* CmdId for MonTimeStampCtrl() */
typedef enum {
	MON_TSCMD_INIT,		/* init package */
	MON_TSCMD_ENABLE,	/* enable traceing */
	MON_TSCMD_MASK,		/* disable individual trace */
	MON_TSCMD_UNMASK,	/* enable individual trace */
	MON_TSCMD_SPY		/* dump info to spy */
} MonTimeStampCmdT;

extern void	MonTimeStampCtrl( MonTimeStampCmdT CmdId, uint32 Arg );
extern void	MonTimeStampStart( uint8 Id );
extern void	MonTimeStampEnd( uint8 Id );

#ifdef SYS_DEBUG_FAULT_FILE_INFO
/*****************************************************************************
 
  FUNCTION NAME: MonPrintFileInfo

  DESCRIPTION:

    This routine sends fault related file and line number information to ETS.
    It accepts a pointer to a null terminated string.

  PARAMETERS:
 
    Pointer to string.

  RETURNED VALUES:

    None.
 
*****************************************************************************/
extern void MonPrintFileInfo(char *FileInfo);

#endif /* SYS_DEBUG_FAULT_FILE_INFO */

extern bool MonTestDspPatchInProgress (void);
extern bool MonIsRAMChip(void);
extern bool MonIsDspvDnldDisabled (void);
void *MonGetDspv(void);
uint32 MonGetDspvSize(void);




#endif /* MONAPI_H */

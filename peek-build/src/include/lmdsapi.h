#ifndef _LMDSAPI_H_
#define _LMDSAPI_H_



/*----------------------------------------------------------------------------
 Include Files
----------------------------------------------------------------------------*/

#include "sysapi.h"
#include "exeapi.h"

/*----------------------------------------------------------------------------
* EXE Interfaces - Definition of Signals and Mailboxes
*----------------------------------------------------------------------------*/

#define LMD_S_MAILBOX            EXE_MAILBOX_1_ID

#define LMD_S_STARTUP_SIGNAL     EXE_SIGNAL_1
#define LMD_S_20MS_STROBE_SIG    EXE_SIGNAL_2

#define LMD_S_VMEM_STROBE_SIG    EXE_SIGNAL_3

/*---------------------------------------------------------------
*  Message IDs for signals and commands sent to LMD_S
* (NOTE:  TDSO Message Definitions are defined in lmdtdso.h)
*---------------------------------------------------------------*/

typedef enum
{
   LMD_S_TDSO_CONNECT_MSG = 0,
   LMD_S_TDSO_CONTROL_MSG,
   LMD_S_TDSO_DATA_MSG,
   LMD_S_TDSO_DISCONNECT_MSG,
   LMD_S_TDSO_CLEAR_STATS_MSG,
   LMD_S_TDSO_RANDOM_NUMBER_CALC_MSG,
   
   /* Voice Memo and Voice Rec recordings */
   LMD_VMEM_REC_START_MSG,            /* voice memo (off-line) */
   LMD_VMEM_REC_STOP_MSG,
   LMD_CONVERSATION_REC_START_MSG,    /* conversion (on-line)  */
   LMD_CONVERSATION_REC_STOP_MSG,
   LMD_VREC_CAPTURE_START_MSG,        /* training              */
   LMD_VREC_CAPTURE_STOP_MSG,
   LMD_VREC_RECOGNITION_START_MSG,    /* recognition           */
   LMD_VREC_RECOGNITION_STOP_MSG,

    /* Voice Memo and Voice Rec playbacks */
   LMD_SPEECH_PLAY_MSG,               /* voice memo            */
   LMD_SPEECH_PLAY_CANCEL_MSG,
   LMD_SPEECH_PLAY_PAUSE_MSG,
   LMD_SPEECH_PLAY_RESUME_MSG,
   LMD_ANSWER_PLAY_MSG,               /* answering machine     */
   LMD_ANSWER_PLAY_CANCEL_MSG,
   LMD_ANSWER_PLAY_PAUSE_MSG,
   LMD_ANSWER_PLAY_RESUME_MSG,
   LMD_VREC_PLAYBACK_START_MSG,       /* training/recognition  */
   LMD_VREC_PLAYBACK_CANCEL_MSG,
   LMD_RECORDING_SPCH_MSG,            /* data msg from Dspv    */
   LMD_BLK_READ_RSP_MSG,              /* read ack from Dbm     */
   LMD_S_NUM_CMD_MSG_IDS
} LmdsCmdMsgIdT;

/*---------------------------------------------------------------
*  Basic Types
*---------------------------------------------------------------*/
typedef enum
{
    LMD_PLAYBACK_TYPE_NONE,
    LMD_PLAYBACK_TYPE_RAW_PCM,
    LMD_PLAYBACK_TYPE_VOICE_PKTS
} LmdVmemPlaybackTypesT;

/*---------------------------------------------------------------
*  Message Definitions
*---------------------------------------------------------------*/
typedef PACKED struct
{
   IpcSpchSrvcOptT            SpchSrvcOption;   /* Speech Service Option to use when recording */
   IpcSpchSrvcOptRateT        MaxRate;          /* max encode rate */
   IpcSpchSrvcOptRateT        MinRate;          /* min encode rate */
   uint16                     DataType;    /* LSB : Use IpcVoiceRecPlayDataTypeT  voice record playback data type */
                                           /* MSB : Use IpcVoiceCompandDataT      When LSB is PCM, MSB determine between LinearPCM, Mu-lawPCM or A-lawPCM */

}LmdVmemRecStartMsgT;

/*
** LMD_VMEM_REC_START_MSG
*/
typedef PACKED struct
{
   IpcVoiceRecSelPathModeT    Path;             /* Recording Path (Near/Far) */
   uint16                     DataType;    /* LSB : Use IpcVoiceRecPlayDataTypeT  voice record playback data type */
                                           /* MSB : Use IpcVoiceCompandDataT      When LSB is PCM, MSB determine between LinearPCM, Mu-lawPCM or A-lawPCM */
   IpcSpchSrvcOptT            SpchSrvcOption;   /* Speech Service Option to use when recording */
}LmdVmemConversationRecStartMsgT;

/*
** LMD_CONVERSATION_REC_START_MSG
*/
typedef LmdVmemRecStartMsgT      LmdVrecConversationRecStartMsgT;

/*
** LMD_VREC_CAPTURE_START_MSG
*/
typedef LmdVmemRecStartMsgT      LmdVrecCaptureStartMsgT;

/*
** LMD_VREC_RECOGNITION_START_MSG
*/
typedef LmdVmemRecStartMsgT      LmdVrecRecognitionStartMsgT;

/*
** LMD_SPEECH_PLAY_MSG
*/
typedef PACKED struct
{
   IpcSpchSrvcOptT   SpchSrvcOption;   /* Speech Service Option to use when recording */
   uint16            NumPackets;       /* Number of speech packets */
   uint16            FlashDataType;    /* FALSE: data in RAM, TRUE: data in FLASH */
   uint16           *SpchDataP;        /* used if FlashDataType = FALSE */
   DbmBlkDataBaseIdT DataBaseId;       /* used if FlashDataType = TRUE */
   uint32            Offset;           /* used if FlashDataType = TRUE */
   uint16            DataType;         /* LSB : Use IpcVoiceRecPlayDataTypeT  voice record playback data type */
                                       /* MSB : Use IpcVoiceCompandDataT      When LSB is PCM, MSB determine between LinearPCM, Mu-lawPCM or A-lawPCM */
} LmdSpeechPlayMsgT;

/*
** LMD_ANSWER_PLAY_MSG
*/
typedef LmdSpeechPlayMsgT        LmdAnswerPlayMsgT;

/*
** LMD_VREC_PLAYBACK_START_MSG
*/
typedef LmdSpeechPlayMsgT        LmdVrecPlaybackStartMsgT;

/*---------------------------------------------------------------
*  Miscellaneous Typedefs
*---------------------------------------------------------------*/

/* voice memo service option connect/disconnect event handlers */
extern void LmdVoiceMemoSpeechSrvOptConnect ( LmdServiceOptionT SrvOpt );
extern void LmdVoiceMemoSpeechSrvOptDisconnect ( void );

extern LmdVmemPlaybackTypesT LmdVmemPlaybackActive(void);





#endif

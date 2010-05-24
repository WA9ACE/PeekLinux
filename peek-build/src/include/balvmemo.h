#ifndef __BAL_VOICE_MEMO_H__
#define __BAL_VOICE_MEMO_H__


 
/*-----------------------------------------------------------------
 * VMEMO Defines and Type Definitions
 *----------------------------------------------------------------*/

typedef enum 
{
   BAL_VMEMO_REC_TYPE_OFFLINE_EVRC,
   BAL_VMEMO_REC_TYPE_OFFLINE_QCELP13K,
   BAL_VMEMO_REC_TYPE_ONLINE_FWD,
   BAL_VMEMO_REC_TYPE_ONLINE_REV
}BalVmemoRecTypeT;

typedef enum
{
   BAL_VMEMO_FORMAT_QCP_VOICE_PACKETS,
   BAL_VMEMO_FORMAT_RAW_PCM_SAMPLES
} BalVmemoDataFormatT;

typedef enum
{
   BAL_VMEMO_RATE_FULL = 4,
   BAL_VMEMO_RATE_HALF = 3
} BalVmemoMaxRateTypeT;

/* Data for BAL_VMEMO_RECORD_END_EVENT and BAL_VMEMO_PLAYBACK_END_EVENT */
typedef struct
{
   BalAppStatusT  ReturnCode;
} BalVmemoEventMsgT;


/*-----------------------------------------------------------------
 * The following definitions are used by LMD VMemo
 *----------------------------------------------------------------*/
/* BAL_VMEMO_SSO_NOTIFY_MSG */
typedef PACKED struct
{
   IpcSpchSrvcOptT          SpchSrvcOption;   /* Speech Service Option connected */
}BalVmemSsoNotifyMsgT;
/* For BAL_SPEECH_PLAY_COMPLETE_MSG, BAL_VREC_PLAYBACK_CANCEL_MSG and 
 * BAL_VREC_PLAYBACK_COMPLETE_MSG messages.
 */
typedef PACKED struct
{
   BalAppStatusT            ReasonCode;
} BalVmemVrecPlayCompleteMsgT;

/* For BAL_VMEMO_REC_STOP_MSG message */
typedef PACKED struct
{
   BalAppStatusT            ReasonCode;
} BalVmemVrecRecordCancelMsgT;

/*-----------------------------------------------------------------
 * The following message definitions are used by ETS for testing
 *----------------------------------------------------------------*/
typedef PACKED struct
{
  BalVmemoRecTypeT          RecordType;
  IpcSpchSrvcOptRateT       MaxRate;
  IpcSpchSrvcOptRateT       MinRate;
  char                      FileName [BAL_VMEM_MAX_FILENAME_LEN];
} BalVmemoRecStartMsgT;

typedef PACKED struct
{
  BalVmemoDataFormatT       DataFormat;  /* Voice packets or PCM samples */
  char                      FileName [BAL_VMEM_MAX_FILENAME_LEN];
} BalVmemoPlayStartMsgT;

 
 
/*-----------------------------------------------------------------
 * VMEMO global function 
 *----------------------------------------------------------------*/
#ifdef __cplusplus
    extern "C" {
#endif 

RegIdT BalVmemoRegister (BalEventFunc EventFuncP);
void   BalVmemoUnregister (RegIdT RegId) ;

BalAppStatusT BalVmemoPlayStart (BalVmemoDataFormatT DataFormat,
                                 char *FileNameP);

BalAppStatusT BalVmemoRecStart (BalVmemoRecTypeT     RecordType, 
                                BalVmemoMaxRateTypeT MaxRate, 
                                char  *FileNameP);

/*****************************************************************************

  FUNCTION NAME:   BalVmemoPause

  DESCRIPTION:     Processes a request to pause the current recording.

  PARAMETERS:      None

  RETURNED VALUES: None

*****************************************************************************/
void BalVmemoRecPause( void );

/*****************************************************************************

  FUNCTION NAME:   BalVmemoResume

  DESCRIPTION:     Processes a request to resume a recording that is currently paused.


  PARAMETERS:      None

  RETURNED VALUES: None

*****************************************************************************/
void BalVmemoRecResume( void );
void BalVmemoPlayPause (void);
void BalVmemoPlayResume (void);

void BalVmemoStop (void);

bool BalVmemoIsInProgress(void);
BalAppStatusT BalVmemoTotalTimeGet (BalVmemoDataFormatT DataFormat,  
                                char *FileNameP,
                                uint32 *MilliSecond);

#ifdef __cplusplus
    }
#endif /* __cplusplus */


/*-----------------------------------------------------------------
 * Functions used by LMD
 *----------------------------------------------------------------*/
bool BalVmemoGetPlaybackData(uint32 offset, uint8 *DstBuf, uint16 Size);


#endif


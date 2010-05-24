#ifndef _HWDAPPSAPI_H_
#define _HWDAPPSAPI_H_


#include "balapi.h"
#include "balfsiapi.h"
#ifdef CUST_H
#include "custbtc.h"
#endif   
/*---------------------------------------------------------------
*  Global Definitions
*---------------------------------------------------------------*/
/* Apps device types */
typedef enum
{
   HWD_APPS_AUDIO_VOICE_DEVICE,
   HWD_APPS_AUDIO_MIDI_DEVICE,
   HWD_APPS_AUDIO_MMAPPS_DEVICE,
   HWD_APPS_AUDIO_DEV_MODE_NUM
} HwdAppsAudioDevModesT;

/* MMApps music types */       
typedef enum
{
    HWD_MMAPPS_MP3_MUSIC,
    HWD_MMAPPS_NUM_MUSIC_TYPES
} HwdMMAppsMusicTypeT;

/* HWD_MMAPPS_CHAN_DATA_MSG */
typedef PACKED struct
{
   uint16      ChannelId;
   uint16      DataSize;         /* Size in WORDS of data     */
   uint16      *DataP;           /* Use IpcDspAppChanDataT    */
} HwdMMAppsChanDataMsgT;

/* Callback definition for sound-over and get-music-buf */
typedef void (*HwdAppsSoundOverFuncT) (BalAppStatusT Status);
typedef uint8 *(*HwdMMAppsMusicGetBufFuncT) (uint32 BufSize);

/*---------------------------------------------------------------
*  Apps Audio Functions
*---------------------------------------------------------------*/
extern void   HwdAppsAudioModeSet( HwdAppsAudioDevModesT DevMode, HwdAudioModeT Mode, 
                                   ExeRspMsgT *RspMsgP );
extern uint16 HwdAppsAudioGetDtmfDelay (void);
extern void   HwdAppsAudioRegisterCB (HwdAppsSoundOverFuncT FuncP);
#ifdef DENALI_AUDIO
extern void   HwdAppsRngrAudioModeSwitch (bool IsSwitch);
#endif
/*---------------------------------------------------------------
*  MIDI Functions
*---------------------------------------------------------------*/
extern uint8 HwdMidiSetupMelody( const uint8* SoundPtr, uint32 Size, uint8 Iterations, bool Tone );
extern bool  HwdMidiPlay( uint8 NextPlaySongIndex, int8 NumIterations );
extern bool  HwdMidiDtmf( BalSndDtmfIdT DtmfId, uint8 Mode );
extern void  HwdMidiTone( BalSndToneIdT ToneId );
extern void  HwdMidiStop (IpcDsvRngrCntrlModeEnum RngrCtrlMode);

/*---------------------------------------------------------------
*  iMelody Functions
*---------------------------------------------------------------*/
extern bool  HwdIMelodyPlay( uint8 NextPlaySongIndex, int8 NumIterations );
extern void  HwdIMelodyStop (void);
extern bool  HwdCMFPlay( uint8 NextPlaySongIndex, int8 NumIterations );
extern void  HwdCMFStop (void);

/*---------------------------------------------------------------
 *  MMApps Functions
 *---------------------------------------------------------------*/
extern bool HwdMMAppsMusicPlay (HwdMMAppsMusicTypeT MusicType, 
                                uint8              *DataP, 
                                BalFsiHandleT       FileHandle,
                                uint32              FileSize, 
                                uint8               NumIterations, 
                                uint32              BaseByteOffset, 
                                uint32              StartingByteOffset);
extern void HwdMMAppsMusicStop (void);
extern void HwdMMAppsMusicFastFwd (uint16 PlayTime, uint16 AdvanceTime);
extern void HwdMMAppsMusicRewind (uint16 PlayTime, uint16 RewindTime);
extern void HwdMMAppsMusicSuspend (void);
extern void HwdMMAppsMusicResume (void);
extern void HwdMMAppsMusicStatusReg (ExeTaskIdT TaskId, ExeMailboxIdT Mailbox, uint32 MsgId);
extern void HwdMMAppsMusicGetBufRegCB (HwdMMAppsMusicGetBufFuncT FuncP);

extern bool HwdMMAppsMidiActivate (void);
extern void HwdMMAppsMidiDeact (void);
extern void HwdMMAppsAbortAll (void);

extern ExeHisrT HwdMMAppsHisrCb;

/*---------------------------------------------------------------
 *  VOCODER Functions
 *---------------------------------------------------------------*/
extern void HwdMMAppsVocoderDnld (IpcSpchSrvcOptT SrvcOpt, bool ForceDnld);
extern bool HwdMMAppsSetVocoder (IpcSpchSrvcOptT SrvcOpt);
extern IpcSpchSrvcOptT HwdMMAppsGetVocoder (void);


/*---------------------------------------------------------------
*  External Micronas MP3 Functions 
*---------------------------------------------------------------*/
extern bool HwdExtMp3Play (void         *DataP, 
                           BalFsiHandleT FileHandle,
                           uint32        FileSize,
                           uint8         NumIterations,
                           uint32        BaseOffset,
                           uint32        StartingOffset);
extern void HwdExtMp3Stop (void);
#endif   /* _HWDAPPSAPI_H_  */


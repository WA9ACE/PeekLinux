/*=====================================================================================
FILE NAME   : valsndapi.h
MODULE NAME : ASL

  
GENERAL DESCRIPTION
	
Copyright (c) 2008-2009 by BTC. All Rights Reserved.
=======================================================================================
Revision History
	  
Modification                   Tracking
Date              Author       Number       Description of changes
----------   --------------   ---------   --------------------------------------
2008-04-02     Broad Zou                       create the file
		
=====================================================================================*/

#ifndef __BALSOUNDAPI_H__
#define __BALSOUNDAPI_H__


#ifdef  __cplusplus
extern "C" {
#endif

#include "sysdefs.h"
#include "hwdaudioapi.h"
#include "syskeydef.h"
#include "balkeypad.h"
#include "balfsiapi.h"
#ifdef CUST_H
#include "custbtc.h"
#endif 

#define BAL_SOUND_CONTINUES (0xFF)
#define BAL_SOUND_BURST     (1)

/* Event ID's used for registered sound callbacks */ 
#define BAL_SND_END_MUSIC_EVENT       1
#define BAL_SND_MUSIC_STATUS_EVENT    2

#define BAL_RINGER_DURATION_FOREVER (0xFFFFFFFF)
#define BAL_VIBRATE_DURATION_FOREVER (0xFFFFFFFF)
 
#define BAL_VMEMO_RECORD_END_EVENT    3
#define BAL_VMEMO_PLAYBACK_END_EVENT  4

#define SOUND_ITERATION_FOREVER  BAL_SOUND_CONTINUES
#define SOUND_DURATION_FOREVER  BAL_RINGER_DURATION_FOREVER
/*-----------------------------------------------------------------
 *	valaudio.c interface
 *----------------------------------------------------------------*/
typedef enum {
    BAL_AUDIO_HANDSET_MODE,	       /* default, internal spkr and mic */
    BAL_AUDIO_MONO_HEADSET_MODE,   /* mono headset mode */
    BAL_AUDIO_HANDSFREE_MODE,      /* headset mode */
    BAL_AUDIO_CDS_MODE,		       /* headset mode */
    BAL_AUDIO_LOUDSPKR_MODE,       /* loudspeaker mode */
    BAL_AUDIO_STEREO_HEADSET_MODE, /* stereo headset */
    BAL_AUDIO_EDAI_MODE,           /* Ext Digital codec  */

		/*
		 *	add new modes here, and modify tables in
		 *	hwdaudio.c to handle new setups and volume controls
		 *	for each new mode
		 */
    BAL_AUDIO_TTY_MODE,        
    BAL_AUDIO_TTY_VCO_MODE,        /* TTY device, Voice Carry Over */
    BAL_AUDIO_TTY_HCO_MODE,        /* TTY device, Hearing Carry Over */
    BAL_AUDIO_RINGER_MODE,  /* Mode used for Polyphonic Ringer 
                               MUST be next to last in enum list */
    BAL_AUDIO_NUM_MODES		/* must be last; used for loops and arrays */
} BalAudioModeT;

/* volume, dtmf and playback mode are in valapi.h */

/* NOTE: Some tone sequences are predefined in valmidi.c and indexed here.
* If this list is changed, must also update MidiFileList in valmidi.c.
*/
typedef enum {
   BAL_AUDIO_TONE_NULL,	/* to stop playback */
   BAL_AUDIO_TONE_DIAL,
   BAL_AUDIO_TONE_RING_BACK,
   BAL_AUDIO_TONE_INTERCEPT,
   BAL_AUDIO_TONE_INTERCEPT_ABBR,
   BAL_AUDIO_TONE_NWK_CONGESTION,
   BAL_AUDIO_TONE_NWK_CONGESTION_ABBR,
   BAL_AUDIO_TONE_BUSY,
   BAL_AUDIO_TONE_CONFIRM,
   BAL_AUDIO_TONE_CALL_WAITING,
   BAL_AUDIO_TONE_ALERT,
   BAL_AUDIO_TONE_POWER_UP,
   BAL_AUDIO_TONE_SMS_ALERT,
   BAL_AUDIO_TONE_REORDER,
   BAL_AUDIO_TONE_ANSWER_HOLD,
   BAL_AUDIO_TONE_POWER_UP_FANCY,
   BAL_AUDIO_TONE_POWER_OFF,
   BAL_AUDIO_TONE_BEEP,
   BAL_AUDIO_TONE_EARBEEP,
   BAL_AUDIO_TONE_EXTERNAL_POWER,
   BAL_AUDIO_TONE_SERVICE_ALERT,
   BAL_AUDIO_TONE_DTACO_TONE,
   BAL_AUDIO_TONE_OTASP_ALERT,

   /* no midi files yet */
#if 0
   BAL_AUDIO_TONE_USER_DATA,
#endif

   BAL_AUDIO_NUM_TONES
} BalSndToneIdT;

/* dtmf id; used to index entries in dtmf tone table  */
typedef enum {		
   BAL_SOUND_DTMF_0 = 0,
   BAL_SOUND_DTMF_1,
   BAL_SOUND_DTMF_2,
   BAL_SOUND_DTMF_3,
   BAL_SOUND_DTMF_4,
   BAL_SOUND_DTMF_5,
   BAL_SOUND_DTMF_6,
   BAL_SOUND_DTMF_7,
   BAL_SOUND_DTMF_8,
   BAL_SOUND_DTMF_9,
   BAL_SOUND_DTMF_STAR,
   BAL_SOUND_DTMF_POUND,
   BAL_SOUND_DTMF_A,
   BAL_SOUND_DTMF_B,
   BAL_SOUND_DTMF_C,
   BAL_SOUND_DTMF_D,
   /* single tone id; used to index entries in single tone keys table  */
   BAL_SOUND_SINGLE_LEFT,
   BAL_SOUND_SINGLE_RIGHT,
   BAL_SOUND_SINGLE_UP,
   BAL_SOUND_SINGLE_DOWN,
   BAL_SOUND_SINGLE_CLEAR,
   BAL_SOUND_SINGLE_SOFT_LEFT,
   BAL_SOUND_SINGLE_SOFT_RIGHT,
   BAL_SOUND_SINGLE_SELECT,
   BAL_SOUND_SINGLE_SEND,
   BAL_SOUND_SINGLE_END,
   BAL_SOUND_SINGLE_POWER,
   BAL_SOUND_SINGLE_CAMERA,
   BAL_SOUND_SINGLE_VOLUME_UP,
   BAL_SOUND_SINGLE_VOLUME_DOWN,
   BAL_SOUND_SINGLE_RECORD,

   /* "Stop DTMF" should always be last */
   BAL_SOUND_DTMF_STOP,
   BAL_SOUND_DTMF_NUM
} BalSndDtmfIdT;

#define MIDI_CHANNEL_MONO      1 /* add temp zouda */
#define MIDI_CHANNEL_STEREO   2 /* add temp zouda */
 
typedef PACKED struct {
   bool           Play;   /* If TRUE, DTMF is played; if FALSE, DTMF is stopped */
   BalSndDtmfIdT  DtmfId;
} BalDtmfToneGenMsgT;

/* Midi Tone message */
typedef PACKED struct
{
    uint8  ToneIdx;           /* Use BalSndToneIdT */
} BalMidiToneMsgT;

typedef enum
{
  BAL_KEY_SOUND_OFF = 0,
  BAL_KEY_SOUND_SHORT,
  BAL_KEY_SOUND_LONG
} BalSndKeySoundT;

typedef struct 
{
   void * pData;
   uint32 Iteration;
   uint32 Duration;
} BalSoundMsgT;

typedef enum
{
   BAL_SOUND_PATH_CALL_SPEAKER  = 1,
   BAL_SOUND_PATH_RINGER_SPEAKER = 2,
  
   BAL_SOUND_PATH_MICROPHONE = 4

} BalSoundPathT;

typedef enum
{
   BAL_SOUND_DEVICE_HANDSET         = 0,  /* Internal DAC, Diff output */
   BAL_SOUND_DEVICE_MONO_HEADSET    = 1,  /* Single-ended Aux receiver */
   BAL_SOUND_DEVICE_HANDSFREE       = 2,
   BAL_SOUND_DEVICE_EDAI            = 3,
   BAL_SOUND_DEVICE_LOUDSPKR        = 4,  /* PDM or External DAC */
   BAL_SOUND_DEVICE_STEREO_HEADSET  = 5,  /* External DAC Headphone */
   BAL_SOUND_DEVICE_CDS             = 6,
   BAL_SOUND_DEVICE_TTY             = 7,
   BAL_SOUND_DEVICE_TTY_VCO         = 8,
   BAL_SOUND_DEVICE_TTY_HCO         = 9,
   BAL_SOUND_DEVICE_I2S_HANDSET     = 10, /* External DAC earpiece  */
   BAL_SOUND_DEVICE_NUM
} BalSoundDeviceT;

typedef enum
{
  BAL_SOUND_VOLUME_INCREASE = HWD_AUDIO_VOL_UP,
  BAL_SOUND_VOLUME_DECREASE = HWD_AUDIO_VOL_DOWN,
  BAL_SOUND_VOLUME_UNMUTE  = HWD_AUDIO_VOL_UNMUTE,
  BAL_SOUND_VOLUME_MUTE     = HWD_AUDIO_VOL_MUTE,
  BAL_SOUND_VOLUME_LEVEL_1  = HWD_AUDIO_VOL_1,
  BAL_SOUND_VOLUME_LEVEL_2  = HWD_AUDIO_VOL_2,
  BAL_SOUND_VOLUME_LEVEL_3  = HWD_AUDIO_VOL_3,
  BAL_SOUND_VOLUME_LEVEL_4  = HWD_AUDIO_VOL_4,
  BAL_SOUND_VOLUME_LEVEL_5  = HWD_AUDIO_VOL_5,
  BAL_SOUND_VOLUME_LEVEL_6  = HWD_AUDIO_VOL_6,
  BAL_SOUND_VOLUME_LEVEL_7  = HWD_AUDIO_VOL_7,
  BAL_SOUND_VOLUME_LEVEL_8  = HWD_AUDIO_VOL_8,
  BAL_SOUND_VOLUME_DEFAULT  = BAL_SOUND_VOLUME_LEVEL_3,
  BAL_SOUND_VOLUME_MAX      = BAL_SOUND_VOLUME_LEVEL_8,
  BAL_SOUND_VOLUME_CURRENT
} BalSoundVolumeT;

typedef enum
{
   BAL_SOUND_FORMAT_TONE = 0,
   BAL_SOUND_FORMAT_KEY_TONE,
   BAL_SOUND_FORMAT_MIDI_MELODY,
   BAL_SOUND_FORMAT_MP3_MELODY,
   BAL_SOUND_FORMAT_IMELODY,
   BAL_SOUND_FORMAT_MICRONAS_MP3,
  BAL_SOUND_FORMAT_CMF,
   BAL_SOUND_FORMAT_NUM
} BalSoundFormatT;

typedef enum
{
   BAL_SOUND_STOP_IMMEDIATE = 0,
   BAL_SOUND_STOP_DECAY
} BalSoundStopT;

typedef enum
{
   BAL_SOUND_VIBRATE_WEAK = 0,
   BAL_SOUND_VIBRATE_MEDIUM,
   BAL_SOUND_VIBRATE_STRONG,
   BAL_SOUND_VIBRATE_NUM
} BalSoundVibrateT;

typedef enum
{
   BAL_SOUND_KEY_VOLUME = 0,
   BAL_SOUND_TONE_VOLUME,
   BAL_SOUND_RING_VOLUME,
   BAL_SOUND_MUSIC_VOLUME,
   BAL_SOUND_VOICE_VOLUME,
   BAL_SOUND_MIC_VOLUME,
   BAL_SOUND_VOLUME_NUM
} BalSoundVolumeModesT;

typedef enum
{
   BAL_SOUND_VOICE_DEVICE = 0,
   BAL_SOUND_RINGER_DEVICE,
   BAL_SOUND_MUSIC_DEVICE,
   BAL_SOUND_DEV_MODE_NUM
} BalSoundDeviceModesT;

/* data for BAL_SND_END_MUSIC_EVENT */
typedef struct
{
   BalSoundFormatT   MusicPlayed;  /* Midi or MP3 */
   BalAppStatusT     ReturnCode;   /* pass/fail reason */
   void              *DataPtr;     /* data buffer used for music 
                                      if applicable, otherwise - NULL */
   BalFsiHandleT     FileHandle;   /* file handle used for music 
                                      if applicable, otherwise - (-1) */
} BalSndEventMsgT;

/* data for BAL_SND_MUSIC_STATUS_EVENT */
typedef enum
{
    BAL_SND_MUSIC_RUNNING,
    BAL_SND_MUSIC_SUSPENDED,
    BAL_SND_MUSIC_FAST_FWD,
    BAL_SND_MUSIC_REWIND,
    BAL_SND_MUSIC_DONE,
    BAL_SND_MUSIC_FRAME_SYNC_NOT_FOUND,
    BAL_SND_MUSIC_LOSS_OF_FRAME_SYNC
} BalSndMusicStatusT;

typedef struct
{
    BalSndMusicStatusT Status;
    uint32             CurrByteOffset;
    uint32             FirstFrameOffsetBytes;
	uint32	           BitRate;
	uint16	           AvgBytesPerSecond;
	uint16	           SamplesPerSecond;
    uint16             LossOfFrameSyncErrCnt;
} BalMusicStatusEventMsgT;

/*=================================================================================
FUNCTION:
BalSetVolume
CREATE DATE:
2008-4-1
AUTHOR: 
Broad Zou
DESCRIPTION:
set volume.
ARGUMENTS PASSED: BalSoundVolumeModesT, BalSoundVolumeT
RETURN VALUE: bool
USED GLOBAL VARIABLES:
USED STATIC VARIABLES:
CALL BY:
IMPORTANT NOTES:
the function i.e side effect..etc
=================================================================================== */
bool BalSetVolume(BalSoundVolumeModesT VolMode, BalSoundVolumeT Volume);

/*=================================================================================
FUNCTION:
BalGetVolume
CREATE DATE:
2008-4-1
AUTHOR: 
Broad Zou
DESCRIPTION:
get the current volume.
ARGUMENTS PASSED: BalSoundVolumeModesT
RETURN VALUE: BalSoundVolumeT
USED GLOBAL VARIABLES:
USED STATIC VARIABLES:
CALL BY:
IMPORTANT NOTES:
the function i.e side effect..etc
=================================================================================== */
BalSoundVolumeT BalGetVolume(BalSoundVolumeModesT VolMode);

/*=================================================================================
FUNCTION:
byd_set_audiopath
CREATE DATE:
2008-4-1
AUTHOR: 
Broad Zou
DESCRIPTION:
set the current audio file path.
ARGUMENTS PASSED: int 
RETURN VALUE: void
USED GLOBAL VARIABLES:
USED STATIC VARIABLES:
CALL BY:
IMPORTANT NOTES:
the function i.e side effect..etc
=================================================================================== */
void byd_set_audiopath(int audioDevice);

/*=================================================================================
FUNCTION:
BalTonePlay
CREATE DATE:
2008-4-1
AUTHOR: 
Broad Zou
DESCRIPTION:
play the midi file strFileName.
ARGUMENTS PASSED: strFileName,(Format, Iterations now is not worked)
RETURN VALUE: uint16
USED GLOBAL VARIABLES:
USED STATIC VARIABLES:
CALL BY:
IMPORTANT NOTES:
the function i.e side effect..etc
=================================================================================== */
uint16 BalTonePlay(char *strFileName, BalSoundFormatT Format, uint8 Iterations);

uint16 BalMemPlay(uint8 type, UINT32*  address, UINT32  size, uint8 Iterations);
/* stop play music file */
void BalToneStop();

/*
FUNCTION:
BalReset
CREATE DATE:
2008-4-24
AUTHOR: 
ZhangFangHui
DESCRIPTION:
Reset phone.
RETURN VALUE: int 0 means success.
*/
int Bal_Reset();
#ifdef  __cplusplus
}
#endif

#endif /* __BALSOUNDAPI_H__ */




#ifndef _HWD_AUDIO_API_H_
#define _HWD_AUDIO_API_H_

#include "hwdapi.h"
#include "sysdefs.h"
#include "ipcapi.h"

#define HWD_VC_CP_SELECT      0x00
#define HWD_VC_DV_SELECT      0x01

#define HWD_VC_REGSEL         HWD_VC_CP_SELECT

#define HWD_AUDIO_DURATION_FOREVER 0xFFFF
   /* regardless of mode, a dtmf tone must play for 95 ms and rest for 35 ms */
#define  DTMF_TONE_TIME 115   /* AMPS needs 115 ms durastion */
#define  DTMF_PAUSE_TIME   35    /* msec */

   /*
    * Volume settings are used for several DSPV volume controls
    * Pay attention when modifying the number of settings, and think
    * first if your goal could not be better accomplished by altering
    * the volume-to-DSPV mapping in hwdaudio.c rather than changing
    * the number (or definitions) of volume steps.
    */
typedef enum
{
   HWD_AUDIO_VOL_UNCHG  = -4,
   HWD_AUDIO_VOL_UP     = -3,
   HWD_AUDIO_VOL_DOWN   = -2,
   HWD_AUDIO_VOL_UNMUTE = -1,
   HWD_AUDIO_VOL_MUTE   = 0,  /* easier to cast form int... */
   HWD_AUDIO_VOL_1      = 1,
   HWD_AUDIO_VOL_2,
   HWD_AUDIO_VOL_3,
   HWD_AUDIO_VOL_4,
   HWD_AUDIO_VOL_5,
   HWD_AUDIO_VOL_6,
   HWD_AUDIO_VOL_7,
   HWD_AUDIO_VOL_8,
   HWD_AUDIO_VOL_DEFAULT= HWD_AUDIO_VOL_3,
   HWD_AUDIO_VOL_MAX    = HWD_AUDIO_VOL_8
} HwdAudioVolT;

   /* audio mode definitions */
typedef enum {
    HWD_AUDIO_MODE_UNCHNG = -1,    /* mode unchanged */
    HWD_AUDIO_HANDSET_MODE = 0,    /* default, internal spkr and mic */
    HWD_AUDIO_MONO_HEADSET_MODE,   /* mono headset mode */
    HWD_AUDIO_HANDSFREE_MODE,      /* headset mode */
    HWD_AUDIO_CDS_MODE,            /* handset test mode, no LNA */
    HWD_AUDIO_LOUDSPKR_MODE,       /* loudspeaker mode */
    HWD_AUDIO_STEREO_HEADSET_MODE, /* stereo headset */
    HWD_AUDIO_EDAI_MODE,           /* Ext Digital codec  */
    HWD_AUDIO_TTY_MODE,            /* TTY device dazhong */
    HWD_AUDIO_TTY_VCO_MODE,        /* TTY device, Voice Carry Over */
    HWD_AUDIO_TTY_HCO_MODE,        /* TTY device, Hearing Carry Over */
    HWD_AUDIO_I2S_HANDSET_MODE,    /* External DAC earpiece */

      /*
       * add new modes here, and modify tables in
       * hwdaudio.c to handle new setups and volume controls
       * for each new mode
       */

    HWD_AUDIO_RINGER_MODE,  /* Mode used for Polyphonic Ringer
                               MUST be next to last in enum list */
    HWD_AUDIO_NUM_MODES    /* must be last; used for loops and arrays */
} HwdAudioModeT;

    /* volume mode definitions: each volume type requires different
     *                          volume messages and parameters */
typedef enum
{
    HWD_AUDIO_VOICE_VOL_MODE,
    HWD_AUDIO_RINGER_VOL_MODE,
    HWD_AUDIO_MUSIC_VOL_MODE
} HwdVolumeModeT;

#if (SYS_BOARD == SB_CDS4)
#define HWD_AUDIO_DEFAULT_MODE HWD_AUDIO_CDS_MODE
#else
#define HWD_AUDIO_DEFAULT_MODE HWD_AUDIO_HANDSET_MODE
#endif
#define HWD_AUDIO_DEFAULT_VOL_MODE  HWD_AUDIO_VOICE_VOL_MODE

   /* Audio devices id used to build logical paths (bitmask) */
#define  HWD_AUDIO_DEV_SPKR   0x01
#define  HWD_AUDIO_DEV_MIC 0x02
 /* Audio devices id used to build device map (bitmask) */
 #define	HWD_AUDIO_DEV_MAIN_SPKR	0x01
 #define	HWD_AUDIO_DEV_AUX_SPKR	0x02
 #define	HWD_AUDIO_DEV_RINGER_SPKR 0x04
 
 #define	HWD_AUDIO_DEV_MAIN_MIC	0x10
 #define	HWD_AUDIO_DEV_AUX_MIC	0x20
 
 #define HWD_AUDIO_DEV_MAP_UNCHNG    0xff
   /* available tone generators, per device (mic/spkr) */
typedef enum {
   HWD_AUDIO_VOICE_0 = 0,
   HWD_AUDIO_VOICE_1,
   HWD_AUDIO_VOICE_2,
   HWD_AUDIO_VOICE_3,
   HWD_AUDIO_VOICE_4,
   HWD_AUDIO_VOICE_5,

   HWD_AUDIO_VOICE_NUM  /* max is defined in ipcapi.h as IPC_NUM_TONE_GEN */
} HwdAudioVoiceT;

/* tone data */
typedef struct {
   uint16         Freq; /* tone frequency (Hz) */
   uint16         Dur;  /* tone duration (ms) */
   HwdAudioVoiceT Voice;   /* tone generator involved */
   HwdAudioVolT   Vol;  /* tone volume (1...10) */
} HwdAudioToneT;

/* operation typedef for HwdAudioL1AAudioCtrl */
typedef enum {
   HWD_AUDIO_L1AAUDIOCTRLOP_UNMUTE = HWD_AUDIO_VOL_UNMUTE,
   HWD_AUDIO_L1AAUDIOCTRLOP_MUTE   = HWD_AUDIO_VOL_MUTE,
   HWD_AUDIO_L1AAUDIOCTRLOP_UNCH /* unchanged */
} HwdAudioL1AAudioCtrlOpT;

typedef enum
{
   HWD_AUDIO_RNGR_SAMPLE_RATE_8KHZ   = 8,
   HWD_AUDIO_RNGR_SAMPLE_RATE_16KHZ  = 16,
   HWD_AUDIO_RNGR_SAMPLE_RATE_24KHZ  = 24,
   HWD_AUDIO_RNGR_SAMPLE_RATE_32KHZ  = 32,
   HWD_AUDIO_RNGR_SAMPLE_RATE_48KHZ  = 48
}HwdAudioRingerSampleRateT;

/*----------------------------------------------------------------------------
 Global Function Prototypes
----------------------------------------------------------------------------*/

extern void          HwdAudioInit( void );
extern void          HwdAudioModeSet( HwdAudioModeT Mode, HwdVolumeModeT VolMode,
                                      ExeRspMsgT *RspInfo );
extern HwdAudioModeT HwdAudioModeGet( void );
extern void          HwdAudioPathSet( bool Enabled, uint16 LogicalPath );
extern uint16        HwdAudioPathGet( void );
extern void          HwdAudioPathCfgResp (IpcCpAudioPathCfgRspMsgT *MsgP);
extern void          HwdAudioVolumeSet( uint16 LogicalPath, HwdAudioVolT Volume );
extern void          HwdAudioVolumeDataSet( uint16 LogicalPath, HwdAudioVolT Volume );
extern bool          HwdAudioGetLoopbackState (void);
extern void          HwdAudioToneVolumeLow (bool VolLow);
extern void          HwdAudioRingerVolumeHigh (bool VolHi);
extern void          HwdAudioRingerVolumeConstant (bool VolConstant);

extern HwdAudioVolT  HwdAudioVolumeGet( uint16 LogicalPath );
extern void          HwdAudioDtmfMute (bool Mute);
extern void          HwdAudioEdaiModeEnable ( HwdAudioEdaiEnableMsgT *EdaiEnableMsgP );
extern void          HwdAudioEdaiModeDisable ( void );
extern void          HwdAudioL1DTstGetPhoneStatus( uint16 *TxMuted, uint16 *RxMuted );
extern void          HwdAudioL1AAudioCtrl( HwdAudioL1AAudioCtrlOpT TxMuteOp,
                                          HwdAudioL1AAudioCtrlOpT RxMuteOp );
extern void          HwdAudioVibrateStart (uint32 VibrateTime, uint32 RestTime, int32 TotDuration);
extern void          HwdAudioVibrateStop (void);
extern void          HwdAudioVibrateEnable( bool Enable );
extern void          HwdAudioDspvAcpConfigure(HwdAudioModeT Mode);
extern void          HwdAudioDspvAecConfigure(HwdAudioModeT Mode);
extern void          HwdAudioDspvTtyConfigure( IpcTtyModeT TtyMode );
extern void          HwdAudioDspvVapConfigure(void);
extern void          HwdAudioDspvMicAnsConfigure(void);
extern void          HwdAudioDspvMicAnsModeSet( bool AnsOn );

extern bool          HwdAudioGetLoopbackState (void);
extern void          HwdAudioRingerInit( uint16 MaxNumVoices,
                                HwdAudioRingerSampleRateT SampleRate,
                                bool IntrpFilterMode,
                                IpcDsvDtmfPathEnum DtmfPath
                              );


extern void          HwdAudioExtAmpEnable( bool Enable );
extern void          HwdAudioExtDacEnable( bool Enable, HwdAudioModeT Mode );

extern void          HwdSetSpkrVolStartIdx (uint16 Idx, uint16 Divider, uint16 Step);
extern void          HwdSetMicVolStartIdx (uint16 Idx, uint16 Divider, uint16 Step);

#endif /* _HWD_AUDIO_API_H_ */



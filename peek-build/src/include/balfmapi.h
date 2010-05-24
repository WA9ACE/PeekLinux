
 //#ifdef SUPPORT_FM
 #ifndef _BAL_FM_API_H_
 #define _BAL_FM_API_H_
 
 //#include "sysdefs.h"
 //#include "hwdfm5760.h"
 #include "balfsiapi.h"
 #include "hwdsiliconfm.h"
 
 #include "custbtc.h"

 #ifdef DENALI_FM_AUDIO_PATH
 typedef enum
{
   FM_NOT_USED                  =0,
   FM_STEREO_HEADSET      = 1,
   FM_LOUDSPEAKER            = 2,
   FM_DEVICE_MAX              = 3                
} BalFMDeviceT;
#endif /*DENALI_FM_AUDIO_PATH*/

#ifdef DENALI_FM_HEADSET_KEY_SEEK
 typedef enum
{
   FM_HEADSETKEY_SEEK_DIS                  =0,   
   FM_HEADSETKEY_SEEK_EN                    =1	
} BalFMHeadsetKeySeekT;
#endif /*DENALI_FM_HEADSET_KEY_SEEK*/
 
 #ifdef __cplusplus
 extern "C"
 {
 #endif /* __cplusplus */
 extern void valFmCspSet( FmChSpT chSp );
 extern void valFmVolSet ( uint8 volume ); 
 extern uint16 valFmVolGet (void); /* bit 7 = 1, output enable, else output disable*/
 
 extern void valFmBbSet(uint16 min, uint16 max);
 extern uint8 valFmseek (uint8 seekup);
 
 extern uint8 valFmRssiGet(void);
 extern void valFmRssiThrSet(uint8 threshold);
 extern void valFmMonoSet(bool mono);
 extern uint8 valFmStereoGet(void);
 extern void valFmInit (void);
 extern void valFmPaly(void);
 extern void valFmStop(void);
 extern uint16 valFmFrqGet(void);
 extern bool  valFmFrqSet(uint16 frequency);/* TRUE: seek failure*/
 #ifdef DENALI_FM_AUDIO_PATH
 extern void BalSetFmDevice(BalFMDeviceT Device);
extern  BalFMDeviceT BalGetFmDevice(void);
#endif /*DENALI_FM_AUDIO_PATH*/

#ifdef DENALI_FM_HEADSET_KEY_SEEK
extern void BalFMHeadsetkeySeek(BalFMHeadsetKeySeekT Enable);
#endif /*DENALI_FM_HEADSET_KEY_SEEK*/
 #ifdef __cplusplus
 }
 #endif /* __cplusplus */
 #endif
 
 //#endif
